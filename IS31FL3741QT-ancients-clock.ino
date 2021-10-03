// Copyright 2021 Aaron Pendley
//
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.



// This was built with the Earle Philhower arduino-pico core (1.9.5).
// You'll need the following libraries installed:
// * Adafruit_NeoPixel (1.8.7 or later)
// * Adafruit_IS31FL3741 (1.2.0 or later)
// * RTCLib (Adafruit's fork, 1.14.1 or later)
//
// I used the following parts:
// * 1 QT Py RP2040 (https://www.adafruit.com/product/4900)
// * 2 IS31FL3741 13x9 Stemma QT LED Matrices from Adafruit (https://www.adafruit.com/product/5201)
// * 1 DS3231 Stemma QT RTC breakout (https://www.adafruit.com/product/5188)
// * 1 breadboard
// A few various Stemma QT cables (your needs may vary):
//   - 1 50mm Stemma QT cable (https://www.adafruit.com/product/4399)
//   - 1 100mm Stemma QT cable (https://www.adafruit.com/product/4210)
//   - 1 Stemma QT to male headers cable (https://www.adafruit.com/product/4209)
//
// CAREFUL! The power requirements for 2 13x9 RGB LED matrices can
// easily overwhelm the 500-600mA regulators on most microcontrollers.
// This firmware as provided should max out around 200mA on the QT Py RP2040,
// but if you change it or use a different microcontroller, you may need to
// figure out how to power the matrices externally or risk browning out your 
// microcontroller's regulator.



#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>
#include "Display.h"
#include "LoopTimer.h"
#include "Color.h"
#include "Ancient4x8.h"

// Uncomment to print the draw time (in milliseconds) to the serial port.
//#define PRINT_DRAW_TIME

// Use built-in NeoPixel on QT Py to show error states.
#define NEOPIXEL 12
#define NEOPIXEL_POWER 11
Adafruit_NeoPixel neopixel(1, NEOPIXEL, NEO_GRB + NEO_KHZ800);

// You may need to change these depending on your matrices and your microcontroller.
// I used a QT Py RP2040 and 2 BGR displays at the default address (1 per I2C bus).
Display display(&Wire, IS3741_ADDR_DEFAULT, IS3741_BGR,         // Left display
                &Wire1, IS3741_ADDR_DEFAULT, IS3741_BGR);       // Right display

// RTC driver
RTC_DS3231 rtc;

// Attempt to sync milliseconds for smooth color animation purposes.
LoopTimer loopTimer;
DateTime lastDateTime;
uint32_t msElapsed = 0;

void setup() {
    Serial.begin(115200);
    //while(!Serial);

    // For the RP2040 seems like we need to do this before we initialize our I2C devices.
    Wire.setClock(800000);
    Wire1.setClock(800000);

    setupRTC();
    setupDisplay();

    lastDateTime = rtc.now();
    msElapsed = 0;
    loopTimer.reset();
}

void loop() {
    uint32_t dt = loopTimer.update();
    DateTime now = rtc.now();

    // For the very first second we won't have an accurate 'elapsedMillis',
    // but after the first time the rtc second() property is updated,
    // we'll pretty darn close from then on.
    if (now == lastDateTime) {
        msElapsed += dt;
    } else {
        msElapsed = 0;
    }
    
    lastDateTime = now;

    #if defined(PRINT_DRAW_TIME)
    uint32_t beforeDraw = millis();
    #endif

    display.fill(0);
    drawTime(now, msElapsed);
    display.show();

    #if defined(PRINT_DRAW_TIME)
    uint32_t afterDraw = millis();
    Serial.println(afterDraw - beforeDraw);
    #endif
}

void drawTime(DateTime now, uint32_t ms) {
    display.setCursor(1, 8);

    display.setTextColor(Color::colorForHour(now));
    display.printf("%02u", now.hour());

    display.setTextColor(Color::colorForMinute(now));
    display.printf("%02u", now.minute());

    display.setTextColor(Color::colorForSecond(now, ms));
    display.printf("%02u", now.second());
}    

void setupRTC() {
    if (!rtc.begin()) {
        // Show white if RTC failed.
        failure(neopixel.Color(255, 255, 255));
    }    

    if (rtc.lostPower()) {
        Serial.println("RTC lost power, resetting date and time.");
        rtc.adjust(DateTime(2021, 1, 1, 0, 0, 0));
    }

    // When time needs to be re-set on a previously configured device, the
    // following line sets the RTC to the date & time this sketch was compiled.
    // NOTE: I added 20 seconds because that's about how long it takes
    // to compile this on my machine.
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + 20);
}

void setupDisplay() {
    auto error = display.begin();

    if (error) {
        // Show red if left matrix failed.
        uint32_t color = neopixel.Color(255, 0, 0);
        
        if (error == 2) {
            // Show yellow if right matrix failed.
            color = neopixel.Color(255, 255, 0);
        }
    
        failure(color);
    }

    display.setFont(&Ancient4x8);    
}

void failure(uint32_t color) {
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, HIGH);

    neopixel.begin();
    neopixel.setBrightness(8);     

    neopixel.fill(color);
    neopixel.show();
    
    while(true) {
        delay(500);
    }       
}
