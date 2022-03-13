#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>

#include "network.h"

namespace mqtt
{
    extern os_timer_t heartbeatTimer;

    extern bool needsHeartbeat;

    extern PubSubClient PSclient;
    
    extern const char *mqttCustomer;
    extern const char *mqttProject;

    extern void PublishData(const char *topic, const char *payload, bool retained);

    extern void ConnectToMQTTBroker();
    extern void SendHeartbeat();

    extern void setup();
    extern void loop();

}

#endif