#ifndef PINS_H
#define PINS_H

#include "Arduino.h"
#include "SSD1306Wire.h"

#define PHOTODIODE_OUT_PIN 34

class Pins
{
public:
    static SSD1306Wire oledDisplay;
    static void setup();
};

#endif