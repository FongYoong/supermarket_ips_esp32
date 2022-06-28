#ifndef VLC_H
#define VLC_H

#include "Arduino.h"
#include "../Utilities.h"
#include "../Pins/Pins.h"

#define PHOTODIODE_INTERVAL 500

class Vlc
{
public:
    static Utilities::RepeatingTask photodiodeTask;
    static int photodiodeValue;
    static void setup();
    static void run();
};

#endif