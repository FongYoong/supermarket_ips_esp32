#ifndef BLUETOOTH_UTILS_H
#define BLUETOOTH_UTILS_H

#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "b82e3690-6032-4dfb-8b53-0bf1a8a14a53"
#define COORDINATE_CHAR_UUID "9d4f98c6-5459-420b-812f-dd255e636228"

class BluetoothUtils
{
public:
    static void setup();
};

#endif