#include "BluetoothUtils.h"

void BluetoothUtils::setup()
{
    Serial.println("Initializing Bluetooth Low-Energy");
    BLEDevice::init("Trolley A");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    // Location characteristic
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                            COORDINATE_CHAR_UUID,
                                            BLECharacteristic::PROPERTY_READ
                                        ); // BLECharacteristic::PROPERTY_WRITE

    pCharacteristic->setValue("-1.5, 3.6");
    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}