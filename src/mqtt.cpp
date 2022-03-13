#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include "version.h"
#include "settings.h"
#include "network.h"
#include "common.h"
#include "logger.h"

#include "ir.h"
#include "ir-toyotomi-car-27paun-raw.h"
#include "ir-sharp.h"
#include "ir-sony-rm-ed022-raw.h"
#include "ir-sony-rm-lp211-raw.h"
#include "ir-toyotomi-yaw1f6-raw.h"
#include "ir-yamaha-rav536.h"

#define MQTT_CUSTOMER "viktak"
#define MQTT_PROJECT "spiti"

namespace mqtt
{
    bool needsHeartbeat = false;
    os_timer_t heartbeatTimer;

    PubSubClient PSclient(network::client);

    const char *mqttCustomer = MQTT_CUSTOMER;
    const char *mqttProject = MQTT_PROJECT;

    void heartbeatTimerCallback(void *pArg)
    {
        needsHeartbeat = true;
    }

    void ConnectToMQTTBroker()
    {
        if (!PSclient.connected())
        {
#ifdef __debugSettings
            Serial.printf("Connecting to MQTT broker %s... ", settings::mqttServer);
#endif
            if (PSclient.connect(settings::localHost, (mqttCustomer + String("/") + mqttProject + String("/") + settings::mqttTopic + "/STATE").c_str(), 0, true, "offline"))
            {
#ifdef __debugSettings
                Serial.println(" success.");
#endif
                PSclient.subscribe((mqttCustomer + String("/") + mqttProject + String("/") + settings::mqttTopic + "/cmnd").c_str(), 0);
                PSclient.publish((mqttCustomer + String("/") + mqttProject + String("/") + settings::mqttTopic + "/STATE").c_str(), "online", true);

                PSclient.setBufferSize(1024 * 5);
            }
            else
            {
#ifdef __debugSettings
                Serial.println(" failure!");
#endif
            }
        }
    }

    void PublishData(const char *topic, const char *payload, bool retained)
    {
        ConnectToMQTTBroker();

        if (PSclient.connected())
        {
            PSclient.publish((mqttCustomer + String("/") + mqttProject + String("/") + settings::mqttTopic + String("/") + topic).c_str(), payload, retained);
        }
    }

    void SendHeartbeat()
    {

        // todo
        StaticJsonDocument<768> doc;

        JsonObject sysDetails = doc.createNestedObject("System");
        sysDetails["ChipID"] = (String)ESP.getChipId();

        sysDetails["Time"] = common::GetFullDateTime("%F %T", size_t(20));
        sysDetails["Node"] = settings::localHost;
        sysDetails["Freeheap"] = ESP.getFreeHeap();

        sysDetails["HardwareID"] = common::HARDWARE_ID;
        sysDetails["HardwareVersion"] = common::HARDWARE_VERSION;
        sysDetails["FirmwareID"] = common::FIRMWARE_ID;
        sysDetails["FirmwareVersion"] = FIRMWARE_VERSION;
        sysDetails["UpTime"] = common::TimeIntervalToString(millis() / 1000);
        sysDetails["CPU0_ResetReason"] = ESP.getResetReason();

        sysDetails["FriendlyName"] = settings::nodeFriendlyName;
        sysDetails["TIMEZONE"] = settings::timeZone;

        JsonObject mqttDetails = doc.createNestedObject("MQTT");

        mqttDetails["MQTT_SERVER"] = settings::mqttServer;
        mqttDetails["MQTT_PORT"] = settings::mqttPort;
        mqttDetails["MQTT_TOPIC"] = settings::mqttTopic;

        JsonObject wifiDetails = doc.createNestedObject("WiFi");
        wifiDetails["APP_NAME"] = settings::localHost;
        wifiDetails["SSID"] = settings::wifiSSID;
        wifiDetails["Channel"] = WiFi.channel();
        wifiDetails["IP_Address"] = WiFi.localIP().toString();
        wifiDetails["MAC_Address"] = WiFi.macAddress();

        String myJsonString;

        serializeJson(doc, myJsonString);

#ifdef __debugSettings
        serializeJsonPretty(doc, Serial);
        Serial.println();
#endif

        ConnectToMQTTBroker();

        if (PSclient.connected())
        {
            PSclient.publish((mqttCustomer + String("/") + mqttProject + String("/") + settings::mqttTopic + "/HEARTBEAT").c_str(), myJsonString.c_str(), false);
#ifdef __debugSettings
            Serial.println("Heartbeat sent.");
#endif
            mqtt::needsHeartbeat = false;
        }
    }

    void mqttCallback(char *topic, byte *payload, unsigned int len)
    {
        const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(14) + 300;

        DynamicJsonDocument doc(capacity);
        deserializeJson(doc, payload);

#ifdef __debugSettings
        Serial.print("Message arrived in topic [");
        Serial.print(topic);
        Serial.println("]: ");

        serializeJsonPretty(doc, Serial);
        Serial.println();
#endif

        if (doc.containsKey("TOYOTOMI_CAR_27PAUN"))
        {
            String s = doc["TOYOTOMI_CAR_27PAUN"];
            s.toUpperCase();
            if (s == "ON")
            {
                ir::TransmitRawIRCommand(TOYOTOMI_CAR_27PAUN_ON, sizeof(TOYOTOMI_CAR_27PAUN_ON), "TOYOTOMI_CAR_27PAUN", s, 38);
            }
            else if (s == "ON_COOL")
            {
                ir::TransmitRawIRCommand(TOYOTOMI_CAR_27PAUN_ON_COOLING, sizeof(TOYOTOMI_CAR_27PAUN_ON_COOLING), "TOYOTOMI_CAR_27PAUN", s, 38);
            }
            else
            {
                ir::TransmitRawIRCommand(TOYOTOMI_CAR_27PAUN_OFF, sizeof(TOYOTOMI_CAR_27PAUN_OFF), "TOYOTOMI_CAR_27PAUN", s, 38);
            }
        }

        //  SONY_RM_ED022
        if (doc.containsKey("SONY_RM_ED022"))
        {
            String s = doc["SONY_RM_ED022"];
            s.toUpperCase();
            if (s == "SONY_RM_ED022_POWER")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_POWER, sizeof(SONY_RM_ED022_POWER), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_INPUT")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_INPUT, sizeof(SONY_RM_ED022_INPUT), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_THEATRE")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_THEATRE, sizeof(SONY_RM_ED022_THEATRE), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_SKIP_BACK")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_SKIP_BACK, sizeof(SONY_RM_ED022_SKIP_BACK), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_PLAY")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_PLAY, sizeof(SONY_RM_ED022_PLAY), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_SKIP_FORWARD")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_SKIP_FORWARD, sizeof(SONY_RM_ED022_SKIP_FORWARD), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_SYNC_MENU")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_SYNC_MENU, sizeof(SONY_RM_ED022_SYNC_MENU), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_PREVIOUS_TRACK")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_PREVIOUS_TRACK, sizeof(SONY_RM_ED022_PREVIOUS_TRACK), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_STOP")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_STOP, sizeof(SONY_RM_ED022_STOP), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_NEXT_TRACK")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_NEXT_TRACK, sizeof(SONY_RM_ED022_NEXT_TRACK), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_ARROW_UP")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_ARROW_UP, sizeof(SONY_RM_ED022_ARROW_UP), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_ARROW_DOWN")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_ARROW_DOWN, sizeof(SONY_RM_ED022_ARROW_DOWN), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_ARROW_LEFT")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_ARROW_LEFT, sizeof(SONY_RM_ED022_ARROW_LEFT), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_ARROW_RIGHT")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_ARROW_RIGHT, sizeof(SONY_RM_ED022_ARROW_RIGHT), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_ENTER")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_ENTER, sizeof(SONY_RM_ED022_ENTER), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_I_MANUAL")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_I_MANUAL, sizeof(SONY_RM_ED022_I_MANUAL), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_SCENE")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_SCENE, sizeof(SONY_RM_ED022_SCENE), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_ASPECT_RATIO")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_ASPECT_RATIO, sizeof(SONY_RM_ED022_ASPECT_RATIO), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_DIGITAL_ANALOG")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_DIGITAL_ANALOG, sizeof(SONY_RM_ED022_DIGITAL_ANALOG), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_FAVOURITES")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_FAVOURITES, sizeof(SONY_RM_ED022_FAVOURITES), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_GUIDE")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_GUIDE, sizeof(SONY_RM_ED022_GUIDE), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_INFO")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_INFO, sizeof(SONY_RM_ED022_INFO), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_RETURN")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_RETURN, sizeof(SONY_RM_ED022_RETURN), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_OPTIONS")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_OPTIONS, sizeof(SONY_RM_ED022_OPTIONS), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_HOME")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_HOME, sizeof(SONY_RM_ED022_HOME), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_RED")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_RED, sizeof(SONY_RM_ED022_RED), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_GREEN")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_GREEN, sizeof(SONY_RM_ED022_GREEN), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_YELLOW")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_YELLOW, sizeof(SONY_RM_ED022_YELLOW), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_BLUE")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_BLUE, sizeof(SONY_RM_ED022_BLUE), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_NUMERIC_1")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_1, sizeof(SONY_RM_ED022_NUMERIC_1), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_NUMERIC_2")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_2, sizeof(SONY_RM_ED022_NUMERIC_2), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_NUMERIC_3")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_3, sizeof(SONY_RM_ED022_NUMERIC_3), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_NUMERIC_4")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_4, sizeof(SONY_RM_ED022_NUMERIC_4), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_NUMERIC_5")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_5, sizeof(SONY_RM_ED022_NUMERIC_5), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_NUMERIC_6")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_6, sizeof(SONY_RM_ED022_NUMERIC_6), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_NUMERIC_7")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_7, sizeof(SONY_RM_ED022_NUMERIC_7), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_NUMERIC_8")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_8, sizeof(SONY_RM_ED022_NUMERIC_8), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_NUMERIC_9")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_9, sizeof(SONY_RM_ED022_NUMERIC_9), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_NUMERIC_0")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_0, sizeof(SONY_RM_ED022_NUMERIC_0), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_TELETEXT")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_TELETEXT, sizeof(SONY_RM_ED022_TELETEXT), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_TELETEXT2")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_TELETEXT2, sizeof(SONY_RM_ED022_TELETEXT2), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_MUTE")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_MUTE, sizeof(SONY_RM_ED022_MUTE), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_VOLUME_UP")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_VOLUME_UP, sizeof(SONY_RM_ED022_VOLUME_UP), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_VOLUME_DOWN")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_VOLUME_DOWN, sizeof(SONY_RM_ED022_VOLUME_DOWN), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_PROGRAM_UP")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_PROGRAM_UP, sizeof(SONY_RM_ED022_PROGRAM_UP), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_PROGRAM_DOWN")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_PROGRAM_DOWN, sizeof(SONY_RM_ED022_PROGRAM_DOWN), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
            if (s == "SONY_RM_ED022_AUDIO")
            {
                ir::TransmitRawIRCommand(SONY_RM_ED022_AUDIO, sizeof(SONY_RM_ED022_AUDIO), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38);
            }
        }
        // //  SONY_RM_LP211
        if (doc.containsKey("SONY_RM_LP211"))
        {
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_POWER")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_POWER, sizeof(SONY_RM_LP211_POWER), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SOURCE_VIDEO_1")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SOURCE_VIDEO_1, sizeof(SONY_RM_LP211_SOURCE_VIDEO_1), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SOURCE_VIDEO_2")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SOURCE_VIDEO_2, sizeof(SONY_RM_LP211_SOURCE_VIDEO_2), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SOURCE_VIDEO_3")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SOURCE_VIDEO_3, sizeof(SONY_RM_LP211_SOURCE_VIDEO_3), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SOURCE_DVD_LD")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SOURCE_DVD_LD, sizeof(SONY_RM_LP211_SOURCE_DVD_LD), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SOURCE_TV_SAT")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SOURCE_TV_SAT, sizeof(SONY_RM_LP211_SOURCE_TV_SAT), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SOURCE_TAPE")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SOURCE_TAPE, sizeof(SONY_RM_LP211_SOURCE_TAPE), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SOURCE_MD_DAT")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SOURCE_MD_DAT, sizeof(SONY_RM_LP211_SOURCE_MD_DAT), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SOURCE_CD_SACD")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SOURCE_CD_SACD, sizeof(SONY_RM_LP211_SOURCE_CD_SACD), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SOURCE_TUNER")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SOURCE_TUNER, sizeof(SONY_RM_LP211_SOURCE_TUNER), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SOURCE_PHONO")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SOURCE_PHONO, sizeof(SONY_RM_LP211_SOURCE_PHONO), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SOURCE_MULTI_CHANEL")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SOURCE_MULTI_CHANEL, sizeof(SONY_RM_LP211_SOURCE_MULTI_CHANEL), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_PREVIOUS_TRACK")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_PREVIOUS_TRACK, sizeof(SONY_RM_LP211_PREVIOUS_TRACK), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_NEXT_TRACK")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_NEXT_TRACK, sizeof(SONY_RM_LP211_NEXT_TRACK), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SKIP_FORWARD")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SKIP_FORWARD, sizeof(SONY_RM_LP211_SKIP_FORWARD), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_SKIP_BACK")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_SKIP_BACK, sizeof(SONY_RM_LP211_SKIP_BACK), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_PLAY")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_PLAY, sizeof(SONY_RM_LP211_PLAY), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_PAUSE")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_PAUSE, sizeof(SONY_RM_LP211_PAUSE), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_STOP")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_STOP, sizeof(SONY_RM_LP211_STOP), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_VOLUME_UP")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_VOLUME_UP, sizeof(SONY_RM_LP211_VOLUME_UP), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_VOLUME_DOWN")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_VOLUME_DOWN, sizeof(SONY_RM_LP211_VOLUME_DOWN), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
            if (doc["SONY_RM_LP211"] == "SONY_RM_LP211_MUTE")
            {
                ir::TransmitRawIRCommand(SONY_RM_LP211_MUTE, sizeof(SONY_RM_LP211_MUTE), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38);
            }
        }
    }

    void setup()
    {
        PSclient.setServer(settings::mqttServer, settings::mqttPort);
        PSclient.setCallback(mqttCallback);

        os_timer_setfn(&heartbeatTimer, heartbeatTimerCallback, NULL);
        os_timer_arm(&heartbeatTimer, settings::heartbeatInterval * 1000, true);
    }

    void loop()
    {
        ConnectToMQTTBroker();
        if (PSclient.connected())
            PSclient.loop();

        if (needsHeartbeat)
            SendHeartbeat();
    }
}
