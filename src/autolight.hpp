#ifndef __AUTOLIGHT_HPP_
#define __AUTOLIGHT_HPP_

class AutoLight {
    float lux_lowpass = 0.0f;

    void inputLux(float lux);

public:
    float lowpass_alpha = 0.1f;
    int interp_en = 0;
    int ena_mul = 1;
    bool enabled = false;

    void reset(float lux=0.0f);
    void setAlpha(float alpha);
    void setMul(int mul);
    void update();
};

extern AutoLight autoLight;

#endif
