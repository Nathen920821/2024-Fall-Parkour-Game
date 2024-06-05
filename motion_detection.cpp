#include "mbed.h"
#include "stm32l475e_iot01_accelero.h"
#include <cmath>
#include "arm_math.h"
#include "my_wifi_sender.h"

// Constants
#define BUFFER_SIZE 20
#define FILTER_TAP_NUM 29

// Function prototypes
void process_data();
void motion_detection(float stm_x, float stm_y, float stm_z, float stm_all_x, float stm_all_y, float stm_all_z);
void initFIRFilter();

Timer t; // Timer to track the 0.5s duration

arm_fir_instance_f32 fir_x, fir_y, fir_z;
float firStateF32_x[FILTER_TAP_NUM + BUFFER_SIZE - 1];
float firStateF32_y[FILTER_TAP_NUM + BUFFER_SIZE - 1];
float firStateF32_z[FILTER_TAP_NUM + BUFFER_SIZE - 1];

// FIR filter coefficients
const float32_t firCoeffs32[FILTER_TAP_NUM] = {
    0.0018225238f, 0.0020318917f, 0.0023229396f, 0.0026937495f, 0.0031397797f,
    0.0036549973f, 0.0042319678f, 0.0048619938f, 0.0055352264f, 0.0062408831f,
    0.0069674629f, 0.0077029880f, 0.0084352682f, 0.0091521361f, 0.0098416830f,
    0.0104925806f, 0.0110933467f, 0.0116327752f, 0.0121003282f, 0.0124866274f,
    0.0127839390f, 0.0129852983f, 0.0130855638f, 0.0130797380f, 0.0129651520f,
    0.0127432694f, 0.0124126540f, 0.0119761570f, 0.0114372370f
};

my_wifi_sender _wifi;

int main() {

    _wifi.connect_wifi();
    _wifi.print_wifi_info();
    _wifi.connect_host();
    
    // Initialize accelerometer
    BSP_ACCELERO_Init();

    initFIRFilter();
    
    t.start(); // Start the timer

    while (true) {
        // Main loop to process data
        process_data();
    }
}

void process_data() {
    // Buffer to hold the last 20 samples
    int16_t buffer[BUFFER_SIZE][3] = {0};
    int bufferIndex = 0;

    // Variables to store standard deviation and amplitude difference
    float stm_x = 0, stm_y = 0, stm_z = 0;
    float stm_diff[3] = {0};
    float stm_all[3] = {0};

    while (true) {
        int16_t pDataXYZ[3] = {0};
        BSP_ACCELERO_AccGetXYZ(pDataXYZ);

        // Calculate amplitude
        // stm_all[0] = pDataXYZ[0];
        // stm_all[1] = pDataXYZ[1];
        // stm_all[2] = pDataXYZ[2];

        // Calculate difference from the previous sample
        if (bufferIndex > 0) {
            stm_diff[0] = (buffer[(bufferIndex - 1) % BUFFER_SIZE][0] - pDataXYZ[0]);
            stm_diff[1] = (buffer[(bufferIndex - 1) % BUFFER_SIZE][1] - pDataXYZ[1]);
            stm_diff[2] = (buffer[(bufferIndex - 1) % BUFFER_SIZE][2] - pDataXYZ[2]);
        }

        // Store the current sample in the buffer
        buffer[bufferIndex % BUFFER_SIZE][0] = pDataXYZ[0];
        buffer[bufferIndex % BUFFER_SIZE][1] = pDataXYZ[1];
        buffer[bufferIndex % BUFFER_SIZE][2] = pDataXYZ[2];
        bufferIndex++;

        float32_t temp_in_arr_x[BUFFER_SIZE];
        float32_t temp_out_arr_x[BUFFER_SIZE];
        for (int i = 0; i < BUFFER_SIZE; i++) {
            temp_in_arr_x[i] = buffer[i][0];
        }
        arm_fir_f32(&fir_x, temp_in_arr_x, temp_out_arr_x, BUFFER_SIZE);

        float32_t temp_in_arr_y[BUFFER_SIZE];
        float32_t temp_out_arr_y[BUFFER_SIZE];
        for (int i = 0; i < BUFFER_SIZE; i++) {
            temp_in_arr_y[i] = buffer[i][1];
        }
        arm_fir_f32(&fir_y, temp_in_arr_y, temp_out_arr_y, BUFFER_SIZE);

        float32_t temp_in_arr_z[BUFFER_SIZE];
        float32_t temp_out_arr_z[BUFFER_SIZE];
        for (int i = 0; i < BUFFER_SIZE; i++) {
            temp_in_arr_z[i] = buffer[i][2];
        }
        arm_fir_f32(&fir_z, temp_in_arr_z, temp_out_arr_z, BUFFER_SIZE);

        for (int i = 0; i < BUFFER_SIZE; i++) {
            stm_all[0] = temp_out_arr_x[bufferIndex % BUFFER_SIZE];
            stm_all[1] = temp_out_arr_y[bufferIndex % BUFFER_SIZE];
            stm_all[2] = temp_out_arr_z[bufferIndex % BUFFER_SIZE];
        }

        // Calculate standard deviation every 20 samples
        if (bufferIndex >= BUFFER_SIZE) {
            float sumX = 0, sumY = 0, sumZ = 0;
            float meanX = 0, meanY = 0, meanZ = 0;
            float varX = 0, varY = 0, varZ = 0;

            for (int i = 0; i < BUFFER_SIZE; i++) {
                sumX += temp_out_arr_x[i];
                sumY += temp_out_arr_y[i];
                sumZ += temp_out_arr_z[i];
            }

            meanX = sumX / BUFFER_SIZE;
            meanY = sumY / BUFFER_SIZE;
            meanZ = sumZ / BUFFER_SIZE;

            for (int i = 0; i < BUFFER_SIZE; i++) {
                varX += pow(temp_out_arr_x[i] - meanX, 2);
                varY += pow(temp_out_arr_y[i] - meanY, 2);
                varZ += pow(temp_out_arr_z[i] - meanZ, 2);
            }

            stm_x = sqrt(varX / BUFFER_SIZE);
            stm_y = sqrt(varY / BUFFER_SIZE);
            stm_z = sqrt(varZ / BUFFER_SIZE);

            // Print standard deviation values
            // printf("Standard Deviation (10ms): X: %.2f, Y: %.2f, Z: %.2f\n", stm_x, stm_y, stm_z);

            // Print amplitude values
            // printf("Amplitude: X: %.2f, Y: %.2f, Z: %.2f\n", stm_all[0], stm_all[1], stm_all[2]);
            // printf("Difference: X: %.2f, Y: %.2f, Z: %.2f\n", stm_diff[0], stm_diff[1], stm_diff[2]);

            // Motion detection
            motion_detection(stm_x, stm_y, stm_z, stm_all[0], stm_all[1], stm_all[2]);
        }

        // Wait for 1 millisecond
        wait_us(1000);
    }
}

void motion_detection(float stm_x, float stm_y, float stm_z, float stm_all_x, float stm_all_y, float stm_all_z) {
    if (fabs(stm_x - stm_y) < 5 && fabs(stm_y - stm_z) < 5 && fabs(stm_z - stm_x) < 5) {
        // printf("STATIONARY\n");
    } else {
        if (stm_x > stm_y && stm_x > stm_z) {
            if (t.read_ms() >= 300) {
                if (stm_all_x < 0) {
                    printf("LEFT\n");
                    _wifi.send_motion("LEFT");
                } else {
                    printf("RIGHT\n");
                    _wifi.send_motion("RIGHT");
                }
                t.reset(); // Restart the timer
                t.start(); // Start the timer again after reset
            } else {
                // printf("STATIONARY\n");
            }
        } else if (stm_z > stm_x && stm_z > stm_y) {
            if (t.read_ms() >= 300) {
                if (stm_all_z < 200) {
                    printf("DOWN\n");
                    _wifi.send_motion("DOWN");
                } else {
                    printf("UP\n");
                    _wifi.send_motion("UP");
                }
                t.reset(); // Restart the timer
                t.start(); // Start the timer again after reset
            } else {
                // printf("STATIONARY\n");
            }
        }
    }
}

void initFIRFilter() {
    arm_fir_init_f32(&fir_x, FILTER_TAP_NUM, (float32_t *)&firCoeffs32[0], &firStateF32_x[0], BUFFER_SIZE);
    arm_fir_init_f32(&fir_y, FILTER_TAP_NUM, (float32_t *)&firCoeffs32[0], &firStateF32_y[0], BUFFER_SIZE);
    arm_fir_init_f32(&fir_z, FILTER_TAP_NUM, (float32_t *)&firCoeffs32[0], &firStateF32_z[0], BUFFER_SIZE);
}
