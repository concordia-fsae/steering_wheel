#include "main.h"

ShiftLights shift_lights = ShiftLights(kSLPin, kSLCount, kSLBrightness);

void setup(){
	Serial.begin(115200);
	
/* DEBUG CODE
	// wait for computer to connect to serial before continuing
	while(Serial.read() <= 0);
	Serial.println("Serial connected.");

// DEBUG CAN CONNECTION
	// verify connection to can bus module
	while(!(CAN_OK == CAN.begin(CAN_1000KBPS, PB12))){
		Serial.print("MCP2515 Connection Failed, retrying ");
		Serial.println(CAN.begin(CAN_1000KBPS, PB12));
		delay(100);
	}
	Serial.println("MCP2515 Connection Successfull!");
*/
	// define all input pins
	uint8_t inputs [13] = {PB3, PB4, PB6, PB8, PA8, PA9, PA10, PA15, PB1, PA7, PA6, PA5, PA4};

	// initialize all pins to input pullup
	for(auto &input : inputs){
		pinMode(input, INPUT_PULLUP);
	}

	// initnialize reset pin and default to high so we don't get stuck in a reset loop
	pinMode(RST, OUTPUT);
	digitalWrite(RST, HIGH);
	
	// start up the screen
	boot_display();

	// run the shift light startup sequence
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

	/*
	 * At this point we know that all systems have booted and everything is good to go
	*/

	// not sure if this even does anything
	CAN.init_Filt(0, 0, recv_id);

	// might not be necessary, might be able to save memory by not defining globally
	voltage = 0;
	o_temp = 0;
	o_press = 0;
	c_temp = 0;
	f_press = 0;
	f_temp = 0;
	gear = 0;
	rpm = 0;
}

void loop(){
	digitalWrite(RST, !diag_rst);
	c_time = millis();

	if(c_time - l_time_50 >= fifty_hz){		// send/recv data from Motec at 50 Hz
		l_time_50 = c_time;
		// code for sending to Motec over CAN
		uint8_t status1, status2;
		get_inputs(&status1, &status2);
		uint8_t buff[4] = {status1, status2, launch_th_speed, launch_rpm};
		CAN.sendMsgBuf(transmit_ID, 0, 4, buff);
		c_time = millis();

		// check for CAN Comms error and process any waiting CAN messages
		if(CAN_MSGAVAIL == CAN.checkReceive()){
			can_receive();
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
			diag_display(diag_shift, diag_fuel_st, diag_rst);
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

void get_inputs(uint8_t *status1, uint8_t *status2){
	c_time = millis();

	mid		=	!digitalRead(PB1);
	sw1 	=	!digitalRead(PB8);
	sw2 	=	!digitalRead(PB6);
	sw3 	=	!digitalRead(PB4);
	sw4 	=	!digitalRead(PB3);
	pl 		=	!digitalRead(PA8);
	pr 		=	!digitalRead(PA9);
	tl 		=	!digitalRead(PA10);
	tr		=	!digitalRead(PA15);
	

	if(tl && tr){
		tr_deb = (!tr_deb) ? c_time : tr_deb;
		if(c_time - tr_deb > 500){
			
			tr_deb = 0;
			remote_start = true;
		}
		tr = tl = false;
		
	} else {
		remote_start = 0;
		tr_deb = 0;
	}

	// replaced by block above. I can't believe I left this in production code
	// tr_deb = (c_time - tr_deb > 500) ? (tr = !tr) && 0 : tr_deb;


	if((tl || tr) && warning != -1){
		if(!(dismissed & (warning+1))){
			dismissed = dismissed | (warning+1);
			warning = -1;
			tl = 0;
			tr = 0;
		}
	}

	dismissed = mid ? 0 : dismissed;

	*status1 = 0;
	*status2 = 0;

	*status1 = sw1 ? *status1 | sw1_st : *status1;
	*status1 = sw2 ? *status1 | sw2_st : *status1;
	*status1 = sw3 ? *status1 | sw3_st : *status1;
	*status1 = sw4 ? *status1 | sw4_st : *status1;
	*status1 = tr ? *status1 | tl_st : *status1;
	*status1 = tl ? *status1 | tr_st : *status1;
	*status1 = pr ? *status1 | pl_st : *status1;
	*status1 = pl ? *status1 | pr_st : *status1;

	*status2 = mid ? *status2 | mid_st : *status2;
	*status2 = diag_fuel_st ? *status2 | fuel_pump : *status2;
	*status2 = remote_start ? *status2 | remote_start_st : *status2;
}