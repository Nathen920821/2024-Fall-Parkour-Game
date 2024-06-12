#include "mbed.h"
#include "stm32l475e_iot01_accelero.h"
#include "my_filter.h"
#include "my_wifi_sender.h"
#include "motion_detection.h"

Timer t; // Timer to track the 0.4s duration

my_wifi_sender _wifi;

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
