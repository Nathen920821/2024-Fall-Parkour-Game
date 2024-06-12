#include "motion_detection.h"

// Timer to track the 0.4s duration
Timer t;

my_wifi_sender _wifi;

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

        float32_t temp_in_arr_y[BUFFER_SIZE];
        float32_t temp_out_arr_y[BUFFER_SIZE];
        for (int i = 0; i < BUFFER_SIZE; i++) {
            temp_in_arr_y[i] = buffer[i][1];
        }

        float32_t temp_in_arr_z[BUFFER_SIZE];
        float32_t temp_out_arr_z[BUFFER_SIZE];
        for (int i = 0; i < BUFFER_SIZE; i++) {
            temp_in_arr_z[i] = buffer[i][2];
        }

        my_filter_apply(temp_in_arr_x, temp_out_arr_x, temp_in_arr_y, temp_out_arr_y, temp_in_arr_z, temp_out_arr_z);

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

        // Wait for 5 millisecond
        wait_us(5000);
    }
}

void motion_detection(float stm_x, float stm_y, float stm_z, float stm_all_x, float stm_all_y, float stm_all_z) {
    if (fabs(stm_x - stm_y) < 5 && fabs(stm_y - stm_z) < 8 && fabs(stm_z - stm_x) < 8) {
        // printf("STATIONARY\n");
    } else {
        if (stm_x > stm_y && stm_x > stm_z) {
            if (t.read_ms() >= 400) {
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
            if (t.read_ms() >= 400) {
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

int main() {
    _wifi.connect_wifi();
    _wifi.print_wifi_info();
    _wifi.connect_host();
    
    // Initialize accelerometer
    BSP_ACCELERO_Init();

    my_filter_init();
    
    t.start(); // Start the timer

    while (true) {
        // Main loop to process data
        process_data();
    }
}
