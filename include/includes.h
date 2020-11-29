////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  Wifi-IR Gateway by Viktor Takacs is licensed under CC BY-NC-SA 4.0.
//  To view a copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0 
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDES_H
#define INCLUDES_H

#include "version.h"
#include "defines.h"
#include "../../_common/defines.h"

#include "enums.h"
#include "../../_common/enums.h"

#include "../../_common/variables.cpp"

#include <cstdlib>
#include <string>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

#include <LittleFS.h>

#include <PubSubClient.h>
#include <EEPROM.h>

#include <IRrecv.h>
#include <IRutils.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRsend.h>
#include <IRremoteESP8266.h>

#include <ArduinoJson.h>
#include <TimeLib.h>
#include <Time.h>
#include <Timezone.h>
#include "NTP.h"

#include "structs.h"

#include "user_interface.h"

#include "ircodes.h"
#include "ir-sony-rm-ed022-raw.h"
#include "ir-sony-rm-lp211-raw.h"
#include "ir-toyotomi-car-27paun-raw.h"

#endif