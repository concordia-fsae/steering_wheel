/*
 * CANBus.h
 */

// Interfaces Used 
#include "mcp_can_stm.h"
#include <inttypes.h>

#ifndef CANBUS_H
#define CANBUS_H

/*
 * MACROS
 */

#define can_timeout		5000		// time without CAN message before error occurs (ms)
#define transmit_ID		0xA0		// CAN Bus transmit ID (messages will be sent with this ID)
#define recv_id			0x80		// CAN Bus receive ID (messages with this ID will be read)

// I/O state vals
// word 0
#define sw1_st	0b10000000
#define sw2_st	0b01000000
#define sw3_st	0b00100000
#define sw4_st	0b00010000
#define tl_st	0b00001000
#define tr_st	0b00000100
#define pl_st	0b00000010
#define pr_st	0b00000001
// word 1
#define mid_st			0b10000000
#define fuel_pump		0b01000000
#define remote_start_st	0b00100000



/*
 * data pertaining to tuning launch control from the steering wheel
 * TODO: store this in eeprom and fetch on boot, returning to default vals
 * if unset
 */
typedef struct s_launch_cnf {
	uint8_t rpm = 6000/100;		// default value for launch rpm
	uint8_t th_speed = 30;		// default value for threshold speed
} launch_cnf_S;

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

/*
 * TYPEDEFS
 */
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



// External Variables
extern veh_data_S veh;
extern uint32_t last_can_update;


// Public Functions
extern void can_receive();

#endif // CANBUS_H
