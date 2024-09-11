#include <Arduino.h>

#include <ble.hpp>
#include <peripheral.hpp>

BLE ble;
Peripheral peri;

void setup() {
    Serial.begin(115200);
    delay(2000);

    ble.init();
    peri.init();
}


void loop() {
    ble.loop();
    peri.loop();

    delay(1000);
}
