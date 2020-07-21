#ifndef SCREEN_H
#define SCREEN_H

#include <inttypes.h>
#include <FT_NHD_43RTP_SHIELD.h>

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


#endif // SCREEN_H
