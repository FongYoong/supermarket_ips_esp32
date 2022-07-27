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
Utilities::RepeatingTask locationUpdateTask(GlobalVariables::locationUpdateInterval);

void setup() {
  Serial.begin(115200);
  Pins::setup();
  WifiUtils::setup();
  FirebaseUtils::setup();
  BluetoothUtils::setup();
  UserInterface::setup();
}

void loop() {
  if ((WiFi.status() != WL_CONNECTED) && wifiReconnectTask.isReady())
  {
    WifiUtils::reconnect();
  }
  if (uiRefreshTask.isReady())
  {
    UserInterface::renderUI(); // Display user interface
  }
  if (locationUpdateTask.isReady())
  {
    //Serial.println("Upload location");
  }
  Vlc::run();
  
  
}
