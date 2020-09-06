/*
 * Master configuration file for the steering wheel
 */

#pragma once

/*
 * DEBUGGING SETTINGD 
 */
// set this variable to enable various serial debugging features
#define DEBUG false 
// set this variable to wait for a connection over serial before continuing
#define WAIT_FOR_SERIAL true
// set this variable to debug the conenction to the mcp2515
#define DEBUG_CAN false
// set this variable to reset ts in eeprom calibration to default 
#define RESET_TS_CALIBRATION false


/*
 * PIN DEFINITIONS
 */
#define MCP_CS PB12
#define MCP_INT PB11
// pin that is connected to reset pin
#define RST PB0


/*
 * CAN BUS CONSTANTS 
 */
#define can_timeout		5000		// time without CAN message before error occurs (ms)
#define transmit_ID		0xA0		// CAN Bus transmit ID (messages will be sent with this ID)
#define recv_id			0x80		// CAN Bus receive ID (messages with this ID will be read)

/*
 * TIMER CONSTANTS
 */

// constants for each refresh rate
// TODO: remove this and be not garbage at programming
#define fifty_hz    20
#define ten_hz      100
#define five_hz     200
#define one_hz      1000


/*
 * EEPROM LAYOUT CONFIG
 */
// TS means Touchscreen
#define TS_CAL_START   0        // location in "eeprom" to start reading from
#define TS_CAL_SIZE    4        // number of  bytes per value 
#define TS_CAL_LEN     6        // number of values to read 

/*
 * LAUNCH CONTROL CONFIG
 */

#define NUM_LAUNCH_PROFS 4      // how many launch profiles to have