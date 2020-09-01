#ifndef CANBUS_H
#define CANBUS_H

#include "mcp_can_stm.h"

// CAN bus def
MCP_CAN CAN;

unsigned long int last_can_update = 0;	// last time a CAN message was received
unsigned int can_timeout = 5000;		// time without CAN message before error occurs

// switch state vals
#define sw1_st	0b10000000
#define sw2_st	0b01000000
#define sw3_st	0b00100000
#define sw4_st	0b00010000
#define tl_st	0b00001000
#define tr_st	0b00000100
#define pl_st	0b00000010
#define pr_st	0b00000001

#define mid_st	0b10000000
#define fuel_pump 0b01000000
#define remote_start_st 0b00100000

float voltage = 0;
int v_int, v_fl, o_temp, o_press, c_temp, f_press, f_temp, gear, rpm = 0;
bool sw1, sw2, sw3, sw4, pl, pr, tl, tr, mid, launch, traction, autoshift, remote_start;


uint8_t launch_rpm = 6000/100;	// default value for launch rpm
uint8_t launch_th_speed = 30;	// default value for threshold speed
uint16_t transmit_ID = 0xA0;	// CAN Bus transmit ID (messages will be sent with this ID)
uint32_t recv_id = 0x80;		// CAN Bus receive ID (messages with this ID will be read)


// Local Function Definition
void can_receive();


void can_receive(){
	uint8_t recv_len;				// length of received message
	uint8_t recv_msg[9];			// the message (9 bytes)
	
	CAN.readMsgBuf(&recv_len, recv_msg);
	
	if(CAN.getCanId() == recv_id){
		gear = ((recv_msg[0] & 0b11110000) >> 4) - 2;
		launch = (recv_msg[0] & 0b00001000) >> 3;
		traction = (recv_msg[0] & 0b00000100) >> 2;
		autoshift = (recv_msg[1] & 0b11110000) >> 4;
		rpm = (recv_msg[2] * 60);
		voltage = (recv_msg[3] / 10.0);
		f_press = (recv_msg[4] * 4);
		o_temp = (recv_msg[5] * 4);
		c_temp = (recv_msg[6] * 4);
		o_press = (recv_msg[7] * 4);
	} else if(CAN.getCanId() == recv_id + 2) {
		f_temp = (recv_msg[0] * 4);
	}
}

#endif // CANBUS_H
