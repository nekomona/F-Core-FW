#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include <Adafruit_INA219.h>

static const uint16_t GATTS_SERVICE_UUID_TEST      = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TEST_A       = 0xFF01;


#define FCORE_PWM_CHF 1
#define FCORE_PWM_CHB 0
#define FCORE_PWM_FREQ 100000

#define FCORE_PIN_SCL 8
#define FCORE_PIN_SDA 2


bool deviceConnected = false;

int16_t mvoltval = 0;
int16_t mampval = 0;

class ServerStatCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Connected");
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Disconnected");
    }
};

class CharacteristicStatCallbacks: public BLECharacteristicCallbacks {

    uint8_t vreplys[6] = {'V', ' ', ' ', ' ', ' ', '\0'};

    void onRead(BLECharacteristic *pCharacteristic) {
        Serial.println("Read");
    }

    int getFanval(char *ch) {
        int fanval = ((ch[0] - '0') % 10) * 10 + ((ch[1] - '0') % 10);
        if (fanval == 99) fanval = 100;
        return (fanval * 255) / 100;
    }

    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        
        // Serial.println("Write");
        if (value.length() > 0) {
            int fval;
            switch (value[0]) {
                case 'F':
                    fval = getFanval(&value[1]);
                    ledcWrite(FCORE_PWM_CHF, fval);

                    Serial.printf("Set fan-F %4d\n", fval);
                    break;
                case 'B':
                    fval = getFanval(&value[1]);
                    ledcWrite(FCORE_PWM_CHB, fval);

                    Serial.printf("Set fan-B %4d\n", fval);
                    break;
                case 'A':
                    fval = getFanval(&value[1]);
                    ledcWrite(FCORE_PWM_CHF, fval);
                    ledcWrite(FCORE_PWM_CHB, fval);

                    Serial.printf("Set fan-A %4d\n", fval);
                    break;
                case 'V':
                    vreplys[1] = mvoltval & 0xFF;
                    vreplys[2] = (mvoltval >> 8) & 0xFF;
                    vreplys[3] = mampval & 0xFF;
                    vreplys[4] = (mampval >> 8) & 0xFF;
                    pCharacteristic->setValue(vreplys, 6);
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

BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;

Adafruit_INA219 ina219;

void setup() {
    Serial.begin(115200);
    delay(2000);

    ledcSetup(FCORE_PWM_CHB, FCORE_PWM_FREQ, 8);
    ledcAttachPin(0, FCORE_PWM_CHB);
    ledcWrite(FCORE_PWM_CHB, 0);

    ledcSetup(FCORE_PWM_CHF, FCORE_PWM_FREQ, 8);
    ledcAttachPin(1, FCORE_PWM_CHF);
    ledcWrite(FCORE_PWM_CHF, 0);

    Wire.end();
    Wire.begin(FCORE_PIN_SDA, FCORE_PIN_SCL);

    ina219.begin();
    // Need to re-scale to 0.01 ohm
    ina219.setCalibration_16V_4A(); // Actually 16V, 4A
    Serial.print("INA current:");
    Serial.println(ina219.getCurrent_mA());

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

bool oldDeviceConnected = false;

void loop() {
    if (!deviceConnected && oldDeviceConnected) {
        delay(500);
        pServer->startAdvertising();
        Serial.println("Advertising");
        oldDeviceConnected = deviceConnected;
    }
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
    // Serial.print("Voltage:");
    // Serial.println(ina219.getBusVoltage_V());
    // Serial.print("Current:");
    // Serial.println(ina219.getCurrent_mA());

    mvoltval = (int16_t)(ina219.getBusVoltage_V() * 1000);
    mampval = (int16_t)(ina219.getCurrent_mA());

    delay(1000);
}
