#include <ble.hpp>

#include <Arduino.h>

#include <BLEUtils.h>
#include <BLE2902.h>

#include <peripheral.hpp>

class ServerStatCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        ble.deviceConnected = true;
        Serial.println("Connected");
    }

    void onDisconnect(BLEServer* pServer) {
        ble.deviceConnected = false;
        Serial.println("Disconnected");
    }
};

class CharacteristicStatCallbacks: public BLECharacteristicCallbacks {

    uint8_t vreplys[10] = {'V', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'};

    void onRead(BLECharacteristic *pCharacteristic) {
        Serial.println("Read");
    }

    int getFanval(char *ch) {
        int fanval = ((ch[0] - '0') % 10) * 10 + ((ch[1] - '0') % 10);
        if (fanval == 99) fanval = 100;
        return (fanval * 255) / 100;
    }

    int getLedval(char *ch) {
        int ledval = ((ch[0] - '0') % 10) * 10 + ((ch[1] - '0') % 10);
        if (ledval == 99) ledval = 100;
        return (ledval * 128) / 100;
    }

    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        
        if (value.length() > 0) {
            int fval;
            switch (value[0]) {
                case 'F':
                    fval = getFanval(&value[1]);
                    peri.setFanF(fval);

                    Serial.printf("Set fan-F %4d\n", fval);
                    break;
                case 'B':
                    fval = getFanval(&value[1]);
                    peri.setFanB(fval);

                    Serial.printf("Set fan-B %4d\n", fval);
                    break;
                case 'A':
                    fval = getFanval(&value[1]);
                    peri.setFanF(fval);
                    peri.setFanB(fval);

                    Serial.printf("Set fan-A %4d\n", fval);
                    break;
                case 'E':
                    fval = getLedval(&value[1]);
                    peri.setLightEna(fval);

                    Serial.printf("Set led-E %4d\n", fval);
                    break;
                case 'D':
                    fval = getLedval(&value[1]);
                    peri.setLightAdj(fval);

                    Serial.printf("Set led-D %4d\n", fval);
                    break;
                case 'V':
                    vreplys[1] = peri.mvoltval & 0xFF;
                    vreplys[2] = (peri.mvoltval >> 8) & 0xFF;
                    vreplys[3] = peri.mampval & 0xFF;
                    vreplys[4] = (peri.mampval >> 8) & 0xFF;
                    vreplys[5] = peri.mlightval & 0xFF;
                    vreplys[6] = (peri.mlightval >> 8) & 0xFF;
                    vreplys[7] = (peri.mlightval >> 16) & 0xFF;
                    vreplys[8] = (peri.mlightval >> 24) & 0xFF;
                    pCharacteristic->setValue(vreplys, 10);
                    pCharacteristic->notify();
                    // Serial.printf("Get battery %5d %5d\n", (uint16_t)mvoltval, (uint16_t)(mampval));
                    break;
                default:
                    {
                        Serial.println("*********");
                        Serial.print("New value: ");
                        for (int i = 0; i < value.length(); i++) {
                            Serial.print(value[i]);
                        }
                        Serial.println();
                        Serial.println("*********");
                    }
            }
        }
    }
};


void BLE::init() {
    BLEDevice::init("F-Core v1.9");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerStatCallbacks());
    BLEService *pService = pServer->createService(BLEUUID(GATTS_SERVICE_UUID_TEST));
    pCharacteristic = pService->createCharacteristic(
        BLEUUID(GATTS_CHAR_UUID_TEST_A),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->setCallbacks(new CharacteristicStatCallbacks());
    pCharacteristic->addDescriptor(new BLE2902());

    pCharacteristic->setValue("F-Core v1.9");
    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(BLEUUID(GATTS_SERVICE_UUID_TEST));
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}

void BLE::loop() {
    if (!deviceConnected && oldDeviceConnected) {
        pServer->startAdvertising();
        Serial.println("Advertising");
    }
    oldDeviceConnected = deviceConnected;
}