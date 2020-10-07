#ifndef SCREEN_H
#define SCREEN_H

// Interfaces Used 
#include <inttypes.h>
#include <EEPROM.h>
#include <FT_NHD_43RTP_SHIELD.h>
#include "MasterConfig.h"
#include "shiftlights.h"
#include "VehicleData.h"


/*
 * MACROS
 */

#define SCR_CS_PIN PA0
#define RAM_FONTS_MICROSS 0


/*
 * PRIVATE DATA
 */
static PROGMEM prog_uchar FONTS_MICROSS[] = {
	#include "FONTS_MICROSS.h"
};

// known good default config values to reset to if necessary
static const uint32_t touch_cal_default [6] = {32439, 4294966806,
										4294236873, 4294966575,
										4294947195, 18508876
										};


// TODO: probably move this to VehicleData.h
// warning defs
typedef struct s_warning {
	int16_t current = -1;
	int16_t last = -1;
	uint8_t dismissed = 0;

	char warnings[8][20] = {"No Conn.", "Low Oil Press.", "Low Fuel Press.", "Low Batt. Volt.", "High Cool. Temp.", "High Oil Temp",
							"Low AFR", "High AFR"};
} warning_S;


/*
 * PUBLIC FUNCTIONS
 */
extern void error_overlay();
extern void main_display();
extern void launch_display();
extern void diag_display(bool, bool, bool);
extern bool boot_display();
extern bool check_display();
extern uint16_t ts_btn_pressed();
extern bool ts_eeprom_get();
extern void ts_cal_get();


/*
 * PRIVATE FUNCTIONS
 */
void calibrate_display();
void common_display();
bool ts_write_regs(uint32_t (&)[TS_CAL_LEN]);
void ts_read_write();
void ts_eeprom_write(const uint32_t (&)[TS_CAL_LEN]);


/*
 * PUBLIC DATA 
 */
extern ShiftLights shift_lights;		// bring in shiftligts object so we can control them
extern FT800IMPL_SPI FTImpl;			// define screen obj
extern warning_S warning;				// define the warning struct above

#endif // SCREEN_H
