#include <LittleFS.h>

#define FORMAT_SPIFFS_IF_FAILED true

namespace filesystem
{

    void setup()
    {
        if (!LittleFS.begin())
        {
            Serial.println("Error: Failed to initialize the filesystem!");
        }
    }

}