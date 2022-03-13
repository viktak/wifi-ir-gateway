#include <Arduino.h>

#include "settings.h"
#include "mqtt.h"

namespace logger
{
    void LogEvent(int Category, int ID, String Title, String Data)
    {
        if (mqtt::PSclient.connected())
        {
            String msg = "{";

            msg += "\"Node\":" + (String)ESP.getChipId() + ",";
            msg += "\"Category\":" + (String)Category + ",";
            msg += "\"ID\":" + (String)ID + ",";
            msg += "\"Title\":\"" + Title + "\",";
            msg += "\"Data\":\"" + Data + "\"}";

            mqtt::PSclient.publish((mqtt::mqttCustomer + String("/") + mqtt::mqttProject + String("/") + settings::mqttTopic + "/log").c_str(), msg.c_str(), false);
        }   
    }

}