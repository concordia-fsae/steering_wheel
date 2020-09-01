//Timer

#include "timer.h"

Timer(uint16_t freq){
    frequency = freq;
}

//This method updates the timer. Should take into consideration the frequeuncy being used
//I need to think of some way to make this method see the elapsed time so check if enough time has passed
public void update(){
    c_time = millis();
    period = (1/frequency)*1000;

    if(c_time - period > ){

    }

}

