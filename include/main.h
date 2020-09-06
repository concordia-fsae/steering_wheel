#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <inttypes.h>
#include "VehicleData.h"
#include "shiftlights.h"
#include "canbus.h"
#include <FT_NHD_43RTP_SHIELD.h>
#include "screen.h"
#include "Debouncer.h"



// pin that is connected to rst
#define RST PB0


// TODO: replace all of this with a timer class
extern uint32_t c_time;
uint32_t l_time_50 = 0;		    // last time a 50Hz event occured
uint32_t l_time_10 = 0;		    // last time a 10Hz event occured
uint32_t l_time_5 = 0;			// last time a 5Hz event occured
uint32_t l_time_1 = 0;			// last time a 1Hz event occured


// constants for each refresh rate
uint16_t fifty_hz = 20;
uint16_t ten_hz = 100;
uint16_t five_hz = 200;
uint16_t one_hz = 1000;
// end todo


// debounce vars
Debouncer remote_start = Debouncer(500, 0);
Debouncer diag_fuel_deb = Debouncer(200, 500);
Debouncer launch_rpm_plus = Debouncer();
Debouncer launch_rpm_minus = Debouncer();
Debouncer launch_thresh_minus = Debouncer();
Debouncer launch_thresh_plus = Debouncer();

// end todo


// status vars
bool led_err_st = 0;
bool led_clear = 1;
bool diag_shift = 0;
bool diag_rst = 0;
bool diag_fuel_st = 0;

// Local Function Prototypes
void get_inputs(uint8_t&, uint8_t&);

// External Variables
//TODO move all CAN stuff out of main and then delete this variable
extern MCP_CAN CAN;
#endif // MAIN_H
