#include "Vlc.h"

Utilities::RepeatingTask Vlc::photodiodeTask(PHOTODIODE_INTERVAL);
int Vlc::photodiodeValue = 0;

void Vlc::setup()
{

}

void Vlc::run()
{
    if (photodiodeTask.isReady())
    {
        photodiodeValue = analogReadMilliVolts(PHOTODIODE_OUT_PIN);
        Serial.println(photodiodeValue);
    }
}