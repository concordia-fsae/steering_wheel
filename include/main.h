#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <inttypes.h>
#include "shiftlights.h"
#include "canbus.h"
#include "screen.h"


// pin that is connected to rst
#define RST PB0

unsigned long int c_time = 0;
unsigned long int l_time_50 = 0;		// last time a 50Hz event occured
unsigned long int l_time_10 = 0;		// last time a 10Hz event occured
unsigned long int l_time_5 = 0;			// last time a 5Hz event occured
unsigned long int l_time_1 = 0;			// last time a 1Hz event occured

// constants for each refresh rate
unsigned int fifty_hz = 20;
unsigned int ten_hz = 100;
unsigned int five_hz = 200;
unsigned int one_hz = 1000;

// debounce vars
uint32_t diag_fuel_deb = 0;
uint32_t tr_deb = 0;		// debounce var for top right btn
uint32_t launch_rpm_plus_deb = 0;
uint32_t launch_rpm_minus_deb = 0;
uint32_t launch_thresh_plus_deb = 0;
uint32_t launch_thresh_minus_deb = 0;

// status vars
bool led_err_st = 0;
bool led_clear = 1;
bool diag_shift = 0;
bool diag_rst = 0;
bool diag_fuel_st = 0;

// Local Function Prototypes
void get_inputs(uint8_t&, uint8_t&);

// External Variables
extern veh_data_S veh;
extern io_state_S io;
extern launch_cnf_S launch;

#endif // MAIN_H
