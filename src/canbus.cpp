#include "canbus.h"


/*
 * PUBLIC DATA
 */
veh_data_S veh;
// io_state_S io;
// launch_cnf_S launch;

// CAN bus def
MCP_CAN CAN;
uint32_t last_can_update = 0;	// last time a CAN message was received

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
