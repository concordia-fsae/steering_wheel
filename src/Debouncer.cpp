#include <Arduino.h>
#include "Debouncer.h"

/*
 * Default Constructor
 * Sets debounce time to 50ms
 */
Debouncer::Debouncer(){}

// parametrized constructor, parameter is debounce time
/*
 * Parametrized Constructor
 * debounce_time: time to debounce for
 */
Debouncer::Debouncer(uint16_t debounce_time){
    this->time_deb_ = debounce_time;
    this->time_recover_ = debounce_time;
}

/*
 * Parametrized Constructor
 * debounce_time: the time to debounce for
 * recover_time: the amount of time to wait before triggering again
 * if the timer isn't reset in between
 */
Debouncer::Debouncer(uint16_t debounce_time, uint16_t recover_time){
    this->time_deb_ = debounce_time;
    this->time_recover_ = recover_time;
}
/*
 * Multi-purpose function.
 * If the debounce timer hasn't started, start it and return 0
 * If the debounce timer has started but hasn't expired, return 0
 * If the debounce timer has started and has expired, return 1 and reset the debouncing flag
 */
bool Debouncer::Deb(){
    if(!isDeb_ && (!time_s_ || !time_recover_)){
    // start debouncing if we're not already
        time_s_ = c_time;
        isDeb_ = true;
    } else if(!isDeb_ && c_time - time_s_ > time_recover_){
    // only start debouncing if we didn't just trigger
        time_s_ = c_time;
        isDeb_ = true;
    } else if(isDeb_ && c_time - time_s_ > time_deb_){
    // trigger if the timer has elapsed
        if(time_recover_){
            // if the recover time is 0 it means we want
            // continuous output, so only reset if recover_time is nonzero
            isDeb_ = false;
            time_s_ = c_time;
        }
        return 1;
    }
    return 0;
}

/*
 * Debounce reset. Use when a button gets un-set before debounce timer has expired
 */
void Debouncer::Reset(){
    // reset the timer once the resource is released
    isDeb_ = false;
    time_s_ = 0;
}
