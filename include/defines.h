////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  Wifi-IR Gateway by Viktor Takacs is licensed under CC BY-NC-SA 4.0.
//  To view a copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0 
//
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef DEFINES_H
#define DEFINES_H

#define MQTT_CUSTOMER "viktak"
#define MQTT_PROJECT  "spiti"

#define HARDWARE_ID "WiFi-IR Gateway"
#define HARDWARE_VERSION "1.0"
#define SOFTWARE_ID "WiFi-IR Gateway "

#define DEBUG_SPEED 921600

#define JSON_SETTINGS_SIZE (JSON_OBJECT_SIZE(9) + 200)
#define JSON_MQTT_COMMAND_SIZE 300

#define CONNECTION_STATUS_LED_GPIO 0
#define HEARTBEAT_INTERVAL 60

#define IR_RECEIVE_GPIO 5
#define IR_SEND_GPIO 4

#define SDA_GPIO 13
#define SCL_GPIO 14

#define ACTIVITY_LED_GPIO 16

#endif