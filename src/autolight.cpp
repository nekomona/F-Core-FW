#include <autolight.hpp>
#include <peripheral.hpp>

float table_lux[] = {
    0*100, 0.5*100,  1*100,  4*100,  8*100, 388*100
};
// Correction for * 512 / 400
float table_en[] = {
//    7,   8, 12, 16, 20, 400
    8,   10, 15, 20, 25, 511
};
float table_slope[] = {
    4.0/100, 10.0/100, 1.667/100, 1.25/100, 1.2789/100
};

float interpolate_en(float lux) {
    if (lux < table_lux[0]) return table_en[0];

    for (int i = 1; i <= 5; ++i) {
        if (lux < table_lux[i]) {
            return table_en[i-1] + table_slope[i-1] * (lux - table_lux[i-1]);
        }
    }
    return table_en[5];
}

void AutoLight::reset(float lux) {
    lux_lowpass = lux;
}

void AutoLight::setAlpha(float alpha) {
    if (alpha < 0.0f) {
        lowpass_alpha = 0.0f;
    } else if (alpha > 0.99f) {
        lowpass_alpha = 0.99f;
    } else {
        lowpass_alpha = alpha;
    }
}

void AutoLight::setMul(int mul) {
    if (mul < 1) {
        ena_mul = 1;
    } else if (mul > 9) {
        ena_mul = 9;
    } else {
        ena_mul = mul;
    }
}

void AutoLight::inputLux(float lux) {
    lux_lowpass = lux_lowpass * lowpass_alpha + lux * (1-lowpass_alpha);
    interp_en = interpolate_en(lux_lowpass) * ena_mul;
    if (interp_en > 511) {
        interp_en = 511;
    }
    if (enabled) {
        peri.setLightEna(interp_en);
    }
}

void AutoLight::update() {
    if (enabled) {
        inputLux(peri.mlightval);
    }
}