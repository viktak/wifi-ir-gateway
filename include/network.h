#ifndef NETWORK_H
#define NETWORK_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

namespace network
{
    extern ESP8266WebServer webServer;
    extern WiFiClient client;

    extern void InitWifiWebServer();

    extern void loop();
    extern void setup();
}

#endif