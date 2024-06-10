#ifndef MOTION_DETECTION_H
#define MOTION_DETECTION_H

#include "mbed.h"
#include "stm32l475e_iot01_accelero.h"
#include <cmath>
#include "my_filter.h"
#include "my_wifi_sender.h"

// Function prototypes
void motion_detection(float stm_x, float stm_y, float stm_z, float stm_all_x, float stm_all_y, float stm_all_z);
void process_data();
extern Timer t;
extern my_wifi_sender _wifi;

#endif // MOTION_DETECTION_H
