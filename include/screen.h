#ifndef SCREEN_H
#define SCREEN_H

// Interfaces Used 
#include <inttypes.h>
#include <FT_NHD_43RTP_SHIELD.h>
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


// touchscreen calibration for orange wheel
extern uint32_t touch_matrix [];


// warning defs
typedef struct s_warning {
	int16_t current = -1;
	int16_t last = -1;
	uint8_t dismissed = 0;

	char warnings[8][20] = {"No Conn.", "Low Oil Press.", "Low Fuel Press.", "Low Batt. Volt.", "High Cool. Temp.", "High Oil Temp",
							"Low AFR", "High AFR"};
} warning_S;



// Function Prototypes 
// TODO: Split into public and private
extern void calibrate_display();
extern void common_display();
extern void error_overlay();
extern void main_display();
extern void launch_display();
extern void diag_display(bool, bool, bool);
extern bool boot_display();
extern bool check_display();

/*
 * PUBLIC VARIABLES
 */

extern ShiftLights shift_lights;		// bring in shiftligts object so we can control them
extern FT800IMPL_SPI FTImpl;			// define screen obj
extern sTagXY sTagxy;					// define touchscreen obj
extern warning_S warning;				// define the warning struct above

#endif // SCREEN_H
