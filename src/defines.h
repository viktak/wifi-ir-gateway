#define MQTT_CUSTOMER "viktak"
#define MQTT_PROJECT  "spiti"

#define HARDWARE_ID "WiFi-IR Gateway"
#define HARDWARE_VERSION "1.0"
#define SOFTWARE_ID "WiFi-IR Gateway "
#define FIRMWARE_VERSION "1.1"

#define JSON_SETTINGS_SIZE (JSON_OBJECT_SIZE(9) + 200)
#define JSON_MQTT_COMMAND_SIZE 300

#define CONNECTION_STATUS_LED_GPIO 0
#define HEARTBEAT_INTERVAL 60

#define IR_RECEIVE_GPIO 5
#define IR_SEND_GPIO 4

#define SDA_GPIO 13
#define SCL_GPIO 14

#define i2c_io1_address 0x20
#define i2c_io2_address 0x21
#define i2c_io3_address 0x22

#define I2C_INT_GPIO 12

#define ACTIVITY_LED_GPIO 16
