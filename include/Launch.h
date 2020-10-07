#pragma once

#include "MasterConfig.h"
#include <inttypes.h>


/*
 * TYPEDEFS
 */

// data pertaining to tuning launch control from the steering wheel
// TODO: store this in eeprom and fetch on boot, returning to default vals
// if unset
typedef struct s_launch_cnf {
	uint8_t rpm = 6000/100;		// default value for launch rpm
	uint8_t th_speed = 30;		// default value for threshold speed
} launch_cnf_S;

// launch profile struct, contains data about a given launch profile
// WIP
typedef struct s_launch_prof{
	uint8_t prof_number = 0;
	const char * prof_desc = "Default Profile";
	launch_cnf_S conf;
} launch_prof_S;


/*
 * PUBLIC FUNCTIONS
 */

extern uint8_t lc_eep_write(launch_prof_S); 

/*
 * PUBLIC DATA
 */
extern launch_prof_S launch_profs [NUM_LAUNCH_PROFS];
