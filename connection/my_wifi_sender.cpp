#include "my_wifi_sender.h"

void my_wifi_sender::connect_wifi(){
    ISM43362Interface* _net = &_wifi;
    nsapi_error_t ret = _net->connect(MBED_CONF_APP_WIFI_SSID,MBED_CONF_APP_WIFI_PASSWORD,NSAPI_SECURITY_WPA_WPA2);
    if(ret != 0){
        printf("error connecting to wifi!\n");
        return;
    }
    printf("connect to wifi successfully\n");
}

void my_wifi_sender::print_wifi_info(){
    printf("MAC: %s\n", _wifi.get_mac_address()); 
    printf("IP: %s\n", _wifi.get_ip_address()); 
    printf("Netmask: %s\n", _wifi.get_netmask()); 
    printf("Gateway: %s\n", _wifi.get_gateway()); 
    printf("RSSI: %d\n\n", _wifi.get_rssi());
}

void my_wifi_sender::connect_host(){
    ret = socket.open(&_wifi);
    if(ret != 0){
        printf("error opening a socket!\n");
        return;
    }
    if(!address.set_ip_address(IP_ADDRESS)){
        printf("error setting IP address\n");
        return;
    }
    address.set_port(PORT);
    ret = socket.connect(address);
    if(ret != 0){
        printf("error connecting to remote host!\n");
        printf("close socket!\n");
        socket.close();
        return;
    }
    printf("connected to host successfully!\n");
}

void my_wifi_sender::send_motion(char* motion){
    int len = sprintf(buffer,"{\"motion\":%s}", motion);
    nsapi_error_t response = socket.send(buffer,len);
    if(response <= 0){
            printf("error sending data to remote host!\n");
            socket.close();
            _wifi.disconnect();
            return;
    }
    return;
}