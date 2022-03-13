#include <os_type.h>
#include <ESP8266WiFi.h>

#include "common.h"
#include "timechangerules.h"
#include "settings.h"

namespace common
{
    TimeChangeRule *tcr;

    //   !  For "strftime" to work delete Time.h file in TimeLib library  !!!
    char *GetFullDateTime(const char *formattingString, size_t size)
    {
        time_t localTime = timechangerules::timezones[settings::timeZone]->toLocal(now(), &tcr);
        struct tm *now = localtime(&localTime);
        char *buf = new char[20];
        strftime(buf, 20, formattingString, now);
        return buf;
    }

    void SetRandomSeed()
    {
        uint32_t seed;

        // random works best with a seed that can use 31 bits
        // analogRead on a unconnected pin tends toward less than four bits
        seed = analogRead(0);
        delay(1);

        for (int shifts = 3; shifts < 31; shifts += 3)
        {
            seed ^= analogRead(0) << shifts;
            delay(1);
        }

        randomSeed(seed);
    }

    String DateTimeToString(const time_t time)
    {

        String myTime = "";
        char s[2];

        //  years
        itoa(year(time), s, DEC);
        myTime += s;
        myTime += "-";

        //  months
        itoa(month(time), s, DEC);
        myTime += s;
        myTime += "-";

        //  days
        itoa(day(time), s, DEC);
        myTime += s;

        myTime += " ";

        //  hours
        itoa(hour(time), s, DEC);
        myTime += s;
        myTime += ":";

        //  minutes
        if (minute(time) < 10)
            myTime += "0";

        itoa(minute(time), s, DEC);
        myTime += s;
        myTime += ":";

        //  seconds
        if (second(time) < 10)
            myTime += "0";

        itoa(second(time), s, DEC);
        myTime += s;

        return myTime;
    }

    String TimeIntervalToString(const time_t time)
    {

        String myTime = "";
        char s[2];

        //  hours
        itoa((time / 3600), s, DEC);
        myTime += s;
        myTime += ":";

        //  minutes
        if (minute(time) < 10)
            myTime += "0";

        itoa(minute(time), s, DEC);
        myTime += s;
        myTime += ":";

        //  seconds
        if (second(time) < 10)
            myTime += "0";

        itoa(second(time), s, DEC);
        myTime += s;
        return myTime;
    }

    String GetDeviceMAC()
    {
        String s = WiFi.macAddress();

        for (size_t i = 0; i < 5; i++)
            s.remove(14 - i * 3, 1);

        return s;
    }

    void setup()
    {
        SetRandomSeed();
    }
}
