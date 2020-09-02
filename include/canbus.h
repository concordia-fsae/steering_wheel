/*
 * CANBus.h
 */

// Interfaces Used 
#include "mcp_can_stm.h"

#ifndef CANBUS_H
#define CANBUS_H

/*
 * MACROS
 */

#define can_timeout		5000		// time without CAN message before error occurs
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
#define mid_st	0b10000000
#define fuel_pump 0b01000000
#define remote_start_st 0b00100000


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

} veh_data_S;

typedef struct s_io_state {
	bool sw1;
	bool sw2;
	bool sw3;
	bool sw4;
	bool pl;
	bool pr;
	bool tl;
	bool tr;
	bool mid;
	bool remote_start;
} io_state_S;

typedef struct s_launch_cnf {
	uint8_t launch_rpm = 6000/100;		// default value for launch rpm
	uint8_t launch_th_speed = 30;		// default value for threshold speed
} launch_cnf_S;

/*
 * PUBLIC DATA
 */
veh_data_S veh;
io_state_S io;
launch_cnf_S launch;


/*
 * PRIVATE DATA
 */

// CAN bus def
MCP_CAN CAN;
uint32_t last_can_update = 0;	// last time a CAN message was received

// float voltage = 0;
// int v_int, v_fl, o_temp, o_press, c_temp, f_press, f_temp, gear, rpm = 0;
// bool sw1, sw2, sw3, sw4, pl, pr, tl, tr, mid, launch, traction, autoshift, remote_start;




// Public Functions
void can_receive();

// implementation needs to be moved to c file
void can_receive(){
	uint8_t recv_len;				// length of received message
	uint8_t recv_msg[9];			// the message (9 bytes)
	
	CAN.readMsgBuf(&recv_len, recv_msg);
	
	if(CAN.getCanId() == recv_id){
		veh.gear = ((recv_msg[0] & 0b11110000) >> 4) - 2;
		veh.launch = (recv_msg[0] & 0b00001000) >> 3;
		veh.traction = (recv_msg[0] & 0b00000100) >> 2;
		veh.autoshift = (recv_msg[1] & 0b11110000) >> 4;
		veh.rpm = (recv_msg[2] * 60);
		veh.voltage = (recv_msg[3] / 10.0);
		veh.f_press = (recv_msg[4] * 4);
		veh.o_temp = (recv_msg[5] * 4);
		veh.c_temp = (recv_msg[6] * 4);
		veh.o_press = (recv_msg[7] * 4);
	} else if(CAN.getCanId() == recv_id + 2) {
		veh.f_temp = (recv_msg[0] * 4);
	}
}

#endif // CANBUS_H
