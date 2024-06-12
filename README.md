# ESLAB-2024-Spring Parkour-like Game Design
Authors: B10901112 陳品翔、B10901156 柯育杰、B10901161 張梓安  

## Introduction
We designed a Parkour-like game similar to "Subway Surfers". But instead of controlling the role in the game by scrolling the screen, you have to move your body to control the role in the game. Please follow the procedure to set up the game in your own PC. If you have any problem, or want to know how the game was designed, refer to ``Report.pdf``.  

## Procedure
First, you need to import the following library to Mbed studio project:  

BSP: http://developer.mbed.org/teams/ST/code/BSP_B-L475E-IOT01/  

DSP: https://os.mbed.com/teams/mbed-official/code/mbed-dsp  

ism43362-driver: https://github.com/ARMmbed/wifi-ism43362/  

Second, find the file with following path in the project folder:  

mbed-dsp/CMSIS_dsp/TransformFunctions/arm_bitreversal2.S  

Before line 43, add the following line:  
```
#define __CC_ARM
```
### Motion Detection Part
First, you will need these files: ``main.cpp``, both ``motion_detection.cpp`` and ``motion_detection.h`` in the Motion Detection folder, and both ``my_filter.cpp`` and ``my_filter.h`` in the Filter folder. During executing ``main.cpp``, it will call the other four files, so make sure that you put all of these five files in the same folder.  

### Data Transfer Part
Create "mbed_app.json" in the project folder and copy the following code to it:
```
{
    "config": {
        "wifi-ssid": {
            "help": "WiFi SSID",
            "value": "\"YOURSSID\""
        },
        "wifi-password": {
            "help": "WiFi Password",
            "value": "\"YOURPASSWORD\""
        }
    },
    "target_overrides": {
        "*": {
            "nsapi.default-wifi-security": "WPA_WPA2",
            "nsapi.default-wifi-ssid": "\"YOURSSID\"",
            "nsapi.default-wifi-password": "\"YOURPASSWORD\""
        }
    }
}
```
YOURSSID and YOURPASSWORD should be replaces with your real wifi SSID and password respectively.

(Note that if your game host is not using its real IP address, the borad should be connected to the same wifi AP as the game host)

Finally, put ``my_wifi_sender.h`` and ``my_wifi_sender.cpp`` into project folder and modify the following part in ``my_wifi_sender.h``
```
// IP address of game server:
#define IP_ADDRESS "GAMESERVERIP"
// port of game server:
#define PORT 8000
```
GAMESERVERIP should be replaced with the IP address of host comupter.

(Note that some times you have to shutdown your firewall or that the connection cannot be set up)
### Game Part
First, open ``cmd/terminal`` and type ``pip install ursina`` to download Ursina.Then download the Game folder, or download the ``main.py`` and assests folder, and put them under a mutual folder. Then replace the IP address with your computer's IP address in line 268 in ``main.py``. After these steps you should able to execute ``main.py`` and enjoy this game.  
###
Game rules: Move the STM32 left, right, up, down and the character will follower the STM32 to do the corresponding motion. You have five lifes at the biginning, and your task is to dodge all kind of barriers such as trains, road blocks, and dragons (Notice that, dragon is 3 blocks wide, so you should move to the rightest or leftest rail to dodge it) and the speed will increase as your score get higher.There is an item looks like a white block with wings icom, if you get it you will fly to the sky for 5 second. Now, Try to earn more score before you are run over by train!
## Game Demo
This is our demo video link:



