/*
   ESP8266 NeoPixel NTP Clock Program

    NTP.h - Network Time Protocol Functions

    Portions of this code were extracted from the
    Time library examples by Michael Margolis and
    Paul Stoffregen. Other portions from the NTPClient
    example program from the Arduino ESP8266 package.

    Concept, Design and Implementation by: Craig A. Lindley
    Version: 2.0
    Last Update: 06/10/2016
*/

#ifndef NTP_H
#define NTP_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiUdp.h>

#define LOCALPORT     2390 // Local port to listen for UDP packets
#define NTP_PACKET_SIZE 48 // NTP time stamp is in the first 48 bytes of the message

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

byte packetBuffer[NTP_PACKET_SIZE]; // Buffer to hold incoming and outgoing packets

// Don't hardwire the IP address or we won't get the benefits of the time server pool.
IPAddress timeServerIP;
#ifdef __debugSettings
const char *ntpServerName = "192.168.1.3";
#else
const char *ntpServerName = "time.nist.gov";
//const char *ntpServerName = "nl.pool.ntp.org";
#endif

// Send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {

  // Set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  // Initialize values needed to form NTP request
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // All NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); // NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
  return 0;
}

// NTP Time Provider Code
time_t getNTPTime() {

  int attempts = 40;

  // Try multiple attempts to return the NTP time
  while (attempts--) {

    // Get a server from the pool
    WiFi.hostByName(ntpServerName, timeServerIP);
    Serial.print("Trying time server: ");
    Serial.println(timeServerIP);

    while (udp.parsePacket() > 0); // Discard any previously received packets

    Serial.println("Transmitted NTP Request.");
    sendNTPpacket(timeServerIP);

    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
      int size = udp.parsePacket();
      if (size >= NTP_PACKET_SIZE) {
        Serial.println("Received NTP Response.");
        udp.read(packetBuffer, NTP_PACKET_SIZE);  // Read packet into the buffer
        unsigned long secsSince1900;

        // Convert four bytes starting at location 40 to a long integer
        secsSince1900 =  (unsigned long) packetBuffer[40] << 24;
        secsSince1900 |= (unsigned long) packetBuffer[41] << 16;
        secsSince1900 |= (unsigned long) packetBuffer[42] << 8;
        secsSince1900 |= (unsigned long) packetBuffer[43];

        Serial.println("Got the time.");

        return secsSince1900 - 2208988800UL;
      }
      delay(10);
    }
    Serial.println("Retrying NTP request...");
    delay(4000);
  }
  Serial.println("No NTP response.");
  return 0;
}

// Assign NTP as the time sync provider
void initNTP() {

  // Login suceeded so set UDP local port
  udp.begin(LOCALPORT);

  // Set the time provider to NTP
  setSyncProvider(getNTPTime);
  setSyncInterval(NTP_REFRESH_INTERVAL);
}

// Check for Internet connectivity
boolean checkInternetConnection() {
  int result = WiFi.hostByName(ntpServerName, timeServerIP);
  return (result == 1);
}

#endif
