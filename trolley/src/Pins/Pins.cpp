#include "Pins.h"

SSD1306Wire Pins::oledDisplay(0x3c, SDA, SCL);

void Pins::setup()
{
    // pinMode(button_left, INPUT_PULLUP);
    // pinMode(button_right, INPUT_PULLUP);
    // pinMode(button_enter, INPUT_PULLUP);
    // pinMode(button_back, INPUT_PULLUP);
    // pinMode(warning_LED, OUTPUT);
}