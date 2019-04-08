#include <EEPROM.h>
#include <SPI.h>
#include <mcp_can_stm.h>
#include <Wire.h>
#include "FT_NHD_43RTP_SHIELD.h"
#include "Adafruit_NeoPixel_stm.h"
#include "Adafruit_GFX.h"

// shift light defs
#define SHIFT_PIN PB10
#define NEOS 16
#define SHIFTBRIGHTNESS 10
Adafruit_NeoPixel shift = Adafruit_NeoPixel(NEOS, SHIFT_PIN, NEO_RGB + NEO_KHZ800);


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

// screen defs
#define MCP_CS PB12
#define MCP_INT PB11
#define SCR_CS_PIN PA0
#define RAM_FONTS_MICROSS 0
static PROGMEM prog_uchar FONTS_MICROSS[] = {
	#include "FONTS_MICROSS.h"
};

// define screen object
FT800IMPL_SPI FTImpl(SCR_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

// touchscreen objects
sTagXY sTagxy;
int tag;

// touchscreen calibration for orange wheel
uint32_t touch_matrix [6] = {32439, 4294966806, 4294236873, 4294966575, 4294947195, 18508876};

// CAN bus def
MCP_CAN CAN;


// pin that is connected to rst
#define RST PB0


float voltage;
int v_int, v_fl, o_temp, o_press, c_temp, f_press, f_temp, gear, rpm;
bool sw1, sw2, sw3, sw4, pl, pr, tl, tr, mid, launch, traction, autoshift, remote_start;


// define the RPM ranges for each gear
uint16_t neutral [2] {0,12500};
uint16_t first [2] {6000, 12500};
uint16_t second [2] {8000, 11500};
uint16_t third [2] {8000, 11000};
uint16_t fourth [2] {8000, 12500};


uint8_t launch_rpm = 6000/100;	// default value for launch rpm
uint8_t launch_th_speed = 30;	// default value for threshold speed
uint8_t status1 = 0;			// first variable containing button states
uint8_t status2 = 0;			// second variable containing button states
uint16_t transmit_ID = 0xA0;	// CAN Bus transmit ID (messages will be sent with this ID)
uint32_t recv_id = 0x80;		// CAN Bus receive ID (messages with this ID will be read)
uint8_t recv_len;				// length of received message
uint8_t recv_msg[9];			// the message (4 bytes)
uint8_t buff [7] = {status1, status2, launch_th_speed, launch_rpm};	// message to be sent, contains statuses


unsigned long int c_time = 0;
unsigned long int l_time_50 = 0;		// last time a 50Hz event occured
unsigned long int l_time_10 = 0;		// last time a 10Hz event occured
unsigned long int l_time_5 = 0;			// last time a 5Hz event occured
unsigned long int l_time_1 = 0;			// last time a 1Hz event occured


// constants for each refresh rate
int fifty_hz = 20;
int ten_hz = 100;
int five_hz = 200;
int one_hz = 1000;


unsigned long int last_can_update = 0;	// last time a CAN message was received
int can_timeout = 5000;					// time without CAN message before error occurs


// debounce vars
uint32_t diag_fuel_deb = 0;
uint32_t tr_deb = 0;		// debounce var for top right btn
uint32_t launch_rpm_plus_deb = 0;
uint32_t launch_rpm_minus_deb = 0;
uint32_t launch_thresh_plus_deb = 0;
uint32_t launch_thresh_minus_deb = 0;


// warning defs
int warning = -1;
char warnings [8][20] = {"No Conn.", "Low Oil Press.", "Low Fuel Press.", "Low Batt. Volt.", "High Cool. Temp.", "High Oil Temp",
						"Low AFR", "High AFR"};
int last_warn = -1;
uint8_t dismissed = 0;


// status vars
bool led_err_st = 0;
bool led_clear = 1;
bool diag_shift = 0;
bool diag_rst = 0;
bool diag_fuel_st = 0;


// ---------------------------------- Helper Functions ----------------------------------


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


// display elements that are shown on most pages
void common_display()
{
	FTImpl.PointSize(160);
	FTImpl.Begin(FT_POINTS);
	

	if(tl)
	{
		FTImpl.ColorRGB(0, 255, 0);
	}
	else
	{
		FTImpl.ColorRGB(255, 255, 255);
	}
	FTImpl.Vertex2ii(20, 35, 1, 0);
	
	
	if(tr)
	{
		FTImpl.ColorRGB(0, 255, 0);
	}
	else
	{
		FTImpl.ColorRGB(255, 255, 255);
	}
	FTImpl.Vertex2ii(445, 35, 1, 0);
	

	if(sw1)
	{
		FTImpl.ColorRGB(0, 255, 0);
	}
	else
	{
		FTImpl.ColorRGB(255, 255, 255);
	}
	FTImpl.Vertex2ii(150, 255, 1, 0);
	
	
	if(sw2)
	{
		FTImpl.ColorRGB(0, 255, 0);
	}
	else
	{
		FTImpl.ColorRGB(255, 255, 255);
	}
	FTImpl.Vertex2ii(210, 255, 1, 0);
	
	
	if(sw3)
	{
		FTImpl.ColorRGB(0, 255, 0);
	}
	else
	{
		FTImpl.ColorRGB(255, 255, 255);
	}
	FTImpl.Vertex2ii(270, 255, 1, 0);
	FTImpl.End();
	


	FTImpl.ColorRGB(255, 255, 255);
	FTImpl.Cmd_Text(20, 15, 21, FT_OPT_CENTER, "DRS");
	
	FTImpl.ColorRGB(255, 255, 255);
	FTImpl.Cmd_Text(445, 15, 21, FT_OPT_CENTER, "LC");
	


	FTImpl.ColorRGB(255, 255, 255);
	FTImpl.Cmd_Text(150, 235, 21, FT_OPT_CENTER, "LC");
	
	FTImpl.ColorRGB(255, 255, 255);
	FTImpl.Cmd_Text(210, 235, 21, FT_OPT_CENTER, "AS");
	
	FTImpl.ColorRGB(255, 255, 255);
	FTImpl.Cmd_Text(270, 237, 21, FT_OPT_CENTER, "CTRL");
	
	
	FTImpl.ColorRGB(255, 255, 255);
	FTImpl.Cmd_Text(330, 237, 21, FT_OPT_CENTER, "WC");

	if(!sw4)
	{
		FTImpl.Cmd_Text(330, 255, 28, FT_OPT_CENTER, "D");
	}
	else
	{
		FTImpl.Cmd_Text(330, 255, 28, FT_OPT_CENTER, "W");
	}
	
	if(remote_start)
	{
		FTImpl.ColorRGB(255, 0, 0);
		FTImpl.Cmd_Text(240, 208, 27, FT_OPT_CENTER, "IGN");
	}
}