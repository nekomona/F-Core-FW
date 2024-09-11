#include <Arduino.h>

#include <ble.hpp>
#include <peripheral.hpp>
#include <autolight.hpp>

BLE ble;
Peripheral peri;
AutoLight autoLight;

void setup() {
    Serial.begin(115200);
    delay(2000);

    ble.init();
    peri.init();
}

uint32_t next_stamp = 0;
uint32_t curr_stamp;

void loop() {
    curr_stamp = millis();

    if (next_stamp == 0) {
        next_stamp = (curr_stamp % 100) + 100;
    }

    if (curr_stamp > next_stamp) {
        peri.loop();
        ble.loop();
        autoLight.update();

        next_stamp += 100;
    } else if (next_stamp - curr_stamp > 1) {
        delay(next_stamp - curr_stamp);
    }
}
