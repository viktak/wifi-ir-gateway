#ifndef IR_H
#define IR_H

#include <IRrecv.h>
#include <IRsend.h>

namespace ir
{

    extern void DisabelIRIn();
    extern void EnableIRIn();

    void TransmitRawIRCommand(const uint16_t buf[], const uint16_t size, const String remoteName, String commandName, uint16_t kHz);

    extern void setup();
    extern void loop();
}
#endif