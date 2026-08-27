#ifndef _BatteryLed_H
#define _BatteryLed_H

#include "gpaddon.h"
#include "status_bar.h"

#define BatteryLedName "BatteryLed"

class BatteryLedAddon : public GPAddon {
public:
    virtual bool available();
    virtual void setup();
    virtual void process();
    virtual void preprocess() {}
    virtual void postprocess(bool sent) {}
    virtual void reinit() {}
    virtual std::string name() { return BatteryLedName; }
};

#endif
