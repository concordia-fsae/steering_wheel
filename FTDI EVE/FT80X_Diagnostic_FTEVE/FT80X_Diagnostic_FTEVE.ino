#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <FT_VM800B35.h>

FT800IMPL_SPI FTImpl(FT_CS_PIN,FT_PDN_PIN,FT_INT_PIN);
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
	FTImpl.DLStart();
	FTImpl.Cmd_Text(240, 32, 28, FT_OPT_CENTER, "Screen Diagnostics");
	
	
	FTImpl.Begin(FT_POINTS);
	FTImpl.PointSize(246);
	FTImpl.Tag(0);
	FTImpl.Vertex2ii(30, 150, 0, 0);
	FTImpl.Tag(1);
	FTImpl.Vertex2ii(30, 30, 0, 0);
	FTImpl.Tag(2);
	FTImpl.Vertex2ii(120, 245, 0, 0);
	FTImpl.Tag(3);
	FTImpl.Vertex2ii(180, 245, 0, 0);
	FTImpl.Tag(4);
	FTImpl.Vertex2ii(240, 245, 0, 0);
	FTImpl.Tag(5);
	FTImpl.Vertex2ii(300, 245, 0, 0);
	FTImpl.Tag(6);
	FTImpl.Vertex2ii(360, 245, 0, 0);
	FTImpl.Tag(7);
	FTImpl.Vertex2ii(450, 30, 0, 0);
	FTImpl.Tag(8);
	FTImpl.Vertex2ii(450, 150, 0, 0);
	FTImpl.Tag(9);
	FTImpl.PointSize(600);
	FTImpl.Vertex2ii(240, 145, 0, 0);
	FTImpl.End();
	
	FTImpl.Cmd_Text(240, 90, 28, FT_OPT_CENTER, "Shift Lights");
	FTImpl.Cmd_Text(65, 150, 28, FT_OPT_CENTER, "PL");
	FTImpl.Cmd_Text(65, 30, 28, FT_OPT_CENTER, "TR");
	FTImpl.Cmd_Text(120, 215, 28, FT_OPT_CENTER, "SW1");
	FTImpl.Cmd_Text(180, 215, 28, FT_OPT_CENTER, "SW2");
	FTImpl.Cmd_Text(240, 215, 28, FT_OPT_CENTER, "MID");
	FTImpl.Cmd_Text(300, 215, 28, FT_OPT_CENTER, "SW4");
	FTImpl.Cmd_Text(360, 215, 28, FT_OPT_CENTER, "SW5");
	FTImpl.Cmd_Text(415, 30, 28, FT_OPT_CENTER, "TR");
	FTImpl.Cmd_Text(415, 150, 28, FT_OPT_CENTER, "PR");
	FTImpl.DLEnd();
	FTImpl.Finish();
}

/* end of file */
