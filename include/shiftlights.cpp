// Shift Lights

#include "shiftlights.h"


ShiftLights::ShiftLights(uint8_t pin, uint8_t LEDCount, uint8_t brightness){
	this->pin_ = pin;
	this->LEDCount_ = LEDCount;
	this->brightness_ = brightness;

	shift_ = Adafruit_NeoPixel(LEDCount_, pin_, NEO_RGB + NEO_KHZ800);

	shift_.begin();
	shift_.setBrightness(brightness_);
	shift_.clear();
	shift_.show();
}

void ShiftLights::SetAllColor(uint32_t color){
	for(int i=0; i<LEDCount_; i++){
		shift_.setPixelColor(i, color);
	}
	shift_.show();
}

void ShiftLights::Clear(){
	shift_.clear();
	shift_.show();
}

void ShiftLights::LedStartup(){
	// generates a "sliding light" effect
	// slides two illuminated LEDs all the way to the right
	// and then all the way back down to the left
	// then blinks the whole thing white, and shuts off

	for(int j=0; j<LEDCount_+2; j++){
		if(j>=2){
			shift_.setPixelColor(j-2, 0x0);
		}

		if(j < LEDCount_){
			shift_.setPixelColor(j, 0x00FF00);
		}

		shift_.show();
		delay(10);
	}

	for(int k=LEDCount_+1; k>0; k--){
		int j = k-2;
		if(j <= LEDCount_-3){
			shift_.setPixelColor(j+2, 0x0);
		}

		if(j >= 0){
			shift_.setPixelColor(j, 0x00FF00);
		}


		shift_.show();
		delay(10);
	}

	for(int i=0; i<LEDCount_; i++){
		shift_.setPixelColor(i, 0xFFFFFF);
	}

	shift_.show();
	delay(50);
	shift_.clear();
	shift_.show();
}

// controls shift lights for each gear and RPM range
void ShiftLights::Update(uint8_t gear, uint16_t rpm){
	float seg {0};				// segment size (i.e. how many RPM does each shift light represent?)
	int num {0};				// number of lights that should be on
	int segs [2] {5, 9};		// how to divide up the shift lights (group of 5, group of 5, group of 6)
	uint32_t colors [3] = {		// choose the colors for each segment
		0xFF0000,
		0x00FF00,
		0x0000FF
	};

	shift_.clear();				// turn off all lights first
	shift_.show();

	// if the gear is Neutral or Unknown (default), use neutral range
	gear = (gear <= 0) ? 0 : gear;
	// calculate how many rpm each led represents
	seg = ranges_[gear][1] - ranges_[gear][0];
	// calculate how many leds should be lit based on current rpm
	num = 0;	// don't illuminate any if below min threshold
	if(rpm >= ranges_[gear][0]){
		num = LEDCount_ * (rpm - ranges_[gear][0])/seg;
	}

	// set the color of each led based on the segments defined above
	for(uint8_t i=0; i<num; i++){
		uint8_t curr_seg = (uint8_t) (i > segs[0]) + (i > segs[1]);

		shift_.setPixelColor(i, colors[curr_seg]);
		shift_.show();
	}
}
