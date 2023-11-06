/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using SPI to communicate
 4 or 5 pins are required to interface.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

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

#define debounce_delay 300

#define rotationButton 26
#define menuButton 25

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

typedef enum menuState {mainMenu, kmphMenu, mphMenu, distanceMenu} stateT;
stateT nextState[] = {kmphMenu, mphMenu, distanceMenu, mainMenu};
int menuState = menuState::mainMenu;
int rotations = 0;

volatile bool rotated = false;
volatile bool menuChange = false;
volatile unsigned long lastRotationTime = 0; // Variable to store the last rotation time in milliseconds.
volatile unsigned long timeDifference = 0; // Variable to store the time difference between the last two rotations in milliseconds.

void rotationHandler(){
    delayMicroseconds(debounce_delay);
    if (digitalRead(rotationButton) == LOW) {
        rotated = true;
        unsigned long currentTime = millis();
        timeDifference = currentTime - lastRotationTime;
        lastRotationTime = currentTime; // Update the last rotation time
    }
}

void menuHandler(){
    delayMicroseconds(debounce_delay);
    if (digitalRead(menuButton) == LOW) {
        menuChange = true;
    }
}

void showMenu() {
    switch (menuState) {
        case mainMenu:
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.println("Main Menu");
            display.display();
            
            break;
        case kmphMenu:
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            double kmph = getSpeedKmph(RPMFromTimeDifference(timeDifference), 20);
            display.println("kmph Menu");
            display.display();
            break;
        case mphMenu:
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.println("mph Menu");
            display.display();
            break;
        case distanceMenu:
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.println("distance Menu");
            display.display();
            break;
    }
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
    attachInterrupt(digitalPinToInterrupt(menuButton), menuHandler, FALLING);

  display.clearDisplay();
  display.display();
}


void loop() {
    showMenu();

    if (rotated) {
        rotated = false;
        rotations++;
        Serial.println("rotations: " + String(rotations));
        Serial.println("rotation time: " + String(timeDifference/1000.0) + "s");
    }

    if (menuChange) {
        menuChange = false;
        menuState = nextState[menuState];
        Serial.println("menu: " + String(menuState));
    }
}
