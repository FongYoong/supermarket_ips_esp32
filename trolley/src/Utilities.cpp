#include "Utilities.h"
using namespace Utilities;

// RepeatingTask(unsigned long previousMillis, unsigned long interval) : previousMillis(previousMillis), interval(interval) {}

bool RepeatingTask::isReady()
{
    const bool ready = (millis() - previousMillis > interval) || previousMillis == 0;
    if (ready)
    {
        previousMillis = millis();
    }
    return ready;
}