#ifndef LEDS_H
#define LEDS_H

namespace leds
{
    extern void connectionLED_TOGGLE();
    extern void connectionLED_ON();
    extern void connectionLED_OFF();

    extern void activityLED_TOGGLE();
    extern void activityLED_ON();
    void activityLED_OFF();

    extern void setup();
}

#endif