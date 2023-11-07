#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI  23
#define OLED_CLK   18
#define OLED_DC    27
#define OLED_CS    5
#define OLED_RESET 17

#define diameter 50 //in cm

#define debounce_delay_millis 20
#define debounce_delay_micros debounce_delay_millis * 1000

#define LONG_BUTTON_PRESS_MILLIS 500

#define rotationButton 26
#define menuButton 25

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

typedef enum menuState {kmphMenu, mphMenu, distanceMenu} stateT;
stateT nextState[] = {mphMenu, distanceMenu, kmphMenu};
int menuState = menuState::kmphMenu;
int rotations = 0;

double distance = 0;
volatile bool eraseDistance = false;
volatile bool rotated = false;
volatile bool menuChange = false;

volatile unsigned long lastMenuPressTime = 0; // Variable to store the last menu press time in milliseconds.
volatile unsigned long lastRotationTime = 0; // Variable to store the last rotation time in milliseconds.
volatile unsigned long timeDifference = INT_MAX; // Variable to store the time difference between the last two rotations in milliseconds.

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
        rotated = true;
        timeDifference = currentTime - lastRotationTime;
        lastRotationTime = currentTime; // Update the last rotation time
    }
}

void menuReleaseHandler(){
    unsigned long releaseTime = millis();
    delayMicroseconds(debounce_delay_micros);
    if (digitalRead(menuButton) == HIGH) {
        bool wasLongPress = (releaseTime - lastMenuPressTime) < LONG_BUTTON_PRESS_MILLIS;
        if (wasLongPress && (menuState == distanceMenu)){
            eraseDistance = true;
        }
        else {
            menuChange = true;
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

void showMenu() {
    switch (menuState) {
        case kmphMenu: {
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            double kmph = getSpeedKmph(RPMFromTimeDifference(timeDifference), diameter);
            display.println(String(kmph) + " km/h");
            display.display();
            break;
        }
        case mphMenu: {
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            double mph = KmphToMph(getSpeedKmph(RPMFromTimeDifference(timeDifference), diameter));
            display.println(String(mph) + " m/h");
            display.display();
            break;
        }
        case distanceMenu: {
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.println(String(distance / 100) + " m"); // 100 cm in a meter
            display.display();
            break;
        }
    }
}

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

    pinMode(menuButton, INPUT_PULLUP );
    pinMode(rotationButton, INPUT_PULLUP );
    attachInterrupt(digitalPinToInterrupt(rotationButton), rotationHandler, FALLING);
    attachInterrupt(digitalPinToInterrupt(menuButton), menuPressHandler, FALLING);

  display.clearDisplay();
  display.display();
}


void loop() {
    showMenu();

    if (eraseDistance) {
        eraseDistance = false;
        distance = 0;
        Serial.println("distance: " + String(distance));
    }

    if (rotated) {
        rotated = false;
        rotations++;
        distance += wheelCircumference(diameter);
        Serial.println("rotations: " + String(rotations));
        Serial.println("rotation time: " + String(timeDifference/1000.0) + "s");
    }

    if (menuChange) {
        menuChange = false;
        menuState = nextState[menuState];
        Serial.println("menu: " + String(menuState));
    }
}
