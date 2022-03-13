#include <ArduinoJson.h>

#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRremoteESP8266.h>

#include "mqtt.h"
#include "leds.h"

#include "ir-toyotomi-car-27paun-raw.h"
#include "ir-sharp.h"
#include "ir-sony-rm-ed022-raw.h"
#include "ir-sony-rm-lp211-raw.h"
#include "ir-toyotomi-yaw1f6-raw.h"
#include "ir-yamaha-rav536.h"

#define IR_RECEIVE_GPIO 5
#define IR_SEND_GPIO 4

// Change to `true` if you miss/need the old "Raw Timing[]" display.
#define LEGACY_TIMING_INFO false

namespace ir
{

    const uint16_t kCaptureBufferSize = 1024;
    const uint8_t kTimeout = 50;       // Milli-Seconds
    const uint16_t kFrequency = 38000; // in Hz. e.g. 38kHz.
    const uint16_t kMinUnknownSize = 12;

    decode_results results;

    IRrecv irrecv(IR_RECEIVE_GPIO, kCaptureBufferSize, kTimeout, false);
    IRsend irsend(IR_SEND_GPIO);

    void DisabelIRIn()
    {
    }

    void EnableIRIn()
    {
    }

    void SendReceivedIRCode(String protocol, String data)
    {

        const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 180;
        StaticJsonDocument<capacity> doc;

        doc["Protocol"] = protocol;
        doc["Data"] = data;

#ifdef __debugSettings
        serializeJsonPretty(doc, Serial);
        Serial.println();
#endif

        String myJsonString;

        serializeJson(doc, myJsonString);

        mqtt::PublishData(((String) "RESULT").c_str(), myJsonString.c_str(), false);
    }

    void beforeIRSend()
    {
        leds::activityLED_ON();
        irrecv.disableIRIn();
    }

    void afterIRSend()
    {
        irrecv.enableIRIn();
        leds::activityLED_OFF();
    }

    void TransmitRawIRCommand(const uint16_t buf[], const uint16_t size, const String remoteName, String commandName, uint16_t kHz)
    {
        beforeIRSend();
        irsend.sendRaw(buf, size, kHz);
        SendReceivedIRCode(remoteName, commandName);
        afterIRSend();
    }

    void setup()
    {
#if DECODE_HASH
        // Ignore messages with less than minimum on or off pulses.
        irrecv.setUnknownThreshold(kMinUnknownSize);
#endif // DECODE_HASH
        irsend.begin();
        irrecv.enableIRIn();
    }

    void loop()
    {
        if (irrecv.decode(&results))
        {
            // Check if we got an IR message that was to big for our capture buffer.
            if (results.overflow)
                Serial.printf(D_WARN_BUFFERFULL "\r\n", kCaptureBufferSize);
            // Display the library version the message was captured with.
            Serial.println(D_STR_LIBRARY "   : v" _IRREMOTEESP8266_VERSION_ "\r\n");
            // Display the basic output of what we found.
            Serial.println(resultToHumanReadableBasic(&results));
            // Display any extra A/C info if we have it.
            String description = IRAcUtils::resultAcToString(&results);
            if (description.length())
                Serial.println(D_STR_MESGDESC ": " + description);

#if LEGACY_TIMING_INFO
            // Output legacy RAW timing info of the result.
            Serial.println(resultToTimingInfo(&results));
#endif // LEGACY_TIMING_INFO

            // Output the results as source code
            Serial.println(resultToSourceCode(&results));
            Serial.println(); // Blank line between entries
            irrecv.resume();  // Receive the next value

            mqtt::PublishData(((String) "RESULT").c_str(), resultToSourceCode(&results).c_str(), false);
        }
    }

}
