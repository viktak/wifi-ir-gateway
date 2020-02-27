#define __debugSettings
#include "includes.h"

IRrecv irrecv(IR_RECEIVE_GPIO, kCaptureBufferSize, kTimeout, false);
IRsend irsend(IR_SEND_GPIO);

decode_results results;

// Current connection state
enum CONNECTION_STATE connectionState;

char defaultSSID[16];

//  Web server
ESP8266WebServer server(80);

//  Initialize Wifi
WiFiClient wclient;
PubSubClient PSclient(wclient);

//  Timers and their flags
os_timer_t heartbeatTimer;

bool needsHeartbeat = false;

//  Other global variables
config appConfig;
char timeout = 30;
bool isAccessPoint = false;
bool isAccessPointCreated = false;

bool ntpInitialized = false;


// Daylight savings time rules for Greece
TimeChangeRule myDST = {"MDT", Fourth, Sun, Mar, 2, DST_TIMEZONE_OFFSET * 60};
TimeChangeRule mySTD = {"MST", Fourth,  Sun, Oct, 2,  ST_TIMEZONE_OFFSET * 60};
Timezone myTZ(myDST, mySTD);

void LogEvent(int Category, int ID, String Title, String Data){
  if (PSclient.connected()){

    String msg = "{";

    msg += "\"Node\":" + (String)ESP.getChipId() + ",";
    msg += "\"Category\":" + (String)Category + ",";
    msg += "\"ID\":" + (String)ID + ",";
    msg += "\"Title\":\"" + Title + "\",";
    msg += "\"Data\":\"" + Data + "\"}";

    Serial.println(msg);

      PSclient.publish(MQTT::Publish(MQTT_CUSTOMER + String("/") + MQTT_PROJECT + String("/") + String(ESP.getChipId()) + "/log/", msg ).set_qos(0));
  }
}

void heartbeatTimerCallback(void *pArg) {
  needsHeartbeat = true;
}

bool loadSettings(config& data) {
  fs::File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    LogEvent(EVENTCATEGORIES::System, 1, "FS failure", "Failed to open config file.");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    LogEvent(EVENTCATEGORIES::System, 2, "FS failure", "Config file size is too large.");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);
  configFile.close();

  StaticJsonDocument<JSON_SETTINGS_SIZE> doc;
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    Serial.println("Failed to parse config file");
    LogEvent(EVENTCATEGORIES::System, 3, "FS failure", "Failed to parse config file.");
    Serial.println(error.c_str());
    return false;
  }

  #ifdef __debugSettings
  serializeJsonPretty(doc,Serial);
  Serial.println();
  #endif

  if (doc["ssid"]){
    strcpy(appConfig.ssid, doc["ssid"]);
  }
  else
  {
    strcpy(appConfig.ssid, "ssid");
  }
  
  if (doc["password"]){
    strcpy(appConfig.password, doc["password"]);
  }
  else
  {
    strcpy(appConfig.password, DEFAULT_PASSWORD);
  }
  
  if (doc["mqttServer"]){
    strcpy(appConfig.mqttServer, doc["mqttServer"]);
  }
  else
  {
    strcpy(appConfig.mqttServer, DEFAULT_MQTT_SERVER);
  }
  
  if (doc["mqttPort"]){
    appConfig.mqttPort = doc["mqttPort"];
  }
  else
  {
    appConfig.mqttPort = DEFAULT_MQTT_PORT;
  }
  
  if (doc["mqttTopic"]){
    strcpy(appConfig.mqttTopic, doc["mqttTopic"]);
  }
  else
  {
    strcpy(appConfig.mqttTopic, DEFAULT_MQTT_TOPIC);
  }
  
  if (doc["friendlyName"]){
    strcpy(appConfig.friendlyName, doc["friendlyName"]);
  }
  else
  {
    strcpy(appConfig.friendlyName, NODE_DEFAULT_FRIENDLY_NAME);
  }
  
  if (doc["timezone"]){
    appConfig.timeZone = doc["timezone"];
  }
  else
  {
    appConfig.timeZone = 0;
  }
  
  if (doc["heartbeatInterval"]){
    appConfig.heartbeatInterval = doc["heartbeatInterval"];
  }
  else
  {
    appConfig.heartbeatInterval = DEFAULT_HEARTBEAT_INTERVAL;
  }
  
  return true;
}

bool saveSettings() {
  StaticJsonDocument<1024> doc;

  doc["ssid"] = appConfig.ssid;
  doc["password"] = appConfig.password;

  doc["heartbeatInterval"] = appConfig.heartbeatInterval;

  doc["timezone"] = appConfig.timeZone;

  doc["mqttServer"] = appConfig.mqttServer;
  doc["mqttPort"] = appConfig.mqttPort;
  doc["mqttTopic"] = appConfig.mqttTopic;

  doc["friendlyName"] = appConfig.friendlyName;
  #ifdef __debugSettings
  serializeJsonPretty(doc,Serial);
  Serial.println();
  #endif

  fs::File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    LogEvent(System, 4, "FS failure", "Failed to open config file for writing.");
    return false;
  }
  serializeJson(doc, configFile);
  configFile.close();

  return true;
}

void defaultSettings(){
  #ifdef __debugSettings
  strcpy(appConfig.ssid, DEBUG_WIFI_SSID);
  strcpy(appConfig.password, DEBUG_WIFI_PASSWORD);
  strcpy(appConfig.mqttServer, DEBUG_MQTT_SERVER);
  #else
  strcpy(appConfig.ssid, "ESP");
  strcpy(appConfig.password, "password");
  strcpy(appConfig.mqttServer, "test.mosquitto.org");
  #endif

  appConfig.mqttPort = 1883;
  strcpy(appConfig.mqttTopic, DEFAULT_MQTT_TOPIC);

  appConfig.timeZone = 2;

  strcpy(appConfig.friendlyName, NODE_DEFAULT_FRIENDLY_NAME);
  appConfig.heartbeatInterval = DEFAULT_HEARTBEAT_INTERVAL;

  if (!saveSettings()) {
    Serial.println("Failed to save config");
  } else {
    Serial.println("Config saved");
  }
}

String DateTimeToString(time_t time){

  String myTime = "";
  char s[2];

  //  years
  itoa(year(time), s, DEC);
  myTime+= s;
  myTime+="-";


  //  months
  itoa(month(time), s, DEC);
  myTime+= s;
  myTime+="-";

  //  days
  itoa(day(time), s, DEC);
  myTime+= s;

  myTime+=" ";

  //  hours
  itoa(hour(time), s, DEC);
  myTime+= s;
  myTime+=":";

  //  minutes
  if(minute(time) <10)
    myTime+="0";

  itoa(minute(time), s, DEC);
  myTime+= s;
  myTime+=":";

  //  seconds
  if(second(time) <10)
    myTime+="0";

  itoa(second(time), s, DEC);
  myTime+= s;

  return myTime;
}

String TimeIntervalToString(time_t time){

  String myTime = "";
  char s[2];

  //  hours
  itoa((time/3600), s, DEC);
  myTime+= s;
  myTime+=":";

  //  minutes
  if(minute(time) <10)
    myTime+="0";

  itoa(minute(time), s, DEC);
  myTime+= s;
  myTime+=":";

  //  seconds
  if(second(time) <10)
    myTime+="0";

  itoa(second(time), s, DEC);
  myTime+= s;
  return myTime;
}

void beforeIRSend(){
  digitalWrite(ACTIVITY_LED_GPIO, LOW);  
  irrecv.disableIRIn();
}

void afterIRSend(){
  irrecv.enableIRIn();
  digitalWrite(ACTIVITY_LED_GPIO, HIGH);  
}

bool is_authenticated(){
  #ifdef __debugSettings
  return true;
  #endif
  if (server.hasHeader("Cookie")){
    String cookie = server.header("Cookie");
    if (cookie.indexOf("EspAuth=1") != -1) {
      LogEvent(EVENTCATEGORIES::Authentication, 1, "Success", "");
      return true;
    }
  }
  LogEvent(EVENTCATEGORIES::Authentication, 2, "Failure", "");
  return false;
}

void handleLogin(){
  String msg = "";
  if (server.hasHeader("Cookie")){
    String cookie = server.header("Cookie");
  }
  if (server.hasArg("DISCONNECT")){
    String header = "HTTP/1.1 301 OK\r\nSet-Cookie: EspAuth=0\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    LogEvent(EVENTCATEGORIES::Login, 1, "Logout", "");
    return;
  }
  if (server.hasArg("username") && server.hasArg("password")){
    if (server.arg("username") == ADMIN_USERNAME &&  server.arg("password") == ADMIN_PASSWORD ){
      String header = "HTTP/1.1 301 OK\r\nSet-Cookie: EspAuth=1\r\nLocation: /status.html\r\nCache-Control: no-cache\r\n\r\n";
      server.sendContent(header);
      LogEvent(EVENTCATEGORIES::Login, 2, "Success", "User name: " + server.arg("username"));
      return;
    }
    msg = "<div class=\"alert alert-danger\"><strong>Error!</strong> Wrong user name and/or password specified.<a href=\"#\" class=\"close\" data-dismiss=\"alert\" aria-label=\"close\">&times;</a></div>";
    LogEvent(EVENTCATEGORIES::Login, 2, "Failure", "User name: " + server.arg("username") + " - Password: " + server.arg("password"));
  }

  fs::File f = SPIFFS.open("/pageheader.html", "r");
  String headerString;
  if (f.available()) headerString = f.readString();
  f.close();

  f = SPIFFS.open("/login.html", "r");

  String s, htmlString;

  while (f.available()){
    s = f.readStringUntil('\n');

    if (s.indexOf("%pageheader%")>-1) s.replace("%pageheader%", headerString);
    if (s.indexOf("%alert%")>-1) s.replace("%alert%", msg);

    htmlString+=s;
  }
  f.close();
  server.send(200, "text/html", htmlString);
}

void handleRoot() {
  LogEvent(EVENTCATEGORIES::PageHandler, 1, "Page requested", "/");

  if (!is_authenticated()){
    String header = "HTTP/1.1 301 OK\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }

  fs::File f = SPIFFS.open("/pageheader.html", "r");
  String headerString;
  if (f.available()) headerString = f.readString();
  f.close();

  f = SPIFFS.open("/index.html", "r");

  String FirmwareVersionString = String(FIRMWARE_VERSION) + " @ " + String(__TIME__) + " - " + String(__DATE__);

  String s, htmlString;

  while (f.available()){
    s = f.readStringUntil('\n');

    if (s.indexOf("%pageheader%")>-1) s.replace("%pageheader%", headerString);
    if (s.indexOf("%espid%")>-1) s.replace("%espid%", (String)ESP.getChipId());
    if (s.indexOf("%hardwareid%")>-1) s.replace("%hardwareid%", HARDWARE_ID);
    if (s.indexOf("%hardwareversion%")>-1) s.replace("%hardwareversion%", HARDWARE_VERSION);
    if (s.indexOf("%softwareid%")>-1) s.replace("%softwareid%", SOFTWARE_ID);
    if (s.indexOf("%firmwareversion%")>-1) s.replace("%firmwareversion%", FirmwareVersionString);

    htmlString+=s;
  }
  f.close();
  server.send(200, "text/html", htmlString);
  LogEvent(EVENTCATEGORIES::PageHandler, 2, "Page served", "/");
}

void handleStatus() {

  LogEvent(EVENTCATEGORIES::PageHandler, 1, "Page requested", "status.html");
  if (!is_authenticated()){
     String header = "HTTP/1.1 301 OK\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
     server.sendContent(header);
     return;
  }

  fs::File f = SPIFFS.open("/pageheader.html", "r");
  String headerString;
  if (f.available()) headerString = f.readString();
  f.close();

  TimeChangeRule *tcr;        // Pointer to the time change rule
  time_t localTime = myTZ.toLocal(now(), &tcr);

  String s;

  f = SPIFFS.open("/status.html", "r");

  String htmlString, ds18b20list;

  while (f.available()){
    s = f.readStringUntil('\n');

    //  System information
    if (s.indexOf("%pageheader%")>-1) s.replace("%pageheader%", headerString);
    if (s.indexOf("%chipid%")>-1) s.replace("%chipid%", (String)ESP.getChipId());
    if (s.indexOf("%uptime%")>-1) s.replace("%uptime%", TimeIntervalToString(millis()/1000));
    if (s.indexOf("%currenttime%")>-1) s.replace("%currenttime%", DateTimeToString(localTime));
    if (s.indexOf("%lastresetreason%")>-1) s.replace("%lastresetreason%", ESP.getResetReason());
    if (s.indexOf("%flashchipsize%")>-1) s.replace("%flashchipsize%",String(ESP.getFlashChipSize()));
    if (s.indexOf("%flashchipspeed%")>-1) s.replace("%flashchipspeed%",String(ESP.getFlashChipSpeed()));
    if (s.indexOf("%freeheapsize%")>-1) s.replace("%freeheapsize%",String(ESP.getFreeHeap()));
    if (s.indexOf("%freesketchspace%")>-1) s.replace("%freesketchspace%",String(ESP.getFreeSketchSpace()));
    if (s.indexOf("%friendlyname%")>-1) s.replace("%friendlyname%",appConfig.friendlyName);

    //  Network settings
    switch (WiFi.getMode()) {
      case WIFI_AP:
        if (s.indexOf("%wifimode%")>-1) s.replace("%wifimode%", "Access Point");
        if (s.indexOf("%macaddress%")>-1) s.replace("%macaddress%",String(WiFi.softAPmacAddress()));
        if (s.indexOf("%networkaddress%")>-1) s.replace("%networkaddress%",WiFi.softAPIP().toString());
        if (s.indexOf("%ssid%")>-1) s.replace("%ssid%",String(WiFi.SSID()));
        if (s.indexOf("%subnetmask%")>-1) s.replace("%subnetmask%","n/a");
        if (s.indexOf("%gateway%")>-1) s.replace("%gateway%","n/a");
        break;
      case WIFI_STA:
        if (s.indexOf("%wifimode%")>-1) s.replace("%wifimode%", "Station");
        if (s.indexOf("%macaddress%")>-1) s.replace("%macaddress%",String(WiFi.macAddress()));
        if (s.indexOf("%networkaddress%")>-1) s.replace("%networkaddress%",WiFi.localIP().toString());
        if (s.indexOf("%ssid%")>-1) s.replace("%ssid%",String(WiFi.SSID()));
        if (s.indexOf("%subnetmask%")>-1) s.replace("%subnetmask%",WiFi.subnetMask().toString());
        if (s.indexOf("%gateway%")>-1) s.replace("%gateway%",WiFi.gatewayIP().toString());
        break;
      default:
        //  This should not happen...
        break;
    }

      htmlString+=s;
    }
    f.close();
  server.send(200, "text/html", htmlString);
  LogEvent(EVENTCATEGORIES::PageHandler, 2, "Page served", "status.html");
}

void handleGeneralSettings() {
  LogEvent(EVENTCATEGORIES::PageHandler, 1, "Page requested", "generalsettings.html");

  if (!is_authenticated()){
     String header = "HTTP/1.1 301 OK\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
     server.sendContent(header);
     return;
   }

  if (server.method() == HTTP_POST){  //  POST
    bool mqttDirty = false;

    if (server.hasArg("timezoneselector")){
      signed char oldTimeZone = appConfig.timeZone;
      appConfig.timeZone = atoi(server.arg("timezoneselector").c_str());

      adjustTime((appConfig.timeZone - oldTimeZone) * SECS_PER_HOUR);

      LogEvent(EVENTCATEGORIES::TimeZoneChange, 1, "New time zone", "UTC " + server.arg("timezoneselector"));
    }

    if (server.hasArg("friendlyname")){
      strcpy(appConfig.friendlyName, server.arg("friendlyname").c_str());
      LogEvent(EVENTCATEGORIES::FriendlyNameChange, 1, "New friendly name", appConfig.friendlyName);
    }

    if (server.hasArg("heartbeatinterval")){
      os_timer_disarm(&heartbeatTimer);
      appConfig.heartbeatInterval = server.arg("heartbeatinterval").toInt();
      LogEvent(EVENTCATEGORIES::HeartbeatIntervalChange, 1, "New Heartbeat interval", (String)appConfig.heartbeatInterval);
      os_timer_arm(&heartbeatTimer, appConfig.heartbeatInterval * 1000, true);
    }

    //  MQTT settings
    if (server.hasArg("mqttbroker")){
      if ((String)appConfig.mqttServer != server.arg("mqttbroker"))
        mqttDirty = true;
        sprintf(appConfig.mqttServer, "%s", server.arg("mqttbroker").c_str());
        LogEvent(EVENTCATEGORIES::MqttParamChange, 1, "New MQTT broker", appConfig.mqttServer);
    }

    if (server.hasArg("mqttport")){
      if (appConfig.mqttPort != atoi(server.arg("mqttport").c_str()))
        mqttDirty = true;
      appConfig.mqttPort = atoi(server.arg("mqttport").c_str());
      LogEvent(EVENTCATEGORIES::MqttParamChange, 2, "New MQTT port", server.arg("mqttport").c_str());
    }

    if (server.hasArg("mqtttopic")){
      if ((String)appConfig.mqttTopic != server.arg("mqtttopic"))
        mqttDirty = true;
        sprintf(appConfig.mqttTopic, "%s", server.arg("mqtttopic").c_str());
        LogEvent(EVENTCATEGORIES::MqttParamChange, 1, "New MQTT topic", appConfig.mqttTopic);
    }

    if (mqttDirty)
      PSclient.disconnect();

    saveSettings();
  }

  fs::File f = SPIFFS.open("/pageheader.html", "r");
  String headerString;
  if (f.available()) headerString = f.readString();
  f.close();

  f = SPIFFS.open("/generalsettings.html", "r");

  String s, htmlString, timezoneslist;

  char ss[2];

  for (signed char i = -12; i < 15; i++) {
    itoa(i, ss, DEC);
    timezoneslist+="<option ";
    if (appConfig.timeZone == i){
      timezoneslist+= "selected ";
    }
    timezoneslist+= "value=\"";
    timezoneslist+=ss;
    timezoneslist+="\">UTC ";
    if (i>0){
      timezoneslist+="+";
    }
    if (i!=0){
      timezoneslist+=ss;
      timezoneslist+=":00";
    }
    timezoneslist+="</option>";
    timezoneslist+="\n";
  }

  while (f.available()){
    s = f.readStringUntil('\n');

    if (s.indexOf("%pageheader%")>-1) s.replace("%pageheader%", headerString);
    if (s.indexOf("%mqtt-servername%")>-1) s.replace("%mqtt-servername%", appConfig.mqttServer);
    if (s.indexOf("%mqtt-port%")>-1) s.replace("%mqtt-port%", String(appConfig.mqttPort));
    if (s.indexOf("%mqtt-topic%")>-1) s.replace("%mqtt-topic%", appConfig.mqttTopic);
    if (s.indexOf("%timezoneslist%")>-1) s.replace("%timezoneslist%", timezoneslist);
    if (s.indexOf("%friendlyname%")>-1) s.replace("%friendlyname%", appConfig.friendlyName);
    if (s.indexOf("%heartbeatinterval%")>-1) s.replace("%heartbeatinterval%", (String)appConfig.heartbeatInterval);

    htmlString+=s;
  }
  f.close();
  server.send(200, "text/html", htmlString);

  LogEvent(EVENTCATEGORIES::PageHandler, 2, "Page served", "generalsettings.html");
}

void handleNetworkSettings() {
  LogEvent(EVENTCATEGORIES::PageHandler, 1, "Page requested", "networksettings.html");

  if (!is_authenticated()){
     String header = "HTTP/1.1 301 OK\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
     server.sendContent(header);
     return;
   }

  if (server.method() == HTTP_POST){  //  POST
    if (server.hasArg("ssid")){
      strcpy(appConfig.ssid, server.arg("ssid").c_str());
      strcpy(appConfig.password, server.arg("password").c_str());
      saveSettings();

      isAccessPoint=false;
      connectionState = STATE_CHECK_WIFI_CONNECTION;
      WiFi.disconnect(false);

    }
  }

  fs::File f = SPIFFS.open("/pageheader.html", "r");
  String headerString;
  if (f.available()) headerString = f.readString();
  f.close();

  f = SPIFFS.open("/networksettings.html", "r");
  String s, htmlString, wifiList;

  byte numberOfNetworks = WiFi.scanNetworks();
  for (size_t i = 0; i < numberOfNetworks; i++) {
    wifiList+="<div class=\"radio\"><label><input ";
    if (i==0) wifiList+="id=\"ssid\" ";

    wifiList+="type=\"radio\" name=\"ssid\" value=\"" + WiFi.SSID(i) + "\">" + WiFi.SSID(i) + "</label></div>";
  }

  while (f.available()){
    s = f.readStringUntil('\n');

    if (s.indexOf("%pageheader%")>-1) s.replace("%pageheader%", headerString);
    if (s.indexOf("%wifilist%")>-1) s.replace("%wifilist%", wifiList);
      htmlString+=s;
    }
    f.close();
  server.send(200, "text/html", htmlString);

  LogEvent(EVENTCATEGORIES::PageHandler, 2, "Page served", "networksettings.html");
}

void handleTools() {
  LogEvent(EVENTCATEGORIES::PageHandler, 1, "Page requested", "tools.html");

  if (!is_authenticated()){
     String header = "HTTP/1.1 301 OK\r\nLocation: /login.html\r\nCache-Control: no-cache\r\n\r\n";
     server.sendContent(header);
     return;
   }

  if (server.method() == HTTP_POST){  //  POST

    if (server.hasArg("reset")){
      LogEvent(EVENTCATEGORIES::Reboot, 1, "Reset", "");
      defaultSettings();
      ESP.reset();
    }

    if (server.hasArg("restart")){
      LogEvent(EVENTCATEGORIES::Reboot, 2, "Restart", "");
      ESP.reset();
    }
  }

  fs::File f = SPIFFS.open("/pageheader.html", "r");
  String headerString;
  if (f.available()) headerString = f.readString();
  f.close();

  f = SPIFFS.open("/tools.html", "r");

  String s, htmlString;

  while (f.available()){
    s = f.readStringUntil('\n');

    if (s.indexOf("%pageheader%")>-1) s.replace("%pageheader%", headerString);

      htmlString+=s;
    }
    f.close();
  server.send(200, "text/html", htmlString);

  LogEvent(EVENTCATEGORIES::PageHandler, 2, "Page served", "tools.html");
}

/*
    for (size_t i = 0; i < server.args(); i++) {
      Serial.print(server.argName(i));
      Serial.print(": ");
      Serial.println(server.arg(i));
    }
*/

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void SendHeartbeat(){
  if (PSclient.connected()){

    TimeChangeRule *tcr;        // Pointer to the time change rule
    time_t localTime = myTZ.toLocal(now(), &tcr);

    const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 180;
    StaticJsonDocument<capacity> doc;

    doc["Time"] = DateTimeToString(localTime);
    doc["Node"] = ESP.getChipId();
    doc["Freeheap"] = ESP.getFreeHeap();
    doc["FriendlyName"] = appConfig.friendlyName;
    doc["HeartbeatInterval"] = appConfig.heartbeatInterval;

    JsonObject wifiDetails = doc.createNestedObject("Wifi");
    wifiDetails["SSId"] = String(WiFi.SSID());
    wifiDetails["MACAddress"] = String(WiFi.macAddress());
    wifiDetails["IPAddress"] = WiFi.localIP().toString();

    #ifdef __debugSettings
    serializeJsonPretty(doc,Serial);
    Serial.println();
    #endif

    String myJsonString;

    serializeJson(doc, myJsonString);

    PSclient.publish(MQTT::Publish(MQTT_CUSTOMER + String("/") + MQTT_PROJECT + "/" + appConfig.mqttTopic + "/STATE", myJsonString ).set_qos(0));
  }

  needsHeartbeat = false;
}

void SendReceivedIRCode(String protocol, String data){

  if (PSclient.connected()){

    const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 180;
    StaticJsonDocument<capacity> doc;

    doc["Protocol"] = protocol;
    doc["Data"] = data;

    #ifdef __debugSettings
    serializeJsonPretty(doc,Serial);
    Serial.println();
    #endif

    String myJsonString;

    serializeJson(doc, myJsonString);

    PSclient.publish(MQTT::Publish(MQTT_CUSTOMER + String("/") + MQTT_PROJECT + "/" + appConfig.mqttTopic + "/RESULT", myJsonString ).set_qos(0));
  }
}

void TransmitRawIRCommand(uint16_t buf[], uint16_t size, String remoteName, String commandName, uint16_t kHz ) {
  beforeIRSend();
  irsend.sendRaw(buf, size, kHz);
  SendReceivedIRCode(remoteName, commandName);
  afterIRSend();
  LogEvent(EVENTCATEGORIES::MqttMsg, 2, remoteName, commandName);
}

/*
Sample message expected:

{dig0:1, dig1:0, dig2:1, dig3:0, pwm0:127}

or

{
  "dig0": 1,
  "dig1": 0,
  "dig2": 1,
  "dig3": 0,
  "pwm0": 127
}

Order of parameters is ignored. Whitespaces/new line characters are ignored.
*/
void mqtt_callback(const MQTT::Publish& pub) {

  #ifdef __debugSettings

  Serial.print("Topic:\t\t");
  Serial.println(pub.topic());

  Serial.print("Payload:\t");
  if (pub.payload_string()!=NULL)
    Serial.println(pub.payload_string());

  #endif

  StaticJsonDocument<JSON_MQTT_COMMAND_SIZE> doc;
  DeserializationError error = deserializeJson(doc, pub.payload_string());

  if (error) {
    Serial.println("Failed to parse incoming string.");
    Serial.println(error.c_str());
    for (size_t i = 0; i < 10; i++) {
      digitalWrite(CONNECTION_STATUS_LED_GPIO, !digitalRead(CONNECTION_STATUS_LED_GPIO));
      delay(50);
    }
    return;
  }

  #ifdef __debugSettings
  serializeJsonPretty(doc,Serial);
  Serial.println();
  #endif

  //  reset
  if (doc.containsKey("reset")){
    LogEvent(EVENTCATEGORIES::MqttMsg, 1, "Reset", "");
    defaultSettings();
    ESP.reset();
  }

  //  restart
  if (doc.containsKey("restart")){
    LogEvent(EVENTCATEGORIES::MqttMsg, 2, "Restart", "");
    ESP.reset();
  }

  if (doc.containsKey("TOYOTOMI_CAR_27PAUN")){
    String s = doc["TOYOTOMI_CAR_27PAUN"];
    s.toUpperCase();
    if (s=="ON"){
      TransmitRawIRCommand(TOYOTOMI_CAR_27PAUN_ON, sizeof(TOYOTOMI_CAR_27PAUN_ON), "TOYOTOMI_CAR_27PAUN", s, 38 );
    }else{
      TransmitRawIRCommand(TOYOTOMI_CAR_27PAUN_OFF, sizeof(TOYOTOMI_CAR_27PAUN_OFF), "TOYOTOMI_CAR_27PAUN", s, 38 );
    }
  }

  if (doc.containsKey("IR_ELECTRA_ONOFF")){
    if (doc["IR_ELECTRA_ONOFF"]==1){
      TransmitRawIRCommand(IR_ELECTRA_ONOFF, sizeof(IR_ELECTRA_ONOFF), "IR_ELECTRA_ONOFF", doc["IR_ELECTRA_ONOFF"], 38 );
    }
  } 

  //  SONY_RM_ED022
  if (doc.containsKey("SONY_RM_ED022")) {
    String s = doc["SONY_RM_ED022"];
    s.toUpperCase();
    if (s=="SONY_RM_ED022_POWER"){
      TransmitRawIRCommand(SONY_RM_ED022_POWER, sizeof(SONY_RM_ED022_POWER), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_INPUT"){
      TransmitRawIRCommand(SONY_RM_ED022_INPUT, sizeof(SONY_RM_ED022_INPUT), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_THEATRE"){
      TransmitRawIRCommand(SONY_RM_ED022_THEATRE, sizeof(SONY_RM_ED022_THEATRE), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_SKIP_BACK"){
      TransmitRawIRCommand(SONY_RM_ED022_SKIP_BACK, sizeof(SONY_RM_ED022_SKIP_BACK), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_PLAY"){
      TransmitRawIRCommand(SONY_RM_ED022_PLAY, sizeof(SONY_RM_ED022_PLAY), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_SKIP_FORWARD"){
      TransmitRawIRCommand(SONY_RM_ED022_SKIP_FORWARD, sizeof(SONY_RM_ED022_SKIP_FORWARD), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_SYNC_MENU"){
      TransmitRawIRCommand(SONY_RM_ED022_SYNC_MENU, sizeof(SONY_RM_ED022_SYNC_MENU), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_PREVIOUS_TRACK"){
      TransmitRawIRCommand(SONY_RM_ED022_PREVIOUS_TRACK, sizeof(SONY_RM_ED022_PREVIOUS_TRACK), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_STOP"){
      TransmitRawIRCommand(SONY_RM_ED022_STOP, sizeof(SONY_RM_ED022_STOP), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_NEXT_TRACK"){
      TransmitRawIRCommand(SONY_RM_ED022_NEXT_TRACK, sizeof(SONY_RM_ED022_NEXT_TRACK), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_ARROW_UP"){
      TransmitRawIRCommand(SONY_RM_ED022_ARROW_UP, sizeof(SONY_RM_ED022_ARROW_UP), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_ARROW_DOWN"){
      TransmitRawIRCommand(SONY_RM_ED022_ARROW_DOWN, sizeof(SONY_RM_ED022_ARROW_DOWN), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_ARROW_LEFT"){
      TransmitRawIRCommand(SONY_RM_ED022_ARROW_LEFT, sizeof(SONY_RM_ED022_ARROW_LEFT), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_ARROW_RIGHT"){
      TransmitRawIRCommand(SONY_RM_ED022_ARROW_RIGHT, sizeof(SONY_RM_ED022_ARROW_RIGHT), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_ENTER"){
      TransmitRawIRCommand(SONY_RM_ED022_ENTER, sizeof(SONY_RM_ED022_ENTER), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_I_MANUAL"){
      TransmitRawIRCommand(SONY_RM_ED022_I_MANUAL, sizeof(SONY_RM_ED022_I_MANUAL), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_SCENE"){
      TransmitRawIRCommand(SONY_RM_ED022_SCENE, sizeof(SONY_RM_ED022_SCENE), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_ASPECT_RATIO"){
      TransmitRawIRCommand(SONY_RM_ED022_ASPECT_RATIO, sizeof(SONY_RM_ED022_ASPECT_RATIO), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_DIGITAL_ANALOG"){
      TransmitRawIRCommand(SONY_RM_ED022_DIGITAL_ANALOG, sizeof(SONY_RM_ED022_DIGITAL_ANALOG), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_FAVOURITES"){
      TransmitRawIRCommand(SONY_RM_ED022_FAVOURITES, sizeof(SONY_RM_ED022_FAVOURITES), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_GUIDE"){
      TransmitRawIRCommand(SONY_RM_ED022_GUIDE, sizeof(SONY_RM_ED022_GUIDE), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_INFO"){
      TransmitRawIRCommand(SONY_RM_ED022_INFO, sizeof(SONY_RM_ED022_INFO), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_RETURN"){
      TransmitRawIRCommand(SONY_RM_ED022_RETURN, sizeof(SONY_RM_ED022_RETURN), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_OPTIONS"){
      TransmitRawIRCommand(SONY_RM_ED022_OPTIONS, sizeof(SONY_RM_ED022_OPTIONS), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_HOME"){
      TransmitRawIRCommand(SONY_RM_ED022_HOME, sizeof(SONY_RM_ED022_HOME), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_RED"){
      TransmitRawIRCommand(SONY_RM_ED022_RED, sizeof(SONY_RM_ED022_RED), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_GREEN"){
      TransmitRawIRCommand(SONY_RM_ED022_GREEN, sizeof(SONY_RM_ED022_GREEN), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_YELLOW"){
      TransmitRawIRCommand(SONY_RM_ED022_YELLOW, sizeof(SONY_RM_ED022_YELLOW), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_BLUE"){
      TransmitRawIRCommand(SONY_RM_ED022_BLUE, sizeof(SONY_RM_ED022_BLUE), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_NUMERIC_1"){
      TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_1, sizeof(SONY_RM_ED022_NUMERIC_1), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_NUMERIC_2"){
      TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_2, sizeof(SONY_RM_ED022_NUMERIC_2), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_NUMERIC_3"){
      TransmitRawIRCommand(SONY_RM_ED022_POWER, sizeof(SONY_RM_ED022_POWER), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_NUMERIC_4"){
      TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_4, sizeof(SONY_RM_ED022_NUMERIC_4), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_NUMERIC_5"){
      TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_5, sizeof(SONY_RM_ED022_NUMERIC_5), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_NUMERIC_6"){
      TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_6, sizeof(SONY_RM_ED022_NUMERIC_6), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_NUMERIC_7"){
      TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_7, sizeof(SONY_RM_ED022_NUMERIC_7), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_NUMERIC_8"){
      TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_8, sizeof(SONY_RM_ED022_NUMERIC_8), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_NUMERIC_9"){
      TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_9, sizeof(SONY_RM_ED022_NUMERIC_9), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_NUMERIC_0"){
      TransmitRawIRCommand(SONY_RM_ED022_NUMERIC_0, sizeof(SONY_RM_ED022_NUMERIC_0), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_TELETEXT"){
      TransmitRawIRCommand(SONY_RM_ED022_TELETEXT, sizeof(SONY_RM_ED022_TELETEXT), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_TELETEXT2"){
      TransmitRawIRCommand(SONY_RM_ED022_TELETEXT2, sizeof(SONY_RM_ED022_TELETEXT2), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_MUTE"){
      TransmitRawIRCommand(SONY_RM_ED022_MUTE, sizeof(SONY_RM_ED022_MUTE), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_VOLUME_UP"){
      TransmitRawIRCommand(SONY_RM_ED022_VOLUME_UP, sizeof(SONY_RM_ED022_VOLUME_UP), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_VOLUME_DOWN"){
      TransmitRawIRCommand(SONY_RM_ED022_VOLUME_DOWN, sizeof(SONY_RM_ED022_VOLUME_DOWN), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_PROGRAM_UP"){
      TransmitRawIRCommand(SONY_RM_ED022_PROGRAM_UP, sizeof(SONY_RM_ED022_PROGRAM_UP), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_PROGRAM_DOWN"){
      TransmitRawIRCommand(SONY_RM_ED022_PROGRAM_DOWN, sizeof(SONY_RM_ED022_PROGRAM_DOWN), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
    if (s=="SONY_RM_ED022_AUDIO"){
      TransmitRawIRCommand(SONY_RM_ED022_AUDIO, sizeof(SONY_RM_ED022_AUDIO), "SONY_RM_ED022", doc["SONY_RM_ED022"], 38 );
    }
  }
  //  SONY_RM_LP211
  if (doc.containsKey("SONY_RM_LP211")) {
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_POWER"){
      TransmitRawIRCommand(SONY_RM_LP211_POWER, sizeof(SONY_RM_LP211_POWER), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SOURCE_VIDEO_1"){
      TransmitRawIRCommand(SONY_RM_LP211_SOURCE_VIDEO_1, sizeof(SONY_RM_LP211_SOURCE_VIDEO_1), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SOURCE_VIDEO_2"){
      TransmitRawIRCommand(SONY_RM_LP211_SOURCE_VIDEO_2, sizeof(SONY_RM_LP211_SOURCE_VIDEO_2), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SOURCE_VIDEO_3"){
      TransmitRawIRCommand(SONY_RM_LP211_SOURCE_VIDEO_3, sizeof(SONY_RM_LP211_SOURCE_VIDEO_3), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SOURCE_DVD_LD"){
      TransmitRawIRCommand(SONY_RM_LP211_SOURCE_DVD_LD, sizeof(SONY_RM_LP211_SOURCE_DVD_LD), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SOURCE_TV_SAT"){
      TransmitRawIRCommand(SONY_RM_LP211_SOURCE_TV_SAT, sizeof(SONY_RM_LP211_SOURCE_TV_SAT), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SOURCE_TAPE"){
      TransmitRawIRCommand(SONY_RM_LP211_SOURCE_TAPE, sizeof(SONY_RM_LP211_SOURCE_TAPE), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SOURCE_MD_DAT"){
      TransmitRawIRCommand(SONY_RM_LP211_SOURCE_MD_DAT, sizeof(SONY_RM_LP211_SOURCE_MD_DAT), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SOURCE_CD_SACD"){
      TransmitRawIRCommand(SONY_RM_LP211_SOURCE_CD_SACD, sizeof(SONY_RM_LP211_SOURCE_CD_SACD), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SOURCE_TUNER"){
      TransmitRawIRCommand(SONY_RM_LP211_SOURCE_TUNER, sizeof(SONY_RM_LP211_SOURCE_TUNER), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SOURCE_PHONO"){
      TransmitRawIRCommand(SONY_RM_LP211_SOURCE_PHONO, sizeof(SONY_RM_LP211_SOURCE_PHONO), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SOURCE_MULTI_CHANEL"){
      TransmitRawIRCommand(SONY_RM_LP211_POWER, sizeof(SONY_RM_LP211_POWER), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_PREVIOUS_TRACK"){
      TransmitRawIRCommand(SONY_RM_LP211_PREVIOUS_TRACK, sizeof(SONY_RM_LP211_PREVIOUS_TRACK), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_NEXT_TRACK"){
      TransmitRawIRCommand(SONY_RM_LP211_NEXT_TRACK, sizeof(SONY_RM_LP211_NEXT_TRACK), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SKIP_FORWARD"){
      TransmitRawIRCommand(SONY_RM_LP211_SKIP_FORWARD, sizeof(SONY_RM_LP211_SKIP_FORWARD), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_SKIP_BACK"){
      TransmitRawIRCommand(SONY_RM_LP211_SKIP_BACK, sizeof(SONY_RM_LP211_SKIP_BACK), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_PLAY"){
      TransmitRawIRCommand(SONY_RM_LP211_PLAY, sizeof(SONY_RM_LP211_PLAY), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_PAUSE"){
      TransmitRawIRCommand(SONY_RM_LP211_POWER, sizeof(SONY_RM_LP211_POWER), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_STOP"){
      TransmitRawIRCommand(SONY_RM_LP211_STOP, sizeof(SONY_RM_LP211_STOP), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_VOLUME_UP"){
      TransmitRawIRCommand(SONY_RM_LP211_VOLUME_UP, sizeof(SONY_RM_LP211_VOLUME_UP), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_VOLUME_DOWN"){
      TransmitRawIRCommand(SONY_RM_LP211_VOLUME_DOWN, sizeof(SONY_RM_LP211_VOLUME_DOWN), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
    if (doc["SONY_RM_LP211"]=="SONY_RM_LP211_MUTE"){
      TransmitRawIRCommand(SONY_RM_LP211_MUTE, sizeof(SONY_RM_LP211_MUTE), "SONY_RM_LP211", doc["SONY_RM_LP211"], 38 );
    }
  }
}

void setup() {
  delay(1); //  Needed for PlatformIO serial monitor
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  Serial.print("\n\n\n\rBooting node:     ");
  Serial.print(ESP.getChipId());
  Serial.println("...");

  String FirmwareVersionString = String(FIRMWARE_VERSION) + " @ " + String(__TIME__) + " - " + String(__DATE__);

  Serial.println("Hardware ID:      " + (String)HARDWARE_ID);
  Serial.println("Hardware version: " + (String)HARDWARE_VERSION);
  Serial.println("Software ID:      " + (String)SOFTWARE_ID);
  Serial.println("Software version: " + FirmwareVersionString);
  Serial.println();

  sprintf(defaultSSID, "ESP-%u", ESP.getChipId());
  WiFi.hostname(defaultSSID);


  //  GPIOs

  //  outputs
  pinMode(CONNECTION_STATUS_LED_GPIO, OUTPUT);
  digitalWrite(CONNECTION_STATUS_LED_GPIO, HIGH);

  pinMode(ACTIVITY_LED_GPIO, OUTPUT);
  digitalWrite(ACTIVITY_LED_GPIO, HIGH);

  pinMode(IR_SEND_GPIO, OUTPUT);
  
  //  File system
  if (!SPIFFS.begin()){
    Serial.println("Error: Failed to initialize the filesystem!");
  }

  if (!loadSettings(appConfig)) {
    Serial.println("Failed to load config, creating default settings...");
    defaultSettings();
  } else {
    Serial.println("Config loaded.");
  }

  //  OTA
  ArduinoOTA.onStart([]() {
    Serial.println("OTA started.");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA finished.");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    if (progress % OTA_BLINKING_RATE == 0){
      if (digitalRead(CONNECTION_STATUS_LED_GPIO)==HIGH)
        digitalWrite(CONNECTION_STATUS_LED_GPIO, LOW);
        else
        digitalWrite(CONNECTION_STATUS_LED_GPIO, HIGH);
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Authentication failed.");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin failed.");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect failed.");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive failed.");
    else if (error == OTA_END_ERROR) Serial.println("End failed.");
  });

  ArduinoOTA.begin();

  Serial.println();

  server.on("/", handleRoot);
  server.on("/status.html", handleStatus);
  server.on("/generalsettings.html", handleGeneralSettings);
  server.on("/networksettings.html", handleNetworkSettings);
  server.on("/tools.html", handleTools);
  server.on("/login.html", handleLogin);

  server.onNotFound(handleNotFound);

  //  Web server
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started.");
  }

  //  Start HTTP (web) server
  server.begin();
  Serial.println("HTTP server started.");

  //  Authenticate HTTP requests
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  server.collectHeaders(headerkeys, headerkeyssize );

  //  Timers
  os_timer_setfn(&heartbeatTimer, heartbeatTimerCallback, NULL);
  os_timer_arm(&heartbeatTimer, appConfig.heartbeatInterval * 1000, true);

  //  Randomizer
  srand(now());

  //  IR
  #if DECODE_HASH
  // Ignore messages with less than minimum on or off pulses.
  irrecv.setUnknownThreshold(kMinUnknownSize);
  #endif  // DECODE_HASH
  irsend.begin();
  irrecv.enableIRIn();

  // Set the initial connection state
  connectionState = STATE_CHECK_WIFI_CONNECTION;
}

void loop(){

  if (isAccessPoint){
    if (!isAccessPointCreated){
      Serial.print(" Could not connect to ");
      Serial.print(appConfig.ssid);
      Serial.println("\r\nReverting to Access Point mode.");

      delay(500);

      WiFi.mode(WiFiMode::WIFI_AP);
      WiFi.softAP(defaultSSID, DEFAULT_PASSWORD);

      IPAddress myIP;
      myIP = WiFi.softAPIP();
      isAccessPointCreated = true;

      Serial.println("Access point created. Use the following information to connect to the ESP device, then follow the on-screen instructions to connect to a different wifi network:");

      Serial.print("SSID:\t\t\t");
      Serial.println(defaultSSID);

      Serial.print("Password:\t\t");
      Serial.println(DEFAULT_PASSWORD);

      Serial.print("Access point address:\t");
      Serial.println(myIP);
    }
    server.handleClient();
  }
  else{
    switch (connectionState) {

      // Check the WiFi connection
      case STATE_CHECK_WIFI_CONNECTION:

        // Are we connected ?
        if (WiFi.status() != WL_CONNECTED) {
          // Wifi is NOT connected
          digitalWrite(CONNECTION_STATUS_LED_GPIO, HIGH);
          connectionState = STATE_WIFI_CONNECT;
        } else  {
          // Wifi is connected so check Internet
          digitalWrite(CONNECTION_STATUS_LED_GPIO, LOW);
          connectionState = STATE_CHECK_INTERNET_CONNECTION;
          
          server.handleClient();
        }
        break;

      // No Wifi so attempt WiFi connection
      case STATE_WIFI_CONNECT:
        {
          // Indicate NTP no yet initialized
          ntpInitialized = false;

          digitalWrite(CONNECTION_STATUS_LED_GPIO, HIGH);
          Serial.printf("Trying to connect to WIFI network: %s", appConfig.ssid);

          // Set station mode
          WiFi.mode(WIFI_STA);

          // Start connection process
          WiFi.begin(appConfig.ssid, appConfig.password);

          // Initialize iteration counter
          char attempt = 0;

          while ((WiFi.status() != WL_CONNECTED) && (attempt++ < timeout)) {
            digitalWrite(CONNECTION_STATUS_LED_GPIO, LOW);
            Serial.print(".");
            delay(50);
            digitalWrite(CONNECTION_STATUS_LED_GPIO, HIGH);
            delay(950);
          }
          if (attempt == 31) {
            Serial.println();
            Serial.println("Could not connect to WiFi");
            delay(100);

            isAccessPoint=true;

            break;
          }
          digitalWrite(CONNECTION_STATUS_LED_GPIO, LOW);
          Serial.println(" Success!");
          Serial.print("IP address: ");
          Serial.println(WiFi.localIP());
          connectionState = STATE_CHECK_INTERNET_CONNECTION;
        }
        break;

      case STATE_CHECK_INTERNET_CONNECTION:

        // Do we have a connection to the Internet ?
        if (checkInternetConnection()) {
          // We have an Internet connection

          if (! ntpInitialized) {
            // We are connected to the Internet for the first time so set NTP provider
            initNTP();

            ntpInitialized = true;

            Serial.println("Connected to the Internet.");
          }

          connectionState = STATE_INTERNET_CONNECTED;
        } else  {
          connectionState = STATE_CHECK_WIFI_CONNECTION;
        }
        break;

      case STATE_INTERNET_CONNECTED:

        ArduinoOTA.handle();

        if (!PSclient.connected()) {
          PSclient.set_server(appConfig.mqttServer, appConfig.mqttPort);

          String msg = "{";
          msg += "\"Node\":" + (String)ESP.getChipId() + ",";
          msg += "\"Category\":1,";
          msg += "\"ID\":2,";
          msg += "\"Title\":\"Node offline\",";
          msg += "\"Data\":\"";
          msg += DateTimeToString(now());
          msg += "\"}";

          if (PSclient.connect("ESP-" + String(ESP.getChipId()), MQTT_CUSTOMER + String("/") + MQTT_PROJECT + String("/") + String(ESP.getChipId()) + "/log", 0, true, msg )){
            PSclient.set_callback(mqtt_callback);
            PSclient.subscribe(MQTT_CUSTOMER + String("/") + MQTT_PROJECT + String("/") + appConfig.mqttTopic + "/cmnd", 0);
            LogEvent(EVENTCATEGORIES::Conn, 1, "Node online", WiFi.localIP().toString());
          }
        }

        if (irrecv.decode(&results)) {
          // Check if we got an IR message that was to big for our capture buffer.
          if (results.overflow)
            Serial.printf(D_WARN_BUFFERFULL "\r\n", kCaptureBufferSize);
          // Display the library version the message was captured with.
          Serial.println(D_STR_LIBRARY "   : v" _IRREMOTEESP8266_VERSION_ "\r\n");
          // Display the basic output of what we found.
          Serial.println(resultToHumanReadableBasic(&results));
          // Display any extra A/C info if we have it.
          String description = IRAcUtils::resultAcToString(&results);
          if (description.length()) Serial.println(D_STR_MESGDESC ": " + description);
      #if LEGACY_TIMING_INFO
          // Output legacy RAW timing info of the result.
          Serial.println(resultToTimingInfo(&results));
      #endif  // LEGACY_TIMING_INFO

          // Output the results as source code
          Serial.println(resultToSourceCode(&results));
          Serial.println();    // Blank line between entries
          irrecv.resume(); // Receive the next value

          PSclient.publish(MQTT::Publish(MQTT_CUSTOMER + String("/") + MQTT_PROJECT + String("/") + appConfig.mqttTopic + "/RESULT", resultToSourceCode(&results) ).set_qos(0));
          LogEvent(EVENTCATEGORIES::IRreceived, results.decode_type, "IR command", resultToSourceCode(&results));
        }

        if (PSclient.connected()){
          PSclient.loop();
        }

        if (needsHeartbeat){
          SendHeartbeat();
          needsHeartbeat = false;
        }

        // Set next connection state
        connectionState = STATE_CHECK_WIFI_CONNECTION;
        break;
    }

  }
}
