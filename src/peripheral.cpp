#include "peripheral.hpp"
#include "defines.h"

void Peripheral::init() {
    ledcSetup(FCORE_PWM_CHB, FCORE_PWM_FREQ, 8);
    ledcAttachPin(0, FCORE_PWM_CHB);
    ledcWrite(FCORE_PWM_CHB, 0);

    ledcSetup(FCORE_PWM_CHF, FCORE_PWM_FREQ, 8);
    ledcAttachPin(1, FCORE_PWM_CHF);
    ledcWrite(FCORE_PWM_CHF, 0);

    ledcSetup(FCORE_LED_CHENA, FCORE_LED_FREQ, 9);
    ledcAttachPin(FCORE_LED_PENA, FCORE_LED_CHENA);
    ledcWrite(FCORE_LED_CHENA, 0);

    ledcSetup(FCORE_LED_CHADJ, FCORE_LED_FREQ, 9);
    ledcAttachPin(FCORE_LED_PADJ, FCORE_LED_CHADJ);
    ledcWrite(FCORE_LED_CHADJ, 0);

    Wire.end();
    Wire.begin(FCORE_PIN_SDA, FCORE_PIN_SCL);

    ina219.begin();
    // Need to re-scale to 0.01 ohm
    ina219.setCalibration_16V_400mA(); // Actually 16V, 4A since Rsample is 1/10 smaller than 219 example
    // Serial.print("INA current:");
    // Serial.println(ina219.getCurrent_mA() * 10);
    
    apds.init();
    apds.enableLightSensor(false);
    apds.setAmbientLightGain(2);
}

void Peripheral::loop() {
    mvoltval = (int16_t)(ina219.getBusVoltage_V() * 1000);
    mampval = (int16_t)(ina219.getCurrent_mA() * 10);

    float ambf;
    apds.readAmbientLightLux(ambf);
    mlightval = (uint32_t)(ambf*100);
}

void Peripheral::setFanF(uint8_t v) {
    ledcWrite(FCORE_PWM_CHF, v);
}

void Peripheral::setFanB(uint8_t v) {
    ledcWrite(FCORE_PWM_CHB, v);
}

void Peripheral::setLightEna(uint8_t v) {
    ledcWrite(FCORE_LED_CHENA, v);
}

void Peripheral::setLightAdj(uint8_t v) {
    ledcWrite(FCORE_LED_CHADJ, v);
}

