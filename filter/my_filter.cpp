// my_filter.cpp

#include "my_filter.h"

// FIR filter coefficients
const float32_t my_filter_coeffs[FILTER_TAP_NUM] = {
    0.0018225238f, 0.0020318917f, 0.0023229396f, 0.0026937495f, 0.0031397797f,
    0.0036549973f, 0.0042319678f, 0.0048619938f, 0.0055352264f, 0.0062408831f,
    0.0069674629f, 0.0077029880f, 0.0084352682f, 0.0091521361f, 0.0098416830f,
    0.0104925806f, 0.0110933467f, 0.0116327752f, 0.0121003282f, 0.0124866274f,
    0.0127839390f, 0.0129852983f, 0.0130855638f, 0.0130797380f, 0.0129651520f,
    0.0127432694f, 0.0124126540f, 0.0119761570f, 0.0114372370f
};

// FIR filter instances and states
arm_fir_instance_f32 my_filter_x, my_filter_y, my_filter_z;
float my_filter_state_x[FILTER_TAP_NUM + BUFFER_SIZE - 1];
float my_filter_state_y[FILTER_TAP_NUM + BUFFER_SIZE - 1];
float my_filter_state_z[FILTER_TAP_NUM + BUFFER_SIZE - 1];

void my_filter_init() {
    arm_fir_init_f32(&my_filter_x, FILTER_TAP_NUM, (float32_t *)&my_filter_coeffs[0], &my_filter_state_x[0], BUFFER_SIZE);
    arm_fir_init_f32(&my_filter_y, FILTER_TAP_NUM, (float32_t *)&my_filter_coeffs[0], &my_filter_state_y[0], BUFFER_SIZE);
    arm_fir_init_f32(&my_filter_z, FILTER_TAP_NUM, (float32_t *)&my_filter_coeffs[0], &my_filter_state_z[0], BUFFER_SIZE);
}

void my_filter_apply(float32_t *inputX, float32_t *outputX, float32_t *inputY, float32_t *outputY, float32_t *inputZ, float32_t *outputZ) {
    arm_fir_f32(&my_filter_x, inputX, outputX, BUFFER_SIZE);
    arm_fir_f32(&my_filter_y, inputY, outputY, BUFFER_SIZE);
    arm_fir_f32(&my_filter_z, inputZ, outputZ, BUFFER_SIZE);
}
