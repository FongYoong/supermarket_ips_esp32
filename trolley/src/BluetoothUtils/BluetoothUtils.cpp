#include "BluetoothUtils.h"

class ServerCallbacks: public NimBLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("BLE: Device connected");
    GlobalVariables::bleClients++;
    pServer->getAdvertising()->start();
  };
  void onDisconnect(BLEServer* pServer) {
    Serial.println("BLE: Device disconnected");
    GlobalVariables::bleClients--;
    pServer->getAdvertising()->start();
  }
};


void BluetoothUtils::setup()
{
    Serial.println("Initializing Bluetooth Low-Energy (BLE)");
    // Server
    NimBLEDevice::init(GlobalVariables::trolleyName);
    NimBLEServer *pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // Service
    NimBLEService *pService = pServer->createService(SERVICE_UUID);
    // Location characteristic
    NimBLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                            COORDINATE_CHAR_UUID,
                                            NIMBLE_PROPERTY::READ
                                        ); // BLECharacteristic::PROPERTY_WRITE
    pCharacteristic->setValue("-1.5, 3.6");
    pService->start();

    // Advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->start();

    // BLEDevice::init(GlobalVariables::trolleyName);
    // BLEServer *pServer = BLEDevice::createServer();
    // BLEService *pService = pServer->createService(SERVICE_UUID);
    // // Location characteristic
    // BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    //                                         COORDINATE_CHAR_UUID,
    //                                         BLECharacteristic::PROPERTY_READ
    //                                     ); // BLECharacteristic::PROPERTY_WRITE

    // pCharacteristic->setValue("-1.5, 3.6");
    // pService->start();
    // // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    // BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    // pAdvertising->addServiceUUID(SERVICE_UUID);
    // pAdvertising->setScanResponse(true);
    // pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    // pAdvertising->setMinPreferred(0x12);
    // BLEDevice::startAdvertising();
}