////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  Wifi-IR Gateway by Viktor Takacs is licensed under CC BY-NC-SA 4.0.
//  To view a copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0 
//
////////////////////////////////////////////////////////////////////////////////////////////////////

struct config{
  char ssid[32];
  char password[32];

  char friendlyName[30];
  uint heartbeatInterval;

  signed char timeZone;

  char mqttServer[64];
  int mqttPort;
  char mqttTopic[32];

  bool dst;

};
