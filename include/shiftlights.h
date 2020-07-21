#ifndef SHIFTLIGHTS_H
#define SHIFTLIGHTS_H

#include <Adafruit_NeoPixel_stm.h>
#include <Adafruit_GFX.h>

// shift light defs
#define kSLPin PB10
#define kSLCount 16
#define kSLBrightness 10


class ShiftLights {
public:
	ShiftLights(uint8_t, uint8_t, uint8_t);
	void SetAllColor(uint32_t);
	void Clear();
	void LedStartup();
	void Update(uint8_t, uint16_t);

private:
	uint8_t pin_;
	uint8_t LEDCount_;
	uint8_t brightness_;
	Adafruit_NeoPixel shift_;
	uint16_t ranges_ [6][2] = {
		{0, 12500},			// neutral
		{5000, 12500},		// first gear
		{5000, 11500},		// second gear
		{5000, 11000},		// third gear
		{5000, 12500},		// fourth gear
		{5000, 12500},		// fifth gear
	};
};
#endif // SHIFTLIGHTS_H
