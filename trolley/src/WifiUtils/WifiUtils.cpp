#include "WifiUtils.h"

unsigned char WifiUtils::wifiRetries = 0;

void WifiUtils::setup()
{
  WiFi.disconnect(true); // delete old config
  WiFi.onEvent(stationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(stationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  WiFi.onEvent(gotIP, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (wifiRetries < 10 && WiFi.status() != WL_CONNECTED)
  {
    // Attempt to reconnect
    Serial.print(".");
    delay(300);
    wifiRetries++;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    wifiRetries = 0;
  }
  else
  {
    Serial.println("Operating in Offline Mode");
  }
}

void WifiUtils::reconnect() {
    Serial.println("Trying to reconnect to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
}

void WifiUtils::stationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Connected to AP successfully!");
}

void WifiUtils::stationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
}

void WifiUtils::gotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Connected with IP: ");
  Serial.print(WiFi.localIP());
  Serial.println();
}
