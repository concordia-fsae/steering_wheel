#include "main.h"

ShiftLights shift_lights = ShiftLights(kSLPin, kSLCount, kSLBrightness);
SPIClass SPI_2(2);

// define global time tracker
uint32_t c_time = millis();

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
// */
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
		warning.current = 0;
	}

	/*
	 * At this point we know that all systems have booted and everything is good to go
	*/

	// not sure if this even does anything
	CAN.init_Filt(0, 0, recv_id);
}

void loop(){
	digitalWrite(RST, !diag_rst);
	c_time = millis();

	if(c_time - l_time_50 >= fifty_hz){		// send/recv data from Motec at 50 Hz
		l_time_50 = c_time;
		// code for sending to Motec over CAN
		uint8_t status1, status2;
		get_inputs(status1, status2);
		uint8_t buff[4] = {status1, status2, veh.launch_cnf.th_speed, veh.launch_cnf.rpm};
		CAN.sendMsgBuf(transmit_ID, 0, 4, buff);
		c_time = millis();

		// check for CAN Comms error and process any waiting CAN messages
		if(CAN_MSGAVAIL == CAN.checkReceive()){
			can_receive();
			last_can_update = c_time;
			warning.current = -1;
		}
		else if((c_time - last_can_update > can_timeout)){
			warning.current = 0;
		}

		shift_lights.Update(veh.gear, veh.rpm);
		c_time = millis();
	}

	if(c_time - l_time_10 > ten_hz){
		diag_shift = 0;
		uint16_t ts_btn = ts_btn_pressed();

		int colors [2] = {0x0, 0x00FF00};
		switch(ts_btn){
		case 0:
			diag_fuel_deb.Reset();
			launch_rpm_minus.Reset();
			launch_rpm_plus.Reset();
			launch_thresh_minus.Reset();
			launch_thresh_plus.Reset();
		// no 0th button
			break;
		case 1:
		// case 1, shift light diagnostics
			diag_shift = 1;
			shift_lights.SetAllColor(colors[diag_shift]);
			break;
		case 2:
		// case 2, turn on fuel pump
			if(diag_fuel_deb.Deb()){
				diag_fuel_st = !diag_fuel_st;
			}
			break;
		case 3:
		// case 3, reset MCU
			FTImpl.Clear(1,1,1);
			diag_rst = 1;
			break;
		case 4:
			if(launch_rpm_minus.Deb()){
				if(veh.launch_cnf.rpm > 20){veh.launch_cnf.rpm -= 2;}
			}
			break;
		case 5:
			if(launch_rpm_plus.Deb()){
				if(veh.launch_cnf.rpm < 80){veh.launch_cnf.rpm += 2;}
			}
			break;
		case 6:
			if(launch_thresh_minus.Deb()){
				if(veh.launch_cnf.th_speed > 15){veh.launch_cnf.th_speed -= 5;}
			}
			break;
		case 7:
			if(launch_thresh_plus.Deb()){
				if(veh.launch_cnf.th_speed < 60){veh.launch_cnf.th_speed += 5;}
			}
			break;
		default:
			break;
		}
	}

	if(check_display()){
		FTImpl.DLStart();

		if(veh.io.sw3){
			diag_display(diag_shift, diag_fuel_st, diag_rst);
		} else if(veh.io.sw1){
			launch_display();
		} else {
			main_display();
		}

		if((warning.current != -1) && !(warning.dismissed & (warning.current + 1)) && !veh.io.sw3){
			error_overlay();
		}

		FTImpl.DLEnd();
		FTImpl.Finish();
	} else {
		FTImpl.DisplayOff();
		FTImpl.Exit();
	}
}

void get_inputs(uint8_t &status1, uint8_t &status2){
	c_time = millis();

	uint32_t portb = GPIOB->regs->IDR;
	uint32_t porta = GPIOA->regs->IDR;

	veh.io.mid		=	!(portb & 0b10);
	veh.io.sw1		=	!(portb & 0b100000000);
	veh.io.sw2		=	!(portb & 0b1000000);
	veh.io.sw3		=	!(portb & 0b10000);
	veh.io.sw4		=	!(portb & 0b1000);
	veh.io.pl		=	!(porta & 0b100000000);
	veh.io.pr		=	!(porta & 0b1000000000);
	veh.io.tl		=	!(porta & 0b10000000000);
	veh.io.tr		=	!(porta & 0b1000000000000000);


	if(veh.io.tl && veh.io.tr){
		// set the remote start bit to true if both the top buttons have
		// been held for more than 500 ms, otherwise set it to false
		// reset when one of the buttons is released
		veh.io.remote_start = remote_start.Deb();
		veh.io.tr = veh.io.tl = false;
	} else {
		veh.io.remote_start = false;
		remote_start.Reset();
	}

	// dismiss the current warning if either tl or tr is pressed, intercept their respective functions
	if((veh.io.tl || veh.io.tr) && warning.current != -1){
		if(!(warning.dismissed & (warning.current+1))){
			warning.dismissed = warning.dismissed | (warning.current+1);
			warning.current = -1;
			veh.io.tl = 0;
			veh.io.tr = 0;
		}
	}

	// un-dismiss the warnings if the middle button is pressed
	warning.dismissed = veh.io.mid ? 0 : warning.dismissed;

	// create the status bitfields
	status1 = 0;
	status2 = 0;

	status1 = status1
					| veh.io.sw1 
					| veh.io.sw2 << 1 
					| veh.io.sw3 << 2 
					| veh.io.sw4 << 3 
					| veh.io.tr << 4 
					| veh.io.tl << 5 
					| veh.io.pr << 6 
					| veh.io.pl << 7;

	status2 = status2
					| veh.io.mid
					| diag_fuel_st << 1
					| veh.io.remote_start << 2;
}
