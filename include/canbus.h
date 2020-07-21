#ifndef CANBUS_H
#define CANBUS_H

#include "mcp_can_stm.h"

// CAN bus def
MCP_CAN CAN;

unsigned long int last_can_update = 0;	// last time a CAN message was received
unsigned int can_timeout = 5000;					// time without CAN message before error occurs

// switch state vals
#define sw1_st	128
#define sw2_st	64
#define sw3_st	32
#define sw4_st	16
#define tl_st	8
#define tr_st	4
#define pl_st	2
#define pr_st	1
#define mid_st	128
#define fuel_pump 64
#define remote_start_st 32

float voltage;
int v_int, v_fl, o_temp, o_press, c_temp, f_press, f_temp, gear, rpm;
bool sw1, sw2, sw3, sw4, pl, pr, tl, tr, mid, launch, traction, autoshift, remote_start;


uint8_t launch_rpm = 6000/100;	// default value for launch rpm
uint8_t launch_th_speed = 30;	// default value for threshold speed
uint8_t status1 = 0;			// first variable containing button states
uint8_t status2 = 0;			// second variable containing button states
uint16_t transmit_ID = 0xA0;	// CAN Bus transmit ID (messages will be sent with this ID)
uint32_t recv_id = 0x80;		// CAN Bus receive ID (messages with this ID will be read)
uint8_t recv_len;				// length of received message
uint8_t recv_msg[9];			// the message (4 bytes)
uint8_t buff [7] = {status1, status2, launch_th_speed, launch_rpm};	// message to be sent, contains statuses


#endif // CANBUS_H
