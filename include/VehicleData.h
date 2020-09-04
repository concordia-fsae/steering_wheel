#pragma once

#include <inttypes.h>

/*
 * TYPEDEFS
 */


/*
 * data pertaining to tuning launch control from the steering wheel
 * TODO: store this in eeprom and fetch on boot, returning to default vals
 * if unset
 */
typedef struct s_launch_cnf {
	uint8_t rpm = 6000/100;		// default value for launch rpm
	uint8_t th_speed = 30;		// default value for threshold speed
} launch_cnf_S;

/*
 * Data that pertains to Steering Wheel IO
 */

typedef struct s_io_state {
	/* the four switches on the bottom of the steering wheel
	 * not counting the one in the middle
	 * numbered from left to right
	 */
	bool sw1;
	bool sw2;
	bool sw3;
	bool sw4;
	/*
	 * left and right paddle buttons
	 */
	bool pl;
	bool pr;
	/*
	 * front top left and right buttons
	 */
	bool tl;
	bool tr;
	// lower middle button
	bool mid;
	/*  remote start FLAG, not IO. True when both TL and TR have been
	 * pressed for sufficiently long
	 * TODO: move this somewhere else maybe? idk
	 */
	bool remote_start;
} io_state_S;

typedef struct s_veh_data {
	int8_t gear;
	bool launch;
	bool traction;
	bool autoshift;
	uint16_t rpm;
	uint16_t f_press;
	uint16_t o_press;
	uint16_t o_temp;
	uint16_t c_temp;
	uint16_t f_temp;

	float voltage;
	// this needs to be replaced with a pure function
	int v_int;
	int v_fl;


	launch_cnf_S launch_cnf;
	io_state_S io;
} veh_data_S;


// Public Variables
extern veh_data_S veh;