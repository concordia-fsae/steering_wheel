#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include "FT_NHD_43RTP_SHIELD.h"

#define SCR_CS_PIN PA0
FT800IMPL_SPI FTImpl(SCR_CS_PIN,FT_PDN_PIN,FT_INT_PIN);

void setup()
{
	FTImpl.Init(FT_DISPLAY_RESOLUTION);
	FTImpl.SetDisplayEnablePin(FT_DISPENABLE_PIN);
	FTImpl.SetAudioEnablePin(FT_AUDIOENABLE_PIN);
	FTImpl.DisplayOn();
	FTImpl.AudioOn();

	FTImpl.DLStart();
	FTImpl.Finish();
	FTImpl.DLStart();
	FTImpl.DLEnd();
	FTImpl.Finish();
}

void loop()
{
	errorOverlay();
}

void errorOverlay()
{
	FTImpl.DLStart();
	
	
	FTImpl.Begin(FT_RECTS);
	
	FTImpl.ColorA(226);
	FTImpl.ColorRGB(71, 71, 71);
	FTImpl.Vertex2ii(0, 0, 0, 0);
	FTImpl.Vertex2ii(480, 272, 0, 0);
	
	
	FTImpl.ColorA(255);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Vertex2ii(50, 50, 0, 0);
	FTImpl.Vertex2ii(433, 222, 0, 0);
	FTImpl.End();
	
	FTImpl.ColorRGB(255, 0, 0);
	FTImpl.Cmd_Text(240, 136, 30, FT_OPT_CENTER, "Warning Source");
	
	FTImpl.DLEnd();
	FTImpl.Finish();
}

/* end of file */
