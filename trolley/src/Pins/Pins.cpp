#include "Pins.h"

SSD1306Wire Pins::oledDisplay(0x3c, SDA, SCL);

void Pins::setup()
{

}