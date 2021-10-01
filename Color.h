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

#ifndef _COLOR_H_
#define _COLOR_H_

#include <Adafruit_IS31FL3741.h>
#include <RTCLib.h>

namespace Color {

    inline uint16_t color565(uint32_t color888) {
        return Adafruit_IS31FL3741::color565(color888);
    }

    inline uint32_t hsv888(uint16_t hue, uint8_t sat = 255, uint8_t val = 255) {
        return Adafruit_IS31FL3741::ColorHSV(hue, sat, val);
    }

    inline uint16_t hsv565(uint16_t hue, uint8_t sat = 255, uint8_t val = 255) {
        return color565(hsv888(hue, sat, val));
    }    
    
    inline uint16_t hueForHour(uint8_t hour, uint8_t minute) {
        if (hour > 23) {
            hour = 23;
        }

        if (minute > 59) {
            minute = 59;
        }
        
        return map(hour * 60 + minute, 0, 24 * 60, 0, 65536);
    }

    inline uint16_t hueForMinute(uint8_t minute, uint8_t second) {
        if (minute > 59) {
            minute = 59;
        }

        if (second > 59) {
            second = 59;
        }
        
        return map(minute * 60 + second, 0, 60 * 60, 0, 65536);
    }

    inline uint16_t hueForSecond(uint8_t second, uint32_t ms = 0) {
        if (second > 59) {
            second = 59;
        }

        if (ms > 999) {
            ms = 999;
        }

        return map(second * 500 + (ms/2), 0, 60 * 500, 0, 65536);
    } 

    inline uint16_t hueForHour(const DateTime& t) {
        return hueForHour(t.hour(), t.minute());
    }

    inline uint16_t hueForMinute(const DateTime& t) {
        return hueForMinute(t.minute(), t.second());
    }

    inline uint16_t hueForSecond(const DateTime& t, uint32_t ms = 0) {
        return hueForSecond(t.second(), ms);
    }     

    inline uint16_t colorForHour(const DateTime& t) {
        return hsv565(hueForHour(t));
    }

    inline uint16_t colorForMinute(const DateTime& t) {
        return hsv565(hueForMinute(t));
    }

    inline uint16_t colorForSecond(const DateTime& t, uint32_t ms = 0) {
        return hsv565(hueForSecond(t, ms));
    }
}

#endif
