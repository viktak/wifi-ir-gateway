#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <TimeLib.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#include "version.h"
#include "settings.h"
#include "common.h"
#include "TimeChangeRules.h"
#include "mqtt.h"

#define ADMIN_USERNAME "admin"
#define UPDATE_SIZE_UNKNOWN 0xFFFFFFFF

namespace network
{

    WiFiClient client;
    ESP8266WebServer webServer(80);

    bool isAccessPoint = false;

    char accessPointSSID[32];

    os_timer_t accessPointTimer;

    TimeChangeRule *tcr;

    bool is_authenticated()
    {
#ifdef __debugSettings
        return true;
#endif
        if (webServer.hasHeader("Cookie"))
        {
            String cookie = webServer.header("Cookie");
            if (cookie.indexOf("EspAuth=1") != -1)
            {
                return true;
            }
        }
        return false;
    }

    void handleLogin()
    {
        String msg = "";
        if (webServer.hasHeader("Cookie"))
        {
            String cookie = webServer.header("Cookie");
        }
        if (webServer.hasArg("DISCONNECT"))
        {
            String header = "HTTP/1.1 301 OK\r\nSet-Cookie: EspAuth=0\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
            webServer.sendContent(header);
            return;
        }
        if (webServer.hasArg("username") && webServer.hasArg("password"))
        {
            if (webServer.arg("username") == ADMIN_USERNAME && webServer.arg("password") == settings::adminPassword)
            {
                String header = "HTTP/1.1 301 OK\r\nSet-Cookie: EspAuth=1\r\nLocation: /status.html\r\nCache-Control: no-cache\r\n\r\n";
                webServer.sendContent(header);
                return;
            }
            msg = "<div class=\"alert alert-danger\"><strong>Error!</strong> Wrong user name and/or password specified.<a href=\"#\" class=\"close\" data-dismiss=\"alert\" aria-label=\"close\">&times;</a></div>";
        }

        time_t localTime = timechangerules::timezones[settings::timeZone]->toLocal(now(), &tcr);

        File f = LittleFS.open("/login.html", "r");

        String htmlString;

        if (f.available())
        {
            htmlString = f.readString();
        }
        f.close();

        htmlString.replace("%year%", (String)year(localTime));
        htmlString.replace("%alert%", msg);

        webServer.send(200, "text/html", htmlString);
    }

    void handleStatus()
    {

        if (!is_authenticated())
        {
            String header = "HTTP/1.1 301 OK\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
            webServer.sendContent(header);
            return;
        }

        time_t localTime = timechangerules::timezones[settings::timeZone]->toLocal(now(), &tcr);

        File f = LittleFS.open("/status.html", "r");

        String htmlString;

        if (f.available())
        {
            htmlString = f.readString();
        }
        f.close();

        //  System information
        htmlString.replace("%year%", (String)year(localTime));
        htmlString.replace("%espid%", (String)ESP.getChipId());
        htmlString.replace("%hardwareid%", common::HARDWARE_ID);
        htmlString.replace("%hardwareversion%", common::HARDWARE_VERSION);
        htmlString.replace("%firmwareid%", common::FIRMWARE_ID);
        htmlString.replace("%firmwareversion%", String(FIRMWARE_VERSION));
        htmlString.replace("%chipid%", String((String)ESP.getChipId()));
        htmlString.replace("%uptime%", common::TimeIntervalToString(millis() / 1000));
        htmlString.replace("%currenttime%", common::DateTimeToString(localTime));
        htmlString.replace("%lastresetreason%", ESP.getResetReason());
        htmlString.replace("%flashchipsize%", String(ESP.getFlashChipSize()));
        htmlString.replace("%flashchipspeed%", String(ESP.getFlashChipSpeed()));
        htmlString.replace("%freeheapsize%", String(ESP.getFreeHeap()));
        htmlString.replace("%freesketchspace%", String(ESP.getFreeSketchSpace()));
        htmlString.replace("%friendlyname%", settings::nodeFriendlyName);
        htmlString.replace("%mqtt-topic%", settings::mqttTopic);

        //  Network settings
        switch (WiFi.getMode())
        {
        case WIFI_AP:
            htmlString.replace("%wifimode%", "Access Point");
            htmlString.replace("%macaddress%", String(WiFi.softAPmacAddress()));
            htmlString.replace("%networkaddress%", WiFi.softAPIP().toString());
            htmlString.replace("%ssid%", String(WiFi.SSID()));
            htmlString.replace("%subnetmask%", "n/a");
            htmlString.replace("%gateway%", "n/a");
            break;
        case WIFI_STA:
            htmlString.replace("%wifimode%", "Station");
            htmlString.replace("%macaddress%", WiFi.macAddress());
            htmlString.replace("%networkaddress%", WiFi.localIP().toString());
            htmlString.replace("%ssid%", String(WiFi.SSID()));
            htmlString.replace("%channel%", String(WiFi.channel()));
            htmlString.replace("%subnetmask%", WiFi.subnetMask().toString());
            htmlString.replace("%gateway%", WiFi.gatewayIP().toString());
            break;
        default:
            //  This should not happen...
            break;
        }

        webServer.send(200, "text/html", htmlString);
    }

    void handleGeneralSettings()
    {

        if (!is_authenticated())
        {
            String header = "HTTP/1.1 301 OK\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
            webServer.sendContent(header);
            return;
        }

        if (webServer.method() == HTTP_POST)
        { //  POST
#ifdef __debugSettings
            Serial.println("================= Submitted data =================");
            for (int i = 0; i < webServer.args(); i++)
                Serial.printf("%s: %s\r\n", webServer.argName(i).c_str(), webServer.arg(i).c_str());
            Serial.println("==================================================");
#endif
            //  System settings
            if (webServer.hasArg("friendlyname"))
                strcpy(settings::nodeFriendlyName, webServer.arg("friendlyname").c_str());

            if (webServer.hasArg("heartbeatinterval"))
            {
                os_timer_disarm(&mqtt::heartbeatTimer);
                settings::heartbeatInterval = atoi(webServer.arg("heartbeatinterval").c_str());
                os_timer_arm(&mqtt::heartbeatTimer, settings::heartbeatInterval * 1000, true);
            }

            if (webServer.hasArg("timezoneselector"))
            {
                settings::timeZone = atoi(webServer.arg("timezoneselector").c_str());
            }

            //  MQTT settings
            if (webServer.hasArg("mqttbroker"))
            {
                sprintf(settings::mqttServer, "%s", webServer.arg("mqttbroker").c_str());
            }

            if (webServer.hasArg("mqttport"))
            {
                settings::mqttPort = atoi(webServer.arg("mqttport").c_str());
            }

            if (webServer.hasArg("mqtttopic"))
            {
                if (webServer.arg("mqtttopic") == "")
                {
                    sprintf(settings::mqttTopic, "%s-%s", DEFAULT_MQTT_TOPIC, common::GetDeviceMAC().substring(6).c_str());
                }
                else
                {
                    sprintf(settings::mqttTopic, "%s", webServer.arg("mqtttopic").c_str());
                }
            }

            settings::SaveSettings();
            ESP.restart();
        }

        File f = LittleFS.open("/generalsettings.html", "r");

        time_t localTime = timechangerules::timezones[settings::timeZone]->toLocal(now(), &tcr);

        String htmlString, timezoneslist = "";

        char ss[4];

        for (signed char i = 0; i < (signed char)(sizeof(timechangerules::tzDescriptions) / sizeof(timechangerules::tzDescriptions[0])); i++)
        {
            itoa(i, ss, DEC);
            timezoneslist += "<option ";
            if (settings::timeZone == i)
            {
                timezoneslist += "selected ";
            }
            timezoneslist += "value=\"";
            timezoneslist += ss;
            timezoneslist += "\">";

            timezoneslist += timechangerules::tzDescriptions[i];

            timezoneslist += "</option>";
            timezoneslist += "\n";
        }

        while (f.available())
        {
            htmlString = f.readString();
        }
        f.close();

        htmlString.replace("%year%", (String)year(localTime));
        htmlString.replace("%mqtt-servername%", settings::mqttServer);
        htmlString.replace("%mqtt-port%", String(settings::mqttPort));
        htmlString.replace("%mqtt-topic%", settings::mqttTopic);
        htmlString.replace("%timezoneslist%", timezoneslist);
        htmlString.replace("%friendlyname%", settings::nodeFriendlyName);
        htmlString.replace("%heartbeatinterval%", (String)settings::heartbeatInterval);

        webServer.send(200, "text/html", htmlString);
    }

    void handleNetworkSettings()
    {

        if (!is_authenticated())
        {
            String header = "HTTP/1.1 301 OK\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
            webServer.sendContent(header);
            return;
        }

        if (webServer.method() == HTTP_POST)
        { //  POST
            if (webServer.hasArg("ssid"))
            {
                strcpy(settings::wifiSSID, webServer.arg("ssid").c_str());
                strcpy(settings::wifiPassword, webServer.arg("password").c_str());
                settings::SaveSettings();
                ESP.restart();
            }
        }

        time_t localTime = timechangerules::timezones[settings::timeZone]->toLocal(now(), &tcr);

        File f = LittleFS.open("/networksettings.html", "r");
        String htmlString, wifiList;

        byte numberOfNetworks = WiFi.scanNetworks();
        for (size_t i = 0; i < numberOfNetworks; i++)
        {
            wifiList += "<div class=\"radio\"><label><input ";
            if (i == 0)
                wifiList += "id=\"ssid\" ";

            wifiList += "type=\"radio\" name=\"ssid\" value=\"" + WiFi.SSID(i) + "\">" + WiFi.SSID(i) + "</label></div>";
        }

        if (f.available())
        {
            htmlString = f.readString();
        }
        f.close();

        htmlString.replace("%year%", (String)year(localTime));
        htmlString.replace("%wifilist%", wifiList);

        webServer.send(200, "text/html", htmlString);
    }

    void handleRemote()
    {
        if (!is_authenticated())
        {
            String header = "HTTP/1.1 301 OK\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
            webServer.sendContent(header);
            return;
        }

        if (webServer.method() == HTTPMethod::HTTP_POST)
        {
            for (int i = 0; i < webServer.args(); i++)
            {
                Serial.print(webServer.argName(i));
                Serial.print(": ");
                Serial.println(webServer.arg(i));
            }
        }

        time_t localTime = timechangerules::timezones[settings::timeZone]->toLocal(now(), &tcr);

        File f = LittleFS.open("/remote.html", "r");

        Serial.println(ESP.getFreeHeap());

        String htmlString;

        Serial.println(ESP.getFreeHeap());

        if (f.available())
        {
            htmlString = f.readString();
        }
        f.close();

        Serial.println(ESP.getFreeHeap());

        htmlString.replace("%year%", (String)year(localTime));

        webServer.send(200, "text/html", htmlString);
    }

    void handleTools()
    {

        if (!is_authenticated())
        {
            String header = "HTTP/1.1 301 OK\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
            webServer.sendContent(header);
            return;
        }

        if (webServer.method() == HTTP_POST)
        { //  POST

            if (webServer.hasArg("reset"))
            {
                settings::DefaultSettings();
                ESP.restart();
            }

            if (webServer.hasArg("restart"))
            {
                ESP.restart();
            }
        }

        File f = LittleFS.open("/tools.html", "r");

        time_t localTime = timechangerules::timezones[settings::timeZone]->toLocal(now(), &tcr);

        String htmlString;

        if (f.available())
        {
            htmlString = f.readString();
        }
        f.close();

        htmlString.replace("%year%", (String)year(localTime));

        webServer.send(200, "text/html", htmlString);
    }

    void handleNotFound()
    {
        if (!is_authenticated())
        {
            String header = "HTTP/1.1 301 OK\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
            webServer.sendContent(header);
            return;
        }

        File f = LittleFS.open("/badrequest.html", "r");

        time_t localTime = timechangerules::timezones[settings::timeZone]->toLocal(now(), &tcr);

        String htmlString;

        if (f.available())
        {
            htmlString = f.readString();
        }
        f.close();

        htmlString.replace("%year%", (String)year(localTime));

        webServer.send(200, "text/html", htmlString);
    }

    void InitWifiWebServer()
    {
        //  Web server
        if (MDNS.begin(settings::localHost))
        {
            Serial.printf("MDNS responder with hostname %s started.\r\n", settings::localHost);
        }

        //  Page handles
        webServer.on("/", handleStatus);
        webServer.on("/login.html", handleLogin);
        webServer.on("/status.html", handleStatus);
        webServer.on("/generalsettings.html", handleGeneralSettings);
        webServer.on("/remote.html", handleRemote);
        webServer.on("/networksettings.html", handleNetworkSettings);
        webServer.on("/tools.html", handleTools);

        webServer.onNotFound(handleNotFound);

        /*handling uploading firmware file */
        webServer.on(
            "/update", HTTP_POST, []()
            {
        webServer.sendHeader("Connection", "close");
        webServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK"); },
            []()
            {
                HTTPUpload &upload = webServer.upload();
                if (upload.status == UPLOAD_FILE_START)
                {
                    Serial.printf("Update: %s\n", upload.filename.c_str());
                    if (!Update.begin(UPDATE_SIZE_UNKNOWN))
                    { // start with max available size
                        Update.printError(Serial);
                    }
                }
                else if (upload.status == UPLOAD_FILE_WRITE)
                {
                    /* flashing firmware to ESP*/
                    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                    {
                        Update.printError(Serial);
                    }
                }
                else if (upload.status == UPLOAD_FILE_END)
                {
                    if (Update.end(true))
                    { // true to set the size to the current progress
                        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                        ESP.restart();
                    }
                    else
                    {
                        Update.printError(Serial);
                    }
                }
                yield();
            });

        //  Start HTTP (web) server
        webServer.begin();
        Serial.println("HTTP server started.");

        //  Authenticate HTTP requests
        const char *headerkeys[] = {"User-Agent", "Cookie"};
        size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);
        webServer.collectHeaders(headerkeys, headerkeyssize);
    }

    void setup()
    {
        InitWifiWebServer();
    }

    void loop()
    {
        webServer.handleClient();
    }

}
