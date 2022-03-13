#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

namespace logger
{
    enum EVENTCATEGORIES
    {
        System,
        Conn,
        MqttMsg,
        NtpTime,
        Reboot,
        Authentication,
        Login,
        PageHandler,
        RefreshSunsetSunrise,
        ReadTemp,
        MqttParamChange,
        TemperatureInterval,
        Hall,
        DSTChange,
        TimeZoneChange,
        IRreceived,
        Lights,
        EntranceLight,
        PwmAutoChange,
        StaircaselightDelay,
        StaircaseLight,
        I2CButtonPressed,
        FriendlyNameChange,
        HeartbeatIntervalChange,
        Relay,
        Clock,
        ArchlightRotaryEncoderDirection,
        BoilerDelay,
        Boiler
    };

    extern void LogEvent(int Category, int ID, String Title, String Data);
}

#endif