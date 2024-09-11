#ifndef __PERIPHERAL_HPP_
#define __PERIPHERAL_HPP_

#include <Adafruit_INA219.h>
#include <APDS9930.h>

#include <stdint.h>

class Peripheral {
private:
    Adafruit_INA219 ina219;
    APDS9930 apds;

public:
    uint16_t mvoltval;
    uint16_t mampval;
    uint32_t mlightval;

    void init();
    void loop();

    void setFanF(uint8_t v);
    void setFanB(uint8_t v);
    void setLightEna(uint8_t v);
    void setLightAdj(uint8_t v);
};

extern Peripheral peri;

#endif