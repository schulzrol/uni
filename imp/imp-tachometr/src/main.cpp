#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include <WebServer.h>

char ssid[] = "tachometr";  // network SSID (name)
char pass[] = "tachometr";  // network password
char hostname[] = "tachometr";  // arduino hostname
bool hideSSID = false;       // whether to NOT broadcast SSID

int status = WL_IDLE_STATUS;
String ip;
WebServer server(80);

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
#define tabHeight 10
#define margin 2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

typedef enum menuState {kmphMenu, mphMenu, distanceMenu, RPMMenu, serverMenu, menusCount} menuStateT;
menuStateT nextMenuState[] = {mphMenu, distanceMenu, RPMMenu, serverMenu, kmphMenu};
int menuState = menuState::kmphMenu;

typedef enum serverState         {serverOn, serverClose, serverOff, serverStartup} serverStateT;
serverStateT nextServerState[] = {serverClose, serverOff, serverStartup, serverOn};
int serverState = serverState::serverOff;

int rotations = 0;
bool APEnabled = false;

unsigned int connected_devices = 0;
auto prefs = Preferences();
volatile double distance = 0;
volatile bool eraseDistance = false;
volatile bool rotated = false;
volatile bool menuChange = false;
volatile bool serverChange = false;

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
        else if (wasLongPress && (menuState == serverMenu)){
            serverState = nextServerState[serverState];
        }
        else {
            menuState = nextMenuState[menuState];
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

void showTabs(){
    // split the screen into menusCount tabs
    int tabWidth = SCREEN_WIDTH / menusCount;
    // draw a rounded rectangle for each tab
    const int radius = 2;
    // fill a tab if it's the current menu
    for (int i = 0; i < menusCount; i++) {
        int x = i * tabWidth;
        int y = SCREEN_HEIGHT - tabHeight;
        display.drawRoundRect(x + margin, y + margin, tabWidth - 2 * margin, tabHeight - 2 * margin, radius, SSD1306_WHITE);
        if (i == menuState) {
            display.fillRoundRect(x + margin, y + margin, tabWidth - 2 * margin, tabHeight - 2 * margin, radius, SSD1306_WHITE);
        }
    }
    // draw a divider line between content and tabs with margin at the top and bottom
    display.drawLine(0, SCREEN_HEIGHT - tabHeight - margin, SCREEN_WIDTH, SCREEN_HEIGHT - tabHeight - margin, SSD1306_WHITE);

}

void printLabeledValue(double value, String label, String format="%5.1f"){
    // center the speed in the middle of the screen
    display.setCursor(0, 0);
    display.setTextSize(4);
    display.setTextColor(SSD1306_WHITE);
    display.printf(format.c_str(), value);
    // draw at the right edge of the screen below the speed
    int length = label.length();
    display.setCursor(SCREEN_WIDTH - (length*2*6), 32);
    display.setTextSize(2);
    display.printf(label.c_str());
}

void showMenu() {
    unsigned long currentTime = millis();
    unsigned long sinceLastRotation = currentTime - lastRotationTime;

    unsigned long timeDifference = max(lastRotationTimeDifference, sinceLastRotation);
    double RPM = RPMFromTimeDifference(timeDifference);
    display.clearDisplay();
    showTabs();

    switch (menuState) {
        case kmphMenu: {
            double kmph = getSpeedKmph(RPM, diameter);
            printLabeledValue(kmph, "Km/h");
            display.display();
            break;
        }
        case mphMenu: {
            double mph = KmphToMph(getSpeedKmph(RPM, diameter));
            printLabeledValue(mph, "Mph");
            display.display();
            break;
        }
        case distanceMenu: {
            double in_meters = distance/100;
            double in_km = in_meters/1000;
            if (in_km > 99) {
                printLabeledValue(in_km, "Km", "%5.1f");
            }
            else if (in_km > .9) {
                printLabeledValue(in_km, "Km", "%5.2f");
            } else {
                printLabeledValue(in_meters, "m", "%5.1f");
            }
            display.display();
            break;
        }

        case RPMMenu: {
            if (RPM > 999) {
                printLabeledValue(RPM, "RPM", "%5.0f");
            }
            else if (RPM > 99) {
                printLabeledValue(RPM, "RPM", "%5.1f");
            }
            else {
                printLabeledValue(RPM, "RPM", "%5.2f");
            }

            display.display();
            break;
        }
        case serverMenu: {
            if (serverState == serverOn) {
                // print ssid and password
                display.setCursor(0,0);
                display.setTextSize(1);
                display.setTextColor(SSD1306_WHITE);
                display.printf("SSID: %s", ssid);
                display.setCursor(0,10);
                display.printf("PASS: %s", pass);
                display.setCursor(0,20);
                display.printf("IP: %s", ip);
                display.setCursor(0,30);
                display.printf("Connected devices: %d", connected_devices);
                display.display();
                break;
            }
            else {
                printLabeledValue(0, "AP OFF", "");
                display.display();
                break;
            }
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

void eraseDistanceHandler(){
    distance = 0;
    eraseDistance = true;
    server.send(200, "text/html", "<h1>Distance erased</h1><p><a href=\"/\">Back</a></p>");
}

void adminPageHandler(){
    server.send(200, "text/html", "<h1>Tachometr Admin</h1><p>Traveled " + String(distance/100) + " meters. </p><p><a href=\"/erase_distance\">Erase distance</a></p>");
}

void setup() {
    Serial.begin(9600);
    initDataFromPersistent();

    // attempt to connect to Wifi network:
    int retries = 3;

    WiFi.setHostname(hostname);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, pass, 1, hideSSID);
    WiFi.enableAP(serverOn);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    Serial.println(String(getSpeedKmph(60, diameter)));
    server.on("/erase_distance", eraseDistanceHandler);
    server.on("/", adminPageHandler);
    pinMode(rotationButton, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(rotationButton), rotationHandler, FALLING);

    pinMode(menuButton, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(menuButton), menuPressHandler, FALLING);

    refreshDisplayTimer = timerBegin(0, 80, true);
    timerAttachInterrupt(refreshDisplayTimer, &showMenu, true);
    timerAlarmWrite(refreshDisplayTimer, 1000 * 1000, true); // Alarm for 1s
    timerAlarmEnable(refreshDisplayTimer);

    display.clearDisplay();
    display.display();
}

void loop() {
    if (rotated || eraseDistance) {
        saveDataToPersistent();
        rotated = false;
        eraseDistance = false;
    }

    switch (serverState){
        case serverOn: {
            connected_devices = WiFi.softAPgetStationNum();
            server.handleClient();
            break;
        }
        case serverOff: {
            break;
        }
        case serverStartup: {
            WiFi.enableAP(true);
            server.begin(80);
            ip = WiFi.softAPIP().toString();

            serverState = nextServerState[serverState];
            break;
        }
        case serverClose: {
            server.close();
            WiFi.enableAP(false);
            serverState = nextServerState[serverState];
            break;
        }
    }


}


