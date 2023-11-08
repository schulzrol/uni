#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

hw_timer_t *refreshDisplayTimer = NULL;

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI  23
#define OLED_CLK   18
#define OLED_DC    27
#define OLED_CS    5
#define OLED_RESET 17

#define FLAG_VAL 0x42

#define diameter 50 //in cm

#define debounce_delay_millis 20
#define debounce_delay_micros debounce_delay_millis * 1000

#define LONG_BUTTON_PRESS_MILLIS 2000

#define rotationButton 26
#define menuButton 25

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

typedef enum menuState {kmphMenu, mphMenu, distanceMenu, RPMMenu} stateT;
stateT nextState[] = {mphMenu, distanceMenu, RPMMenu, kmphMenu};
int menuState = menuState::kmphMenu;
int rotations = 0;
auto prefs = Preferences();
volatile double distance = 0;
volatile bool eraseDistance = false;
volatile bool rotated = false;
volatile bool menuChange = false;

volatile unsigned long lastMenuPressTime = 0; // Variable to store the last menu press time in milliseconds.
volatile unsigned long lastRotationTime = 0; // Variable to store the last rotation time in milliseconds.
volatile unsigned long lastRotationTimeDifference = UINT_MAX;

double wheelCircumference(double d) {
    return d * PI;
}

double RPMFromTimeDifference(unsigned long timeDifferenceMillis) {
    return 60000.0 / timeDifferenceMillis; // 60000 milliseconds in a minute
}

double KmphToMph(double kmph) {
    return kmph * 0.621371; // 1 kmph = 0.621371 mph
}

double getSpeedKmph(double RPM, double wheelDiameterCm) {
    return (RPM * wheelDiameterCm * PI * 60) / 100000; // 100000 cm in a km and 60 minutes in an hour
}

void rotationHandler(){
    unsigned long currentTime = millis();
    delayMicroseconds(debounce_delay_micros);
    if (digitalRead(rotationButton) == LOW) {
        rotations++;
        rotated = true;
        distance += wheelCircumference(diameter);
        lastRotationTimeDifference = currentTime - lastRotationTime;
        lastRotationTime = currentTime; // Update the last rotation time
    }
}

void menuPressHandler();

void menuReleaseHandler(){
    unsigned long releaseTime = millis();
    delayMicroseconds(debounce_delay_micros);
    if (digitalRead(menuButton) == HIGH) {
        bool wasLongPress = (releaseTime - lastMenuPressTime) >= LONG_BUTTON_PRESS_MILLIS;
        if (wasLongPress && (menuState == distanceMenu)){
            distance = 0;
            eraseDistance = true;
        }
        else {
            menuState = nextState[menuState];
        }
        attachInterrupt(digitalPinToInterrupt(menuButton), menuPressHandler, FALLING);
    }

}

void menuPressHandler(){
    unsigned long currentTime = millis();
    delayMicroseconds(debounce_delay_micros);
    if (digitalRead(menuButton) == LOW) {
        lastMenuPressTime = currentTime;
        attachInterrupt(digitalPinToInterrupt(menuButton), menuReleaseHandler, RISING);
    }
}

unsigned int max(unsigned int a, unsigned int b){
    return a > b ? a : b;
}

void showMenu() {
    unsigned long currentTime = millis();
    unsigned long sinceLastRotation = currentTime - lastRotationTime;

    unsigned long timeDifference = max(lastRotationTimeDifference, sinceLastRotation);
    double RPM = RPMFromTimeDifference(timeDifference);
    switch (menuState) {
        case kmphMenu: {
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            double kmph = getSpeedKmph(RPM, diameter);
            display.printf("%3.1f km/h", kmph);
            display.display();
            break;
        }
        case mphMenu: {
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            double mph = KmphToMph(getSpeedKmph(RPM, diameter));
            display.printf("%3.1f m/h", mph);
            display.display();
            break;
        }
        case distanceMenu: {
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.printf("%.2f m", distance/100);
            display.display();
            break;
        }

        case RPMMenu: {
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.printf("%.2f RPM", RPM);
            display.display();
            break;
        }
    }
}

void initDataFromPersistent(){
    bool success = prefs.begin("distance", false);
    if (!success) {
        Serial.println("Failed to open preferences");
        for(;;); // Don't proceed, loop forever
    }
    if (prefs.isKey("distance")) {
        Serial.println("Found distance key");
        distance = prefs.getDouble("distance", 0);
    }
    else {
        Serial.println("Did not find distance key");
        prefs.putDouble("distance", 0);
    }
}

void saveDataToPersistent(){
    prefs.putDouble("distance", distance);
}

void setup() {
  Serial.begin(9600);
  initDataFromPersistent();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

    Serial.println(String(getSpeedKmph(60, diameter)));

    pinMode(rotationButton, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(rotationButton), rotationHandler, FALLING);

    pinMode(menuButton, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(menuButton), menuPressHandler, FALLING);

  refreshDisplayTimer = timerBegin(0, 80, true);
  timerAttachInterrupt(refreshDisplayTimer, &showMenu, true);
  timerAlarmWrite(refreshDisplayTimer, 1000*1000, true); // Alarm for 500ms
  timerAlarmEnable(refreshDisplayTimer);

  display.clearDisplay();
  display.display();
}

void loop(){
    if (rotated || eraseDistance) {
        saveDataToPersistent();
        rotated = false;
        eraseDistance = false;
    }
}
