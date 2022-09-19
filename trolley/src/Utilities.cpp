#include "Utilities.h"
using namespace Utilities;


bool RepeatingTask::isReady()
{
    const bool ready = (millis() - previousMillis > interval) || previousMillis == 0;
    if (ready)
    {
        previousMillis = millis();
    }
    return ready;
}

String Utilities::toCoordinatesString(double x, double y)
{
    return String(x, 6) + "," + String(y, 6);
}