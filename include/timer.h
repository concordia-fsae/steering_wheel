#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>

//Something probably has to go here that I forgot

class Timer{
    public:
        uint32_t c_time;
        bool timeElapsed;
        void updateTimer();

    private:
        uint16_t frequency;
}
#endif //TIMER_H