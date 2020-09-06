/*
 * CANBus.h
 */

#ifndef CANBUS_H
#define CANBUS_H

// Interfaces Used 
#include "mcp_can_stm.h"
#include <inttypes.h>
#include "VehicleData.h"


/*
 * MACROS
 */

#define MCP_CS PB12
#define MCP_INT PB11

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


// External Data
extern uint32_t last_can_update;
extern veh_data_S veh;

// Public Functions
extern void can_receive();

#endif // CANBUS_H
