#include <Esp.h>

namespace leds
{
#define CONNECTION_STATUS_LED_GPIO 0
#define ACTIVITY_LED_GPIO 16

    void connectionLED_TOGGLE()
    {
        if (digitalRead(CONNECTION_STATUS_LED_GPIO) == LOW)
            digitalWrite(CONNECTION_STATUS_LED_GPIO, HIGH);
        else
            digitalWrite(CONNECTION_STATUS_LED_GPIO, LOW);
    }

    void connectionLED_ON()
    {
        digitalWrite(CONNECTION_STATUS_LED_GPIO, LOW);
    }

    void connectionLED_OFF()
    {
        digitalWrite(CONNECTION_STATUS_LED_GPIO, HIGH);
    }

    void activityLED_TOGGLE()
    {
        if (digitalRead(ACTIVITY_LED_GPIO) == LOW)
            digitalWrite(ACTIVITY_LED_GPIO, HIGH);
        else
            digitalWrite(ACTIVITY_LED_GPIO, LOW);
    }

    void activityLED_ON()
    {
        digitalWrite(ACTIVITY_LED_GPIO, LOW);
    }

    void activityLED_OFF()
    {
        digitalWrite(ACTIVITY_LED_GPIO, HIGH);
    }

    void setup()
    {
        pinMode(CONNECTION_STATUS_LED_GPIO, OUTPUT);
        digitalWrite(CONNECTION_STATUS_LED_GPIO, HIGH);

        pinMode(ACTIVITY_LED_GPIO, OUTPUT);
        digitalWrite(ACTIVITY_LED_GPIO, HIGH);
    }

}
