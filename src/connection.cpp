#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "ntp.h"
#include "settings.h"
#include "network.h"
#include "mqtt.h"
#include "ota.h"
#include "leds.h"
#include "ir.h"

#define MAX_WIFI_INACTIVITY 300
#define WIFI_CONNECTION_TIMEOUT 10
#define ACCESS_POINT_TIMEOUT 300000

namespace connection
{
    bool ntpInitialized = false;
    bool isAccessPoint = false;
    bool isAccessPointCreated = false;
    bool needsRestart = false;

    const char *ntpServerName = "diy.viktak.com";

    os_timer_t accessPointTimer;

    void accessPointTimerCallback(void *pArg)
    {
        ESP.reset();
    }

    enum CONNECTION_STATE
    {
        STATE_CHECK_WIFI_CONNECTION,
        STATE_WIFI_CONNECT,
        STATE_CHECK_INTERNET_CONNECTION,
        STATE_INTERNET_CONNECTED
    } connectionState;

    boolean checkInternetConnection()
    {
        IPAddress timeServerIP;
        int result = WiFi.hostByName(ntpServerName, timeServerIP);
        return (result == 1);
    }

    void setup()
    {
    }

    void loop()
    {
        if (isAccessPoint)
        {
            if (!isAccessPointCreated)
            {
                Serial.printf("Could not connect to %s.\r\nReverting to Access Point mode.\r\n", settings::wifiSSID);

                delay(500);
                WiFi.mode(WIFI_AP);
                WiFi.softAP(settings::localHost, settings::accessPointPassword);

                IPAddress myIP = WiFi.softAPIP();
                isAccessPointCreated = true;

                network::InitWifiWebServer();

                Serial.println("Access point created. Use the following information to connect to the ESP device, then follow the on-screen instructions to connect to a different wifi network:");

                Serial.print("SSID:\t\t\t");
                Serial.println(settings::localHost);

                Serial.print("Password:\t\t");
                Serial.println(settings::accessPointPassword);

                Serial.print("Access point address:\t");
                Serial.println(myIP);

                //  Timers/interrupts
                os_timer_setfn(&accessPointTimer, accessPointTimerCallback, NULL);
                os_timer_arm(&accessPointTimer, ACCESS_POINT_TIMEOUT, true);
                os_timer_disarm(&mqtt::heartbeatTimer);
            }
        }
        else
        {
            switch (connectionState)
            {

            // Check the WiFi connection
            case STATE_CHECK_WIFI_CONNECTION:
            {
                // Are we connected ?
                if (WiFi.status() != WL_CONNECTED)
                {
                    // Wifi is NOT connected
                    leds::connectionLED_OFF();
                    connectionState = STATE_WIFI_CONNECT;
                }
                else
                {
                    // Wifi is connected so check Internet
                    leds::connectionLED_ON();
                    connectionState = STATE_CHECK_INTERNET_CONNECTION;
                }
                break;
            }

            // No Wifi so attempt WiFi connection
            case STATE_WIFI_CONNECT:
            {
                // Indicate NTP no yet initialized
                ntpInitialized = false;
                leds::connectionLED_OFF();
                Serial.printf("Trying to connect to WIFI network: %s", settings::wifiSSID);

                // Set station mode
                WiFi.mode(WIFI_STA);

                // Start connection process
                WiFi.hostname(settings::localHost); //  so that it shows up coorectly in DHCP/DNS on the router
                WiFi.begin(settings::wifiSSID, settings::wifiPassword);

                // Initialize iteration counter
                uint8_t attempt = 0;

                while ((WiFi.status() != WL_CONNECTED) && (attempt++ < WIFI_CONNECTION_TIMEOUT))
                {
                    leds::connectionLED_ON();
                    Serial.print(".");
                    delay(50);
                    leds::connectionLED_OFF();
                    delay(950);
                }
                if (attempt >= WIFI_CONNECTION_TIMEOUT)
                {
                    Serial.println();
                    Serial.println("Could not connect to WiFi.");
                    delay(100);

                    isAccessPoint = true;

                    break;
                }
                leds::connectionLED_ON();

                Serial.println(" Success!");

                IPAddress ip = WiFi.localIP();

                Serial.printf("WiFi channel:\t%u\r\n", WiFi.channel());
                Serial.printf("IP address:\t%s\r\n", ip.toString().c_str());
                connectionState = STATE_CHECK_INTERNET_CONNECTION;
                break;
            }

            case STATE_CHECK_INTERNET_CONNECTION:
            {
                // Do we have a connection to the Internet ?
                if (checkInternetConnection())
                {
                    // We have an Internet connection
                    if (!ntpInitialized)
                    {
                        // We are connected to the Internet for the first time so set NTP provider
                        ntp::setup();

                        ntpInitialized = true;

                        Serial.println("Connected to the Internet.");
                    }

                    connectionState = STATE_INTERNET_CONNECTED;
                    leds::connectionLED_OFF();
                }
                else
                {
                    connectionState = STATE_CHECK_WIFI_CONNECTION;
                    leds::connectionLED_ON();
                }
                break;
            }

            case STATE_INTERNET_CONNECTED:
            {
                ota::loop();
                mqtt::loop();
                ir::loop();
                ntp::loop();
                // Set next connection state
                connectionState = STATE_CHECK_WIFI_CONNECTION;
                break;
            }
            }
        }
        network::loop();
    }

}
