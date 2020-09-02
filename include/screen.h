#ifndef SCREEN_H
#define SCREEN_H

// Interfaces Used 
#include <inttypes.h>
#include <FT_NHD_43RTP_SHIELD.h>
#include "shiftlights.h"

/*
 * MACROS
 */

#define MCP_CS PB12
#define MCP_INT PB11
#define SCR_CS_PIN PA0
#define RAM_FONTS_MICROSS 0

/*
 * PRIVATE DATA
 */
static PROGMEM prog_uchar FONTS_MICROSS[] = {
	#include "FONTS_MICROSS.h"
};

// define screen object
FT800IMPL_SPI FTImpl(SCR_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

// touchscreen object
sTagXY sTagxy;

// touchscreen calibration for orange wheel
uint32_t touch_matrix [6] = {32439, 4294966806, 4294236873, 4294966575, 4294947195, 18508876};

// warning defs
int warning = -1;
char warnings [8][20] = {"No Conn.", "Low Oil Press.", "Low Fuel Press.", "Low Batt. Volt.", "High Cool. Temp.", "High Oil Temp",
						"Low AFR", "High AFR"};
int last_warn = -1;
uint8_t dismissed = 0;


// Function Prototypes 
void calibrate_display();
void common_display();
void error_overlay();
void main_display();
void launch_display();
void diag_display(bool, bool, bool);
bool boot_display();
bool check_display();

/*
 * EXTERNAL VARIABLES
 */

extern ShiftLights shift_lights;
// extern s_veh_data veh_data_S;

// ---------------------------------- Helper Functions ----------------------------------

/*
 * Start up the screen
 */
bool boot_display(){
	uint32_t chipid;
	FTImpl.Init(FT_DISPLAY_RESOLUTION, 0);		//configure the display to the WQVGA
	chipid = FTImpl.Read32(FT_ROM_CHIPID);
	/* Identify the chip */
	int counter = 0;
	while(FT800_CHIPID != chipid){
		shift_lights.SetAllColor(0xFF0000);
//		Serial.print("Error in chip id read ");
//		Serial.println(chipid,HEX);
		FTImpl.Init(FT_DISPLAY_RESOLUTION, 0);
		//delay(20);
		chipid = FTImpl.Read32(FT_ROM_CHIPID);
		counter++;
		if (counter == 10){
			return 1;
		}
	}

	shift_lights.Clear();

	/* Set the Display & audio pins */
	FTImpl.SetDisplayEnablePin(FT_DISPENABLE_PIN);
	FTImpl.SetAudioEnablePin(FT_AUDIOENABLE_PIN);

	// turn the screen on and the amplifier off
	FTImpl.DisplayOn();
	FTImpl.AudioOff();

	// set touchscreen calibration values to what we've acquired
	FTImpl.Write32(REG_TOUCH_TRANSFORM_A, touch_matrix[0]);
	FTImpl.Write32(REG_TOUCH_TRANSFORM_B, touch_matrix[1]);
	FTImpl.Write32(REG_TOUCH_TRANSFORM_C, touch_matrix[2]);
	FTImpl.Write32(REG_TOUCH_TRANSFORM_D, touch_matrix[3]);
	FTImpl.Write32(REG_TOUCH_TRANSFORM_E, touch_matrix[4]);
	FTImpl.Write32(REG_TOUCH_TRANSFORM_F, touch_matrix[5]);
	FTImpl.Cmd_SetMatrix();

	// prepare the font to be displayed (gear and rpm)
	FTImpl.DLStart();
	FTImpl.Cmd_Inflate(RAM_FONTS_MICROSS);
	FTImpl.WriteCmdfromflash(FONTS_MICROSS, sizeof(FONTS_MICROSS));
	FTImpl.DLEnd();
	FTImpl.DLStart();
	FTImpl.BitmapHandle(0);
	FTImpl.BitmapSource(-178412);
	FTImpl.BitmapLayout(FT_L4, 40, 93);
	FTImpl.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, 80, 93);
	FTImpl.Cmd_SetFont(0, 0);
	FTImpl.DLEnd();
	FTImpl.Finish();

	return 0;
}


/*
 * Verify successful connection to screen by reading the FT800 chip
 * id from its memory and comparing with the known good value
*/
bool check_display(){
	uint32_t chipid;
	chipid = FTImpl.Read32(FT_ROM_CHIPID);
	/* Identify the chip */
	if(FT800_CHIPID != chipid) return 0;

	return 1;
}


/*
 * If called, prompts the user to click on 6 points on the screen.
 * Generates calibration data
*/
void calibrate_display(){
	FTImpl.DLStart();

	FTImpl.Cmd_Calibrate(0);

	FTImpl.DLEnd();
	FTImpl.Finish();

	touch_matrix[0] = FTImpl.Read32(REG_TOUCH_TRANSFORM_A);
	touch_matrix[1] = FTImpl.Read32(REG_TOUCH_TRANSFORM_B);
	touch_matrix[2] = FTImpl.Read32(REG_TOUCH_TRANSFORM_C);
	touch_matrix[3] = FTImpl.Read32(REG_TOUCH_TRANSFORM_D);
	touch_matrix[4] = FTImpl.Read32(REG_TOUCH_TRANSFORM_E);
	touch_matrix[5] = FTImpl.Read32(REG_TOUCH_TRANSFORM_F);
}


// Pill Maker
void make_pill(int x, int y)		// coords are in the form {x, y}
{
	FTImpl.PointSize(246);
	FTImpl.Begin(FT_POINTS);
	FTImpl.Vertex2ii(x - 25, y, 1, 0);
	FTImpl.Vertex2ii(x + 25, y, 1, 0);
	FTImpl.End();
	FTImpl.Begin(FT_RECTS);
	FTImpl.Vertex2ii(x - 25, y - 15, 0, 0);
	FTImpl.Vertex2ii(x + 25, y + 15, 0, 0);
	FTImpl.End();
}

void error_overlay(){
	FTImpl.Begin(FT_RECTS);

	FTImpl.ColorA(64);
	FTImpl.ColorRGB(71, 71, 71);
	FTImpl.Vertex2ii(0, 0, 0, 0);
	FTImpl.Vertex2ii(480, 272, 0, 0);

	FTImpl.ColorA(255);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Vertex2ii(50, 50, 0, 0);
	FTImpl.Vertex2ii(433, 222, 0, 0);
	FTImpl.End();

	FTImpl.ColorRGB(255, 0, 0);
	FTImpl.Cmd_Text(240, 136, 30, FT_OPT_CENTER, warnings[warning]);
}


void main_display(){
	uint32_t col = 0;
	int x = 0;
	int y = 0;

	// Top Left Pill (Battery Voltage)
	col = (veh.voltage > 12) ? 0x00FF00 : 0x0000FF;
	col = (veh.voltage <= 9) ? 0xFF0000 : col;

	x = 45;
	y = 105;
	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(x, y-25, 26, FT_OPT_CENTER, "B_Volt");
	FTImpl.ColorRGB(col);
	make_pill(x, y);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Number(x-15, y, 28, FT_OPT_CENTER, veh.v_int);
	FTImpl.Cmd_Text(x, y, 28, FT_OPT_CENTER, ".");
	FTImpl.Cmd_Number(x+7, y, 28, FT_OPT_CENTER, veh.v_fl);
	FTImpl.Cmd_Text(x+27, y, 28, FT_OPT_CENTER, "V");


	// Top Right Pill (Coolant Temp)
	col = (veh.c_temp > 100) ? 0xFF0000 : 0x00FF00;
	col = (veh.c_temp <= 80) ? 0x0000FF : col;
	col = (veh.c_temp <= 40) ? 0xFF0000 : col;

	x = 435;
	y = 105;

	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(x, y-25, 26, FT_OPT_CENTER, "C_Temp");
	FTImpl.ColorRGB(col);
	make_pill(x, y);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Number(x-15, y, 28, FT_OPT_SIGNED | FT_OPT_CENTER, veh.c_temp);
	FTImpl.Cmd_Text(x+27, y, 28, FT_OPT_CENTER, "C");
	FTImpl.Cmd_Text(x+17, y-10, 26, FT_OPT_CENTER, "o");


	// Middle Left Pill (Oil Pressure)
	x = 45;
	y = 170;
	col = (veh.o_press > 380) ? 0x00FF00 : 0x0000FF;
	col = (veh.o_press <= 100) ? 0xFF0000 : col;

	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(x, y-25, 26, FT_OPT_CENTER, "O_Press");
	FTImpl.ColorRGB(col);
	make_pill(x, y);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Number(x-15, y, 28, FT_OPT_SIGNED | FT_OPT_CENTER, veh.o_press);
	FTImpl.Cmd_Text(x+27, y, 28, FT_OPT_CENTER, "C");
	FTImpl.Cmd_Text(x+17, y-8, 26, FT_OPT_CENTER, "o");


	// Middle Right Pill (Fuel Pressure)
	col = (veh.f_press > 380) ? 0x00FF00 : 0x0000FF;
	col = (veh.f_press <= 100) ? 0xFF0000 : col;

	x = 435;
	y = 170;
	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(x, y-25, 26, FT_OPT_CENTER, "F_Press");
	FTImpl.ColorRGB(col);
	make_pill(x, y);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Number(x-21, y, 28, FT_OPT_SIGNED | FT_OPT_CENTER, veh.f_press);
	FTImpl.Cmd_Text(x+18, y, 28, FT_OPT_CENTER, "kPa");


	// Bottom Left Pill (Oil Temp)
	col = (veh.o_temp > 115) ? 0xFF0000 : 0x00FF00;
	col = (veh.o_temp <= 80) ? 0x0000FF : col;
	col = (veh.o_temp <= 40) ? 0xFF0000 : col;

	x = 45;
	y = 235;

	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(x, y-25, 26, FT_OPT_CENTER, "O_Temp");
	FTImpl.ColorRGB(col);
	make_pill(x, y);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Number(x-15, y, 28, FT_OPT_SIGNED | FT_OPT_CENTER, veh.o_temp);
	FTImpl.Cmd_Text(x+27, y, 28, FT_OPT_CENTER, "C");
	FTImpl.Cmd_Text(x+17, y-8, 26, FT_OPT_CENTER, "o");


	// Bottom Right Pill (Fuel Temp)
	x = 435;
	y = 235;
	col = (veh.f_temp > 380) ? 0x00FF00 : 0x0000FF;
	col = (veh.f_temp <= 100) ? 0xFF0000 : col;

	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(x, y-25, 26, FT_OPT_CENTER, "F_Temp");
	FTImpl.ColorRGB(col);
	make_pill(x, y);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Number(x-15, y, 28, FT_OPT_SIGNED | FT_OPT_CENTER, veh.f_temp);
	FTImpl.Cmd_Text(x+27, y, 28, FT_OPT_CENTER, "C");
	FTImpl.Cmd_Text(x+17, y-8, 26, FT_OPT_CENTER, "o");

	FTImpl.ColorRGB(255, 255, 255);
	FTImpl.Cmd_Number(240, 45, 0, FT_OPT_CENTER, veh.rpm);

	const char * gears [6]= {"N", "1", "2", "3", "4", "5"};

	if(veh.gear > -1 && veh.gear < 7){
		FTImpl.Cmd_Text(240, 156, 0, FT_OPT_CENTER, gears[veh.gear]);
	} else {
		FTImpl.Cmd_Text(240, 156, 0, FT_OPT_CENTER, "?");
	}

	common_display();
}

// display elements that are shown on most pages
void common_display()
{
	FTImpl.PointSize(160);
	FTImpl.Begin(FT_POINTS);
	
	uint32_t col;

	col = (io.tl) ? 0x00FF00 : 0xFFFFFF;
	FTImpl.ColorRGB(col);
	FTImpl.Vertex2ii(20, 35, 1, 0);

	col = (io.tl) ? 0x00FF00 : 0xFFFFFF;
	FTImpl.ColorRGB(col);
	FTImpl.Vertex2ii(445, 35, 1, 0);

	col = (io.tl) ? 0x00FF00 : 0xFFFFFF;
	FTImpl.ColorRGB(col);
	FTImpl.Vertex2ii(150, 255, 1, 0);

	col = (io.tl) ? 0x00FF00 : 0xFFFFFF;
	FTImpl.ColorRGB(col);
	FTImpl.Vertex2ii(210, 255, 1, 0);

	col = (io.tl) ? 0x00FF00 : 0xFFFFFF;
	FTImpl.ColorRGB(col);
	FTImpl.Vertex2ii(270, 255, 1, 0);
	FTImpl.End();


	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(20, 15, 21, FT_OPT_CENTER, "DRS");

	FTImpl.ColorRGB(0xFFFFF);
	FTImpl.Cmd_Text(445, 15, 21, FT_OPT_CENTER, "LC");

	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(150, 235, 21, FT_OPT_CENTER, "LC");

	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(210, 235, 21, FT_OPT_CENTER, "AS");

	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(270, 237, 21, FT_OPT_CENTER, "CTRL");


	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(330, 237, 21, FT_OPT_CENTER, "WC");

	const char *d_w = !io.sw4 ? "D" : "W";
	FTImpl.Cmd_Text(330, 255, 28, FT_OPT_CENTER, d_w);


	if(io.remote_start){
		FTImpl.ColorRGB(255, 0, 0);
		FTImpl.Cmd_Text(240, 208, 27, FT_OPT_CENTER, "IGN");
	}
}

void launch_display(){

	FTImpl.ColorRGB(255, 255, 255);
	FTImpl.Begin(FT_LINES);
	FTImpl.Vertex2ii(160, 0, 0, 0);
	FTImpl.Vertex2ii(160, 272, 0, 0);
	FTImpl.End();

	FTImpl.Begin(FT_LINES);
	FTImpl.Vertex2ii(320, 0, 0, 0);
	FTImpl.Vertex2ii(320, 272, 0, 0);
	FTImpl.End();

	FTImpl.Cmd_Text(80, 15, 28, FT_OPT_CENTER, "Best");
	FTImpl.Cmd_Text(240, 15, 28, FT_OPT_CENTER, "Last");
	FTImpl.Cmd_Text(240, 32, 28, FT_OPT_CENTER, "Launch");
	FTImpl.Cmd_Text(400, 15, 28, FT_OPT_CENTER, "Current");

	FTImpl.Cmd_Text(348, 35, 23, FT_OPT_CENTER, "RPM:");
	FTImpl.Cmd_Number(400, 55, 30, FT_OPT_CENTER, launch.launch_rpm*100);
	FTImpl.Tag(4);
	FTImpl.Cmd_Button(340, 70, 48, 48, 27, 0, "-");
	FTImpl.Tag(5);
	FTImpl.Cmd_Button(410, 70, 48, 48, 27, 0, "+");

	FTImpl.Cmd_Text(355, 133, 23, FT_OPT_CENTER, "Thresh:");
	FTImpl.Cmd_Number(400, 155, 30, FT_OPT_CENTER, launch.launch_th_speed);
	FTImpl.Tag(6);
	FTImpl.Cmd_Button(335, 170, 48, 48, 27, 0, "-");
	FTImpl.Tag(7);
	FTImpl.Cmd_Button(414, 170, 48, 48, 27, 0, "+");
	FTImpl.DLEnd();


	common_display();
}


void diag_display(bool diag_shift, bool diag_fuel_st, bool diag_rst){
	FTImpl.Cmd_Text(240, 32, 28, FT_OPT_CENTER, "Diagnostics");

	FTImpl.Begin(FT_POINTS);
	FTImpl.PointSize(600);

	FTImpl.Tag(1);
	if(diag_shift) FTImpl.ColorRGB(0, 255, 0);
	FTImpl.Vertex2ii(100, 145, 0, 0);
	FTImpl.ColorRGB(255, 255, 255);

	FTImpl.Tag(2);
	if(diag_fuel_st) FTImpl.ColorRGB(0, 255, 0);
	FTImpl.Vertex2ii(230, 145, 0, 0);
	FTImpl.ColorRGB(255, 255, 255);

	FTImpl.Tag(3);
	if(diag_rst) FTImpl.ColorRGB(0, 255, 0);
	FTImpl.Vertex2ii(360, 145, 0, 0);
	FTImpl.ColorRGB(255, 255, 255);

	FTImpl.End();


	if(diag_shift) FTImpl.ColorRGB(0, 255, 0);
	FTImpl.Cmd_Text(100, 90, 28, FT_OPT_CENTER, "Shift Lights");
	FTImpl.ColorRGB(255, 255, 255);

	if(diag_fuel_st) FTImpl.ColorRGB(0, 255, 0);
	FTImpl.Cmd_Text(230, 90, 28, FT_OPT_CENTER, "Fuel Pump");
	FTImpl.ColorRGB(255, 255, 255);

	if(diag_rst) FTImpl.ColorRGB(0, 255, 0);
	FTImpl.Cmd_Text(360, 90, 28, FT_OPT_CENTER, "Reset MCU");
	FTImpl.ColorRGB(255, 255, 255);


	common_display();
}

#endif // SCREEN_H
