#include <Arduino.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoOTA.h>

#include "network.h"
#include "leds.h"

#define OTA_BLINKING_RATE 3

namespace ota
{

    String host = "diy.viktak.com";
    String bin = "/firmware.bin"; // bin file name with a slash in front.
    const int port = 80;
    int contentLength = 0;
    bool isValidContentType = false;

    String getHeaderValue(const String header, const String headerName)
    {
        return header.substring(strlen(headerName.c_str()));
    }

    void initOTA()
    {
        ArduinoOTA.onStart([]()
                           { Serial.println("OTA started.\r\n"); });

        ArduinoOTA.onEnd([]()
                         { Serial.println("\r\nOTA finished.\r\n"); });

        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                              {
                            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
                            if (progress % OTA_BLINKING_RATE == 0) leds::connectionLED_TOGGLE(); });

        ArduinoOTA.onError([](ota_error_t error)
                           {
                            Serial.printf("Error[%u]: ", error);
                            if (error == OTA_AUTH_ERROR) Serial.println("Authentication failed.");
                            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin failed.");
                            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect failed.");
                            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive failed.");
                            else if (error == OTA_END_ERROR) Serial.println("End failed."); });

        ArduinoOTA.begin();

#ifdef __debugSettings
        Serial.println("ArduinoOTA started.");
#endif
    }

    void execOTA()
    {
        Serial.println("Connecting to: " + String(host));
        // Connect to S3
        if (network::client.connect(host.c_str(), port))
        {
            // Connection Succeed.
            // Fecthing the bin
            Serial.println("Fetching Bin: " + String(bin));

            // Get the contents of the bin file
            network::client.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                                  "Host: " + host + "\r\n" +
                                  "Cache-Control: no-cache\r\n" +
                                  "Connection: close\r\n\r\n");

            // Check what is being sent
            //    Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
            //                 "Host: " + host + "\r\n" +
            //                 "Cache-Control: no-cache\r\n" +
            //                 "Connection: close\r\n\r\n");

            unsigned long timeout = millis();
            while (network::client.available() == 0)
            {
                if (millis() - timeout > 5000)
                {
                    Serial.println("Client Timeout !");
                    network::client.stop();
                    return;
                }
            }
            // Once the response is available,
            // check stuff

            while (network::client.available())
            {
                // read line till /n
                String line = network::client.readStringUntil('\n');
                // remove space, to check if the line is end of headers
                line.trim();

                // if the the line is empty,
                // this is end of headers
                // break the while and feed the
                // remaining `client` to the
                // Update.writeStream();
                if (!line.length())
                {
                    // headers ended
                    break; // and get the OTA started
                }

                // Check if the HTTP Response is 200
                // else break and Exit Update
                if (line.startsWith("HTTP/1.1"))
                {
                    if (line.indexOf("200") < 0)
                    {
                        Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
                        break;
                    }
                }

                // extract headers here
                // Start with content length
                if (line.startsWith("Content-Length: "))
                {
                    contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
                    Serial.println("Got " + String(contentLength) + " bytes from server");
                }

                // Next, the content type
                if (line.startsWith("Content-Type: "))
                {
                    String contentType = getHeaderValue(line, "Content-Type: ");
                    Serial.println("Got " + contentType + " payload.");
                    if (contentType == "application/octet-stream")
                    {
                        isValidContentType = true;
                    }
                }
            }
        }
        else
        {
            // Connect to S3 failed
            // May be try?
            // Probably a choppy network?
            Serial.println("Connection to " + String(host) + " failed. Please check your setup");
            // retry??
            // execOTA();
        }

        // Check what is the contentLength and if content type is `application/octet-stream`
        Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

        // check contentLength and content type
        if (contentLength && isValidContentType)
        {
            // Check if there is enough to OTA Update
            bool canBegin = Update.begin(contentLength);

            // If yes, begin
            if (canBegin)
            {
                Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quiet for a while.. Patience!");
                // No activity would appear on the Serial monitor
                // So be patient. This may take 2 - 5mins to complete
                size_t written = Update.writeStream(network::client);

                if (written == (size_t)contentLength)
                {
                    Serial.println("Written : " + String(written) + " successfully");
                }
                else
                {
                    Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
                    // retry??
                    // execOTA();
                }

                if (Update.end())
                {
                    Serial.println("OTA done!");
                    if (Update.isFinished())
                    {
                        Serial.println("Update successfully completed. Rebooting.");
                        ESP.restart();
                    }
                    else
                    {
                        Serial.println("Update not finished? Something went wrong!");
                    }
                }
                else
                {
                    Serial.println("Error Occurred. Error #: " + String(Update.getError()));
                }
            }
            else
            {
                // not enough space to begin OTA
                // Understand the partitions and
                // space availability
                Serial.println("Not enough space to begin OTA");
                network::client.flush();
            }
        }
        else
        {
            Serial.println("There was no content in the response");
            network::client.flush();
        }
    }

    void setup(){
        initOTA();
    }

    void loop()
    {
        ArduinoOTA.handle();
    }
}
