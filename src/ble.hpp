#ifndef __BLE_HPP_
#define __BLE_HPP_

#include <BLEDevice.h>
#include <BLEServer.h>

static const uint16_t GATTS_SERVICE_UUID_TEST      = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TEST_A       = 0xFF01;

class BLE {
    BLEServer *pServer = nullptr;
    BLECharacteristic *pCharacteristic = nullptr;
    
public:
    bool deviceConnected = false;
    bool oldDeviceConnected = false;
    
    void init();
    void loop();
};

extern BLE ble;

#endif