
#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "Arduino.h"
#include <WiFi.h>
#include "../GlobalVariables/GlobalVariables.h"
#include "../Pins/Pins.h"

#define UI_REFRESH_INTERVAL 500
#define QR_width 60
#define QR_height 60
#define QR_xOffset 30
#define Display Pins::oledDisplay

class UserInterface
{
public:
    // All UI Pages
    enum UIPage
    {
        PAGE_ENABLED,
        PAGE_DISABLED,
    };
    static void setup();
    static void renderUI();
    static void displayEnabledPage();
    static void displayDisabledPage();
};

#endif