// my_filter.h

#ifndef MY_FILTER_H
#define MY_FILTER_H

#include "arm_math.h"

// Constants
#define BUFFER_SIZE 20
#define FILTER_TAP_NUM 29

// Function prototypes
void my_filter_init();
void my_filter_apply(float32_t *inputX, float32_t *outputX, float32_t *inputY, float32_t *outputY, float32_t *inputZ, float32_t *outputZ);

// External variables
extern arm_fir_instance_f32 my_filter_x, my_filter_y, my_filter_z;
extern float my_filter_state_x[FILTER_TAP_NUM + BUFFER_SIZE - 1];
extern float my_filter_state_y[FILTER_TAP_NUM + BUFFER_SIZE - 1];
extern float my_filter_state_z[FILTER_TAP_NUM + BUFFER_SIZE - 1];

#endif // MY_FILTER_H
