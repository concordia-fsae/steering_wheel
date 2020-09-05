#include <Arduino.h>
#include "Debouncer.h"

Debouncer::Debouncer(){}

Debouncer::Debouncer(uint16_t debounce_time){
    this->time_deb_ = debounce_time;
}

bool Debouncer::Deb(){
    // start debouncing if we're not already
    if(!isDeb_){
        time_s_ = c_time;
        isDeb_ = true;
    } else if(c_time - time_s_ > time_deb_){
        isDeb_ = false;
        return 1;
    }
    return 0;
}

void Debouncer::Reset(){
    isDeb_ = false;
}
