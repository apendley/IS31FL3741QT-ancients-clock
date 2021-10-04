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

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <Print.h>
#include <Wire.h>
#include <Adafruit_IS31FL3741.h>

class Display: public Print {
public:

    Display(TwoWire* leftWire, uint8_t leftAddress, IS3741_order leftColorOrder, 
            TwoWire* rightWire, uint8_t rightAddress, IS3741_order rightColorOrder)
        : leftMatrix(leftColorOrder)
        , rightMatrix(rightColorOrder)
        , devices({
            {leftWire, &leftMatrix, leftAddress},
            {rightWire, &rightMatrix, rightAddress}
        })
    {

    }

    // Returns 0 if successful, otherwise the the display number that failed (1 or 2).
    uint8_t begin() {
        for (int d = 0; d < deviceCount; d++) {
            const Device& device = devices[d];
            
            if (!device.matrix->begin(device.address, device.wire)) {
                return d + 1;
            }
        }
    
        for (int d = 0; d < deviceCount; d++) {
            Adafruit_IS31FL3741_QT_buffered* matrix = devices[d].matrix;
            matrix->setLEDscaling(255);
            // Tone down the brightness so we don't overload the regulator.
            // Also because these things are blisteringly bright.
            matrix->setGlobalCurrent(8);
            matrix->enable(true);
            matrix->setRotation(0);
            matrix->setTextWrap(false);            
        }

        return 0;
    }

    void setBrightness(uint8_t brightness) {
        leftMatrix.setGlobalCurrent(brightness);
        rightMatrix.setGlobalCurrent(brightness);
    }

    void setFont(const GFXfont *f = NULL) {
        leftMatrix.setFont(f);
        rightMatrix.setFont(f);
    }

    void setTextColor(uint16_t c) { 
        leftMatrix.setTextColor(c);
        rightMatrix.setTextColor(c);
    } 
    
    void setTextColor(uint16_t c, uint16_t bg) {
        leftMatrix.setTextColor(c, bg);
        rightMatrix.setTextColor(c, bg);        
    }    

    void fill(uint16_t color) {
        leftMatrix.fill(color);
        rightMatrix.fill(color);
    }

    void setCursor(int16_t x, int16_t y) {
        leftMatrix.setCursor(x, y);
        rightMatrix.setCursor(x - leftMatrix.width(), y);
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) {
        leftMatrix.drawPixel(x, y, color);
        rightMatrix.drawPixel(x - leftMatrix.width(), y, color);
    }

    void show() {
        leftMatrix.show();
        rightMatrix.show();
    }

    virtual size_t write(uint8_t c) {
        leftMatrix.write(c);
        rightMatrix.write(c);
        return 1;
    }

private:

    Adafruit_IS31FL3741_QT_buffered leftMatrix;
    Adafruit_IS31FL3741_QT_buffered rightMatrix;

    static const int deviceCount = 2;

    struct Device {
        TwoWire* wire;
        Adafruit_IS31FL3741_QT_buffered* matrix;
        uint8_t address;
    };
    
    const Device devices[deviceCount];
};

#endif
