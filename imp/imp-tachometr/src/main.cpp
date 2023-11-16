#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include <WebServer.h>
#include <WiFi.h>
#include <HTTPClient.h>

HTTPClient http;
String cloudMethod = "";
String asStationSSID = "iPhone";  // network SSID (name)
String asStationPASS = "12345678";  // network SSID (name)

char asAPSSID[] = "tachometr";  // network SSID (name)
char asAPPass[] = "tachometr";  // network password
char hostname[] = "tachometr";  // arduino hostname
bool hideSSID = false;       // whether to NOT broadcast SSID


String IP = "";
int status = WL_IDLE_STATUS;
unsigned int tries = 0;
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
#define AVG_OVER 10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


typedef enum menuState {speedMenu, distanceMenu, RPMMenu, serverMenu, menusCount} menuStateT;
menuStateT nextMenuState[] = {distanceMenu, RPMMenu, serverMenu, speedMenu};
int menuState = menuState::speedMenu;

typedef enum serverState         {serverAsAP, serverAsStationConnecting, serverAsStation, serverClose, serverOff, serverAsAPStartup} serverStateT;
serverStateT nextServerState[] = {serverAsStationConnecting, serverAsStation, serverClose, serverOff, serverAsAPStartup, serverAsAP};
int serverState = serverState::serverOff;

unsigned long lastDataSentMillis = 0;

int rotations = 0;
bool APEnabled = false;

bool memoryFull = false;
int memoryPosition = 0;
double lastKmphSpeeds[AVG_OVER] = {0};

volatile bool useMph = false;
unsigned int connected_devices = 0;
auto prefs = Preferences();
volatile double averageSpeedKmph = 0;
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

double updateAverageSpeedKmph(double newSpeed, double memory[AVG_OVER]=lastKmphSpeeds, int* position=&memoryPosition, bool* full=&memoryFull) {
    int n = (*full) ? AVG_OVER : (*position)+1;
    double sum = 0;

    memory[*position] = newSpeed;

    for (int i = 0; i < n; i++) {
        sum += memory[i];
    }
    if ((*position)+1 == AVG_OVER) {
        *full = true;
    }
    *position = ((*position)+1) % AVG_OVER;
    double average = std::max(sum, 0.) / (double) n;
    averageSpeedKmph = average;
    return average;
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
            if (serverState == serverAsStationConnecting) {
                serverState = serverClose;
            }
            else if (serverState == serverClose){
                // do nothing
            }
            else {
                serverState = nextServerState[serverState];
            }
        }
        else if (wasLongPress && (menuState == speedMenu)){
            useMph = !useMph;
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

void printLabeledValue(double value, String label, String format="%5.1f", double* labelValue=NULL){
    // center the speed in the middle of the screen
    display.setCursor(0, 0);
    display.setTextSize(4);
    display.setTextColor(SSD1306_WHITE);
    display.printf(format.c_str(), value);
    // draw at the right edge of the screen below the speed
    display.setTextSize(2);
    if (labelValue != NULL) {
        String separator = ";;";
        int separatorPosition = label.indexOf(separator);
        String valueFmt = label.substring(0, separatorPosition);
        String descLabel = label.substring(separatorPosition + separator.length());
        display.setCursor(0, 32);
        display.printf(valueFmt.c_str(), *labelValue);
        display.setCursor(SCREEN_WIDTH - (descLabel.length()*2*6), 32);
        display.printf(descLabel.c_str());
    }
    else {
        display.setCursor(SCREEN_WIDTH - (label.length()*2*6), 32);
        display.printf(label.c_str());
    }
}

void showMenu() {
    unsigned long currentTime = millis();
    unsigned long sinceLastRotation = currentTime - lastRotationTime;
    unsigned long lastRTD = lastRotationTimeDifference;

    unsigned long timeDifference = std::max(lastRTD, sinceLastRotation);
    double RPM = RPMFromTimeDifference(timeDifference);
    display.clearDisplay();
    showTabs();
    double kmphSpeed = getSpeedKmph(RPM, diameter);
    double avgKmphSpeed = updateAverageSpeedKmph(kmphSpeed);
    switch (menuState) {
        case speedMenu: {
            double speed, avgSpeed;
            String avgLabel;
            if (useMph) {
                avgSpeed = KmphToMph(avgKmphSpeed);
                speed = KmphToMph(kmphSpeed);
                avgLabel = "%5.1f;;Mph";
            }
            else {
                avgSpeed = avgKmphSpeed;
                speed = kmphSpeed;
                avgLabel = "%5.1f;;Km/h";
            }
            printLabeledValue(speed, avgLabel, "%5.1f", &avgSpeed);
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
            switch (serverState) {
                case serverAsAP: {
                    // print ssid and password
                    display.setCursor(0,0);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_WHITE);
                    display.printf("SSID: %s", asAPSSID);
                    display.setCursor(0,10);
                    display.printf("PASS: %s", asAPPass);
                    display.setCursor(0,20);
                    display.printf("IP: %s", IP.c_str());
                    display.setCursor(0,30);
                    display.printf("Connected devices: %d", connected_devices);
                    display.display();
                    break;
                }
                case serverAsStationConnecting: {
                    display.setCursor(0,0);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_WHITE);
                    display.printf("SSID: %s", asStationSSID);
                    display.setCursor(0,10);
                    display.printf("PASS: %s", asStationPASS);
                    display.setCursor(0,20);
                    display.printf("Connecting... %d", tries);
                    display.display();
                    break;
                }
                
                case serverAsStation: {
                    display.setCursor(0,0);
                    display.setTextSize(1);
                    display.setTextColor(SSD1306_WHITE);
                    display.printf("Connected to %s", asStationSSID);
                    display.setCursor(0,10);
                    display.printf("IP: %s", IP.c_str());
                    display.setCursor(0,20);
                    display.printf("Method: ");
                    display.setCursor(0,30);
                    display.printf("%s", cloudMethod.c_str());
                    display.display();
                    break;
                }
                
                case serverClose: {
                    printLabeledValue(0, "Closing...", "");
                    display.display();
                    break;
                }
                
                default: {
                    printLabeledValue(0, "SERVER OFF", "");
                    display.display();
                    break;
                }
            }
            break;
        }
    }
}

void initDataFromPersistent(){
    if (!prefs.begin("tachometr", false)) {
        Serial.println("Failed to open preferences");
        for(;;); // Don't proceed, loop forever
    }

    if (prefs.isKey("stationSSID")) {
        Serial.println("Found stationSSID key");
        asStationSSID = prefs.getString("stationSSID", asStationSSID);
    }
    else {
        Serial.println("Did not find stationSSID key. Initializing.");
        prefs.putString("stationSSID", asStationSSID);
    }

    if (prefs.isKey("stationPASS")) {
        Serial.println("Found stationPASS key");
        asStationPASS = prefs.getString("stationPASS", asStationPASS);
    }
    else {
        Serial.println("Did not find stationPASS key. Initializing.");
        prefs.putString("stationPASS", asStationPASS);
    }

    if (prefs.isKey("distance")) {
        Serial.println("Found distance key");
        distance = prefs.getDouble("distance", 0);
    }
    else {
        Serial.println("Did not find distance key. Initializing.");
        prefs.putDouble("distance", 0);
    }

    if (prefs.isKey("cloudMethod")) {
        Serial.println("Found cloudMethod key");
        cloudMethod = prefs.getString("cloudMethod", cloudMethod);
    }
    else {
        Serial.println("Did not find cloudMethod key. Leaving empty.");
    }
}

void saveDataToPersistent(){
    prefs.putDouble("distance", distance);
    prefs.putString("stationSSID", asStationSSID);
    prefs.putString("stationPASS", asStationPASS);
    prefs.putString("cloudMethod", cloudMethod);
}

void eraseDistanceHandler(){
    distance = 0;
    eraseDistance = true;
    server.send(200, "text/html", "<h1>Distance erased</h1><p><a href=\"/\">Back</a></p>");
}

void adminPageHandler(){
    double AvgSpeedKmph = averageSpeedKmph;
    server.send(200, "text/html",
    "<h1>Tachometr Admin</h1>" \
    "<p>Traveled " + String(distance/100) + " meters.</p>" \
    "<p>Avg speed " + String(AvgSpeedKmph) + " Km/h. </p>" \
    "<p><a href=\"/setup\">Setup</a></p>" \
    "<p><a href=\"/api\">Access API</a></p>" \
    "<p><a href=\"/erase_distance\">Erase distance</a></p>");
}

String dataAsJson(){
    return
    "{" \
    "\"distanceCM\": " + String(distance) + ", " \
    "\"avgSpeedKmph\": " + String(averageSpeedKmph) + "" \
    "\"}";
}

void apiHandler(){
    server.send(200, "text/json", dataAsJson());
}

void setupHandler(){
    if (server.args() > 0){
        if (server.hasArg("stationSSID")) { // ssid
            String ssid = server.arg("stationSSID");
            asStationSSID = ssid;
        }
        if (server.hasArg("stationPASS")) { // pass
            String pass = server.arg("stationPASS");
            asStationPASS = pass;
        }
        if (server.hasArg("cloudMethod")) { // pass
            String method = server.arg("cloudMethod");
            cloudMethod = method;
        }

        saveDataToPersistent();
        server.send(200, "text/html", "<h1>Setup saved</h1><p><a href=\"/\">Back</a></p>");
    } else {
        server.send(200, "text/html",
        "<h1>Tachometer as station setup</h1>" \
        "<form action=\"/setup\" method=\"POST\">" \
        "<label for=\"cloudMethod\">Cloud Method:</label><br>" \
        "<input type=\"text\" id=\"cloudMethod\" name=\"cloudMethod\" value=\"" + cloudMethod + "\"><br>" \
        "<label for=\"stationSSID\">SSID:</label><br>" \
        "<input type=\"text\" id=\"stationSSID\" name=\"stationSSID\" value=\"" + asStationSSID + "\"><br>" \
        "<label for=\"stationPASS\">Password:</label><br>" \
        "<input type=\"text\" id=\"stationPASS\" name=\"stationPASS\" value=\"" + asStationPASS + "\"><br><br>" \
        "<input type=\"submit\" value=\"Submit\">" \
        "</form>");
    }
}

void setup() {
    Serial.begin(9600);
    initDataFromPersistent();

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }


    WiFi.mode(WIFI_AP_STA);
    WiFi.persistent(false);
    WiFi.setAutoReconnect(false);

    Serial.println(String(getSpeedKmph(60, diameter)));
    server.on("/setup", setupHandler);
    server.on("/erase_distance", eraseDistanceHandler);
    server.on("/api", apiHandler);
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
        Serial.print("State :" + String(serverState) + "\n");
        saveDataToPersistent();
        rotated = false;
        eraseDistance = false;
    }
    switch (serverState){
        case serverOff: {
            break;
        }
        case serverAsAPStartup: {
            WiFi.enableAP(true);
            WiFi.softAP(asAPSSID, asAPPass, 1, hideSSID);
            server.begin(80);
            IP = WiFi.softAPIP().toString();
            serverState = nextServerState[serverState];
            break;
        }

        case serverAsAP: {
            connected_devices = WiFi.softAPgetStationNum();
            server.handleClient();
            break;
        }
        
        case serverAsStationConnecting: {
            WiFi.enableAP(false);
            WiFi.enableSTA(true);
            WiFi.begin(asStationSSID, asStationPASS);
            while (WiFi.status() != WL_CONNECTED) {
                if (tries > 120) { // 120 * 500ms = 60s
                    serverState = serverClose;
                    break;
                }
                tries++;
                delay(500);
            }
            tries = 0;
            IP = WiFi.localIP().toString();
            serverState = nextServerState[serverState];
            break;
        }

        case serverAsStation: {
            server.handleClient();
            // when lose connection, go back to connecting
            if (WiFi.status() != WL_CONNECTED) {
                serverState = serverAsStationConnecting;
            }

            String httpRequestData = dataAsJson();
            unsigned long currentTimeMillis = millis();
            if ((currentTimeMillis - lastDataSentMillis) >= 1000) {
                http.begin(cloudMethod);
                http.addHeader("Content-Type", "application/json");
                int httpResponseCode = http.POST(httpRequestData);
                http.end();
                lastDataSentMillis = millis();
            }
            break;
        }

        case serverClose: {
            WiFi.enableAP(false);
            WiFi.enableSTA(false);
            serverState = nextServerState[serverState];
            break;
        }
    }

}


