/*
 * CANBus.h
 */

#ifndef CANBUS_H
#define CANBUS_H

// Interfaces Used 
#include "mcp_can_stm.h"
#include <inttypes.h>
#include "MasterConfig.h"
#include "VehicleData.h"


/*
 * MACROS
 */
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
