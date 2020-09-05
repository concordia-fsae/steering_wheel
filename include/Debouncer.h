#pragma once
#include <inttypes.h>

extern uint32_t c_time;

class Debouncer{
public:
    Debouncer();
    Debouncer(uint16_t);
    bool Deb();
    void Reset();

private:
    bool isDeb_ = false;        // flag for whether timer is running
    // bool isSet_ = false;        // flag for whether resource is debounced
    uint32_t time_s_ = 0;       // debounce start time
    uint16_t time_deb_ = 25;    // debounce duration
};