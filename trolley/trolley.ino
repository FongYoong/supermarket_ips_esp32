#include "src/Utilities.h"
#include "src/GlobalVariables/GlobalVariables.h"
#include "src/Pins/Pins.h"
#include "src/WifiUtils/WifiUtils.h"
#include "src/FirebaseUtils/FirebaseUtils.h"
#include "src/BluetoothUtils/BluetoothUtils.h"
#include "src/UserInterface/UserInterface.h"
#include "src/Vlc/Vlc.h"

// Tasks
Utilities::RepeatingTask wifiReconnectTask(WIFI_RECONNECT_INTERVAL);
Utilities::RepeatingTask uiRefreshTask(UI_REFRESH_INTERVAL);

void setup() {
  Serial.begin(115200);
  Pins::setup();
  WifiUtils::setup();
  FirebaseUtils::setup();
  BluetoothUtils::setup();
  Vlc::setup();
  UserInterface::setup();
  // Uncomment below if Bluetooth uses too much memory
  //  Serial.println(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
}

void loop() {
  if ((WiFi.status() != WL_CONNECTED) && wifiReconnectTask.isReady())
  {
    WifiUtils::reconnect();
  }
  FirebaseUtils::run();
  if (GlobalVariables::trolleyEnabled)
  {
    Vlc::run();
  }
  if (uiRefreshTask.isReady())
  {
    UserInterface::renderUI(); // Display user interface
  }
  BluetoothUtils::run();
}
