#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include "Arduino.h"
#include <WiFi.h>
// WiFi credentials
#define WIFI_SSID "D253@unifi"
#define WIFI_PASSWORD "66886688"
#define WIFI_RECONNECT_INTERVAL 10000 // Milliseconds // Interval between reconnecting Wi-Fi

class WifiUtils
{
public:
    static unsigned char wifiRetries;
    static void setup();
    static void reconnect();
    static void stationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
    static void stationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
    static void gotIP(WiFiEvent_t event, WiFiEventInfo_t info);
};

#endif