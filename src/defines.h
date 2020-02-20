#define MQTT_CUSTOMER "viktak"
#define MQTT_PROJECT  "spiti"

#define HARDWARE_ID "WiFi-IR Gateway"
#define HARDWARE_VERSION "1.0"
#define SOFTWARE_ID "WiFi-IR Gateway "
#define FIRMWARE_VERSION "1.0"

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

//  IR key codes
#define IR_PWM_R_0 0x1067906F
#define IR_PWM_R_1 0x1067B04F
#define IR_PWM_R_2 0x10678877
#define IR_PWM_R_3 0x1067A857
#define IR_PWM_R_4 0x10679867
#define IR_PWM_R_5 0x1067B847
#define IR_PWM_R_6 0x1067827D
#define IR_PWM_R_7 0x1067A25D

#define IR_PWM_G_0 0x106710EF
#define IR_PWM_G_1 0x106730CF
#define IR_PWM_G_2 0x106708F7
#define IR_PWM_G_3 0x106728D7
#define IR_PWM_G_4 0x106718E7
#define IR_PWM_G_5 0x106738C7
#define IR_PWM_G_6 0x106702FD
#define IR_PWM_G_7 0x106722DD

#define IR_PWM_B_0 0x106750AF
#define IR_PWM_B_1 0x1067708F
#define IR_PWM_B_2 0x106748B7
#define IR_PWM_B_3 0x10676897
#define IR_PWM_B_4 0x106758A7
#define IR_PWM_B_5 0x10677887
#define IR_PWM_B_6 0x106742BD
#define IR_PWM_B_7 0x1067629D

