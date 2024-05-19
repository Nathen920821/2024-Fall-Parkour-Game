/*
 * A class wifi setting and data sending
 * 
 * This class helps to connect STM32 to Internet via wifi and send data to game host with TCP protocol
 * The sent data is in json format {"motion":MOTION_NAME} 
 *
 */
#ifndef __MY_WIFI_SENDER_H
#define __MY_WIFI_SENDER_H

#include "mbed.h"
#include "ISM43362Interface.h"
#include "TCPSocket.h"

// IP address of game server:
#define IP_ADDRESS "140.112.248.225"
// port of game server:
#define PORT 7777

class my_wifi_sender{
    public:
        my_wifi_sender() : _wifi(false){}

        /*
         * Connect to wifi AP
         *
         * You need to configure wifi SSID and password in mbed_app.json
         * The corresponding marco will be automatically define in mbed_config.h when compiling with mbed studio
         *
         */
        void connect_wifi();

        /*
         * Print information about wifi to which we connect 
         */
        void print_wifi_info();

        /*
         * connect to remote server (i.e. The game server)
         *
         * The IP address and the port is define with marco 
         * If the host change the IP address and port, the marco should be change correspondingly
         *
         */
        void connect_host();

        /*
         *send motion to game server with a single pair json format
         */
        void send_motion(char* motion);

    private:
        ISM43362Interface _wifi;
        TCPSocket socket;
        SocketAddress address;
        nsapi_error_t ret;
        
        //buffer for motion sending
        char buffer[1024];
};

#endif
