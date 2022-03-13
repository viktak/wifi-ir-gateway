#include "version.h"
#include "common.h"
#include "settings.h"
#include "filesystem.h"
#include "network.h"
#include "mqtt.h"
#include "ota.h"
#include "connection.h"
#include "leds.h"
#include "ir.h"

void setup()
{
    Serial.begin(common::DEBUG_SPEED);
#ifdef __debugSettings
    delay(1000); //  Wait for PlatformIO serial monitor
#endif

    String FirmwareVersionString = String(FIRMWARE_VERSION) + " @ " + String(__TIME__) + " - " + String(__DATE__);

    Serial.printf("\r\n\n\nBooting ESP node %u...\r\n", ESP.getChipId());
    Serial.println("Hardware ID:      " + common::HARDWARE_ID);
    Serial.println("Hardware version: " + common::HARDWARE_VERSION);
    Serial.println("Software ID:      " + common::FIRMWARE_ID);
    Serial.println("Software version: " + FirmwareVersionString);
    Serial.println();

    filesystem::setup();
    settings::setup();
    common::setup();
    leds::setup();
    network::setup();
    ota::setup();
    mqtt::setup();
    ir::setup();

    //  Finished setup()
    Serial.println("Setup finished successfully.");
}

void loop()
{
    connection::loop();
}