#pragma once
#include <inttypes.h>

extern volatile uint32_t &c_time;

class Debouncer{
public:
    Debouncer();
    Debouncer(uint16_t);
    Debouncer(uint16_t, uint16_t);
    bool Deb();
    void Reset();

private:
    bool isDeb_ = false;            // flag for whether timer is running
    uint32_t time_s_ = 0;           // debounce start time
    uint16_t time_deb_ = 50;        // debounce duration
    uint16_t time_recover_ = 50;    // recovery time if resource is held
                                    // before action will occur again
};