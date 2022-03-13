#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
#include <os_type.h>

namespace common
{
#define __debugSettings

    static const int32_t DEBUG_SPEED = 921600;
    static const String HARDWARE_ID = "WiFi-IR Gateway";
    static const String HARDWARE_VERSION = "1.0";
    static const String FIRMWARE_ID = "wifi-ir-gateway";

    extern String GetDeviceMAC();
    extern char *GetFullDateTime(const char *formattingString, size_t size);
    extern String DateTimeToString(const time_t time);
    extern String TimeIntervalToString(const time_t time);
    extern String GetDeviceMAC();

    extern void setup();
}

#endif