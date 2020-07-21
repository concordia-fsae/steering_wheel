#include "main.h"

ShiftLights shift_lights = ShiftLights(kSLPin, kSLCount, kSLBrightness);

void setup()
{

//	Serial.begin(115200);
/*
	while(Serial.read() <= 0);
	Serial.println("Serial connected.");

	while(!(CAN_OK == CAN.begin(CAN_1000KBPS, PB12))){
		Serial.print("MCP2515 Connection Failed, retrying ");
		Serial.println(CAN.begin(CAN_1000KBPS, PB12));
		delay(100);
	}
	Serial.println("MCP2515 Connection Successfull!");
*/

	uint8_t inputs [13] = {PB3, PB4, PB6, PB8, PA8, PA9, PA10, PA15, PB1, PA7, PA6, PA5, PA4};

	for(auto &input : inputs){
		pinMode(input, INPUT_PULLUP);
	}

	pinMode(RST, OUTPUT);
	digitalWrite(RST, HIGH);
	

	boot_display();
	shift_lights.LedStartup();

/* Uncomment when calibrating touch screen
	for(int i=0; i<6; i++)
	{
		Serial.print(touch_matrix[i]); Serial.print(" ");
	}
	Serial.println();
*/

	if(!(CAN_OK == CAN.begin(CAN_1000KBPS, MCP_CS))){
		warning = 0;
	}

	CAN.init_Filt(0, 0, recv_id);

	voltage = 0;
	o_temp = 0;
	o_press = 0;
	c_temp = 0;
	f_press = 0;
	f_temp = 0;
	gear = 0;
	rpm = 0;

}


bool check_display(){
	uint32_t chipid;
	chipid = FTImpl.Read32(FT_ROM_CHIPID);
	/* Identify the chip */
	if(FT800_CHIPID != chipid) return 0;

	return 1;
}


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


void loop(){
	digitalWrite(RST, !diag_rst);
	c_time = millis();

	if(c_time - l_time_50 > fifty_hz){		// send/recv data from Motec at 50 Hz
		l_time_50 = c_time;
		// code for sending to Motec over CAN
		get_inputs();
		buff[0] = status1;
		buff[1] = status2;
		buff[2] = launch_th_speed;
		buff[3] = launch_rpm;
		CAN.sendMsgBuf(transmit_ID, 0, 4, buff);
		c_time = millis();

		// check for CAN Comms error and process any waiting CAN messages
		if(CAN_MSGAVAIL == CAN.checkReceive()){
			MCP2515_ISR();
			last_can_update = c_time;
			warning = -1;
		}
		else if((c_time - last_can_update > can_timeout)){
			warning = 0;
		}

		v_int = voltage;
		v_fl = (float)((voltage - v_int)*10);

		shift_lights.Update(gear, rpm);
		c_time = millis();
	}

	if(c_time - l_time_10 > ten_hz){
		diag_shift = 0;
		FTImpl.GetTagXY(sTagxy);
		int tag = sTagxy.tag;

		int colors [2] = {0x0, 0x00FF00};
		switch(tag){
		case 1:
		// case 1, shift light diagnostics
			diag_shift = 1;

			shift_lights.SetAllColor(colors[diag_shift]);
			break;
		case 2:
		// case 2, turn on fuel pump
			if(!diag_fuel_deb){
				diag_fuel_deb = c_time;
			}
			else if(c_time - diag_fuel_deb > 250){
				diag_fuel_deb = 0;
				diag_fuel_st = !diag_fuel_st;
			}
			break;
		case 3:
		// case 3, reset MCU
			FTImpl.Clear(1,1,1);
			diag_rst = 1;
			break;
		case 4:
			if(!launch_rpm_minus_deb){
				launch_rpm_minus_deb = c_time;
			}
			else if(c_time - launch_rpm_minus_deb > 50){
				launch_rpm_minus_deb = 0;
				if(launch_rpm > 20){launch_rpm -= 2;}
			}
			break;
		case 5:
			if(!launch_rpm_plus_deb){
				launch_rpm_plus_deb = c_time;
			}
			else if(c_time - launch_rpm_plus_deb > 50){
				launch_rpm_plus_deb = 0;
				if(launch_rpm < 80){launch_rpm += 2;}
			}
			break;
		case 6:
			if(!launch_thresh_minus_deb){
				launch_thresh_minus_deb = c_time;
			}
			else if(c_time - launch_thresh_minus_deb > 50){
				launch_thresh_minus_deb = 0;
				if(launch_th_speed > 15){launch_th_speed -= 5;}
			}
			break;
		case 7:
			if(!launch_thresh_plus_deb){
				launch_thresh_plus_deb = c_time;
			}
			else if(c_time - launch_thresh_plus_deb > 50){
				launch_thresh_plus_deb = 0;
				if(launch_th_speed < 60){launch_th_speed += 5;}
			}
			break;
		default:
			break;
		}
	}

	if(check_display()){
		FTImpl.DLStart();

		if(sw3){
			diag_display();
		} else if(sw1){
			launch_display();
		} else {
			main_display();
		}

		if((warning != -1) && !(dismissed & (warning + 1)) && !sw3){
			error_overlay();
		}

		FTImpl.DLEnd();
		FTImpl.Finish();
	} else {
		FTImpl.DisplayOff();
		FTImpl.Exit();
	}
}

void get_inputs(){
	c_time = millis();

	sw1 	=	!digitalRead(PB8);
	sw2 	=	!digitalRead(PB6);
	sw3 	=	!digitalRead(PB4);
	sw4 	=	!digitalRead(PB3);
	tl 		=	!digitalRead(PA10);

	if(!digitalRead(PA15)){
		remote_start = tl;
		tr_deb = (!tr_deb) ? c_time : tr_deb;
		tr_deb = (c_time - tr_deb > 500) ? (tr = !tr) && 0 : tr_deb;
	}
	else{
		remote_start = 0;
		tr_deb = 0;
	}


	pl 		=	!digitalRead(PA8);
	pr 		=	!digitalRead(PA9);
	mid		=	!digitalRead(PB1);

	if((tl || tr) && warning != -1){
		if(!(dismissed & (warning+1))){
			dismissed = dismissed | (warning+1);
			warning = -1;
			tl = 0;
			tr = 0;
		}
	}

	dismissed = mid ? 0 : dismissed;

	status1 = 0;
	status2 = 0;

	status1 = sw1 ? status1 | sw1_st : status1;
	status1 = sw2 ? status1 | sw2_st : status1;
	status1 = sw3 ? status1 | sw3_st : status1;
	status1 = sw4 ? status1 | sw4_st : status1;
	status1 = tr ? status1 | tl_st : status1;
	status1 = tl ? status1 | tr_st : status1;
	status1 = pr ? status1 | pl_st : status1;
	status1 = pl ? status1 | pr_st : status1;

	status2 = mid ? status2 | mid_st : status2;
	status2 = diag_fuel_st ? status2 | fuel_pump : status2;
	status2 = remote_start ? status2 | remote_start_st : status2;
}


void MCP2515_ISR(){
	CAN.readMsgBuf(&recv_len, recv_msg);

	if(CAN.getCanId() == recv_id){
		gear = ((recv_msg[0] & 0b11110000) >> 4)-2;
		launch = (recv_msg[0] & 0b00001000) >> 3;
		traction = (recv_msg[0] & 0b00000100) >> 2;
		autoshift = (recv_msg[1] & 0b11110000) >> 4;
		rpm = (recv_msg[2] * 60);
		voltage = (recv_msg[3] / 10.0);
		f_press = (recv_msg[4] * 4);
		o_temp = (recv_msg[5] * 4);
		c_temp = (recv_msg[6] * 4);
		o_press = (recv_msg[7] * 4);
	} else if(CAN.getCanId() == recv_id + 2) {
		f_temp = (recv_msg[0] * 4);
	}
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
	col = (voltage > 12) ? 0x00FF00 : 0x0000FF;
	col = (voltage <= 9) ? 0xFF0000 : col;

	x = 45;
	y = 105;
	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(x, y-25, 26, FT_OPT_CENTER, "B_Volt");
	FTImpl.ColorRGB(col);
	make_pill(x, y);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Number(x-15, y, 28, FT_OPT_CENTER, v_int);
	FTImpl.Cmd_Text(x, y, 28, FT_OPT_CENTER, ".");
	FTImpl.Cmd_Number(x+7, y, 28, FT_OPT_CENTER, v_fl);
	FTImpl.Cmd_Text(x+27, y, 28, FT_OPT_CENTER, "V");


	// Top Right Pill (Coolant Temp)
	col = (c_temp > 100) ? 0xFF0000 : 0x00FF00;
	col = (c_temp <= 80) ? 0x0000FF : col;
	col = (c_temp <= 40) ? 0xFF0000 : col;

	x = 435;
	y = 105;

	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(x, y-25, 26, FT_OPT_CENTER, "C_Temp");
	FTImpl.ColorRGB(col);
	make_pill(x, y);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Number(x-15, y, 28, FT_OPT_SIGNED | FT_OPT_CENTER, c_temp);
	FTImpl.Cmd_Text(x+27, y, 28, FT_OPT_CENTER, "C");
	FTImpl.Cmd_Text(x+17, y-10, 26, FT_OPT_CENTER, "o");


	// Middle Left Pill (Oil Pressure)
	x = 45;
	y = 170;
	col = (o_press > 380) ? 0x00FF00 : 0x0000FF;
	col = (o_press <= 100) ? 0xFF0000 : col;

	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(x, y-25, 26, FT_OPT_CENTER, "O_Press");
	FTImpl.ColorRGB(col);
	make_pill(x, y);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Number(x-15, y, 28, FT_OPT_SIGNED | FT_OPT_CENTER, o_press);
	FTImpl.Cmd_Text(x+27, y, 28, FT_OPT_CENTER, "C");
	FTImpl.Cmd_Text(x+17, y-8, 26, FT_OPT_CENTER, "o");


	// Middle Right Pill (Fuel Pressure)
	col = (f_press > 380) ? 0x00FF00 : 0x0000FF;
	col = (f_press <= 100) ? 0xFF0000 : col;

	x = 435;
	y = 170;
	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(x, y-25, 26, FT_OPT_CENTER, "F_Press");
	FTImpl.ColorRGB(col);
	make_pill(x, y);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Number(x-21, y, 28, FT_OPT_SIGNED | FT_OPT_CENTER, f_press);
	FTImpl.Cmd_Text(x+18, y, 28, FT_OPT_CENTER, "kPa");


	// Bottom Left Pill (Oil Temp)
	col = (o_temp > 115) ? 0xFF0000 : 0x00FF00;
	col = (o_temp <= 80) ? 0x0000FF : col;
	col = (o_temp <= 40) ? 0xFF0000 : col;

	x = 45;
	y = 235;

	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(x, y-25, 26, FT_OPT_CENTER, "O_Temp");
	FTImpl.ColorRGB(col);
	make_pill(x, y);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Number(x-15, y, 28, FT_OPT_SIGNED | FT_OPT_CENTER, o_temp);
	FTImpl.Cmd_Text(x+27, y, 28, FT_OPT_CENTER, "C");
	FTImpl.Cmd_Text(x+17, y-8, 26, FT_OPT_CENTER, "o");


	// Bottom Right Pill (Fuel Temp)
	x = 435;
	y = 235;
	col = (f_temp > 380) ? 0x00FF00 : 0x0000FF;
	col = (f_temp <= 100) ? 0xFF0000 : col;

	FTImpl.ColorRGB(0xFFFFFF);
	FTImpl.Cmd_Text(x, y-25, 26, FT_OPT_CENTER, "F_Temp");
	FTImpl.ColorRGB(col);
	make_pill(x, y);
	FTImpl.ColorRGB(0, 0, 0);
	FTImpl.Cmd_Number(x-15, y, 28, FT_OPT_SIGNED | FT_OPT_CENTER, f_temp);
	FTImpl.Cmd_Text(x+27, y, 28, FT_OPT_CENTER, "C");
	FTImpl.Cmd_Text(x+17, y-8, 26, FT_OPT_CENTER, "o");


	FTImpl.ColorRGB(255, 255, 255);
	FTImpl.Cmd_Number(240, 45, 0, FT_OPT_CENTER, rpm);

	const char * gears [6]= {"N", "1", "2", "3", "4", "5"};

	if(gear > -1 && gear < 7){
		FTImpl.Cmd_Text(240, 156, 0, FT_OPT_CENTER, gears[gear]);
	} else {
		FTImpl.Cmd_Text(240, 156, 0, FT_OPT_CENTER, "?");
	}

	common_display();
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
	FTImpl.Cmd_Number(400, 55, 30, FT_OPT_CENTER, launch_rpm*100);
	FTImpl.Tag(4);
	FTImpl.Cmd_Button(340, 70, 48, 48, 27, 0, "-");
	FTImpl.Tag(5);
	FTImpl.Cmd_Button(410, 70, 48, 48, 27, 0, "+");

	FTImpl.Cmd_Text(355, 133, 23, FT_OPT_CENTER, "Thresh:");
	FTImpl.Cmd_Number(400, 155, 30, FT_OPT_CENTER, launch_th_speed);
	FTImpl.Tag(6);
	FTImpl.Cmd_Button(335, 170, 48, 48, 27, 0, "-");
	FTImpl.Tag(7);
	FTImpl.Cmd_Button(414, 170, 48, 48, 27, 0, "+");
	FTImpl.DLEnd();


	common_display();
}


void diag_display(){
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


	FTImpl.DisplayOn();
	FTImpl.AudioOff();


	FTImpl.Write32(REG_TOUCH_TRANSFORM_A, touch_matrix[0]);
	FTImpl.Write32(REG_TOUCH_TRANSFORM_B, touch_matrix[1]);
	FTImpl.Write32(REG_TOUCH_TRANSFORM_C, touch_matrix[2]);
	FTImpl.Write32(REG_TOUCH_TRANSFORM_D, touch_matrix[3]);
	FTImpl.Write32(REG_TOUCH_TRANSFORM_E, touch_matrix[4]);
	FTImpl.Write32(REG_TOUCH_TRANSFORM_F, touch_matrix[5]);
	FTImpl.Cmd_SetMatrix();

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
