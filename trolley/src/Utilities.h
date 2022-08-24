#ifndef UTILITIES_H
#define UTILITIES_H

#include "Arduino.h"

namespace Utilities
{

    class RepeatingTask
    {
    public:
        unsigned long previousMillis = 0;
        unsigned long interval;
        RepeatingTask(unsigned long interval) : interval(interval){};
        bool isReady();
    };

    String toCoordinatesString(double x, double y);
}

#endif
