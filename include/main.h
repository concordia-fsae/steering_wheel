#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include "shiftlights.h"
#include "canbus.h"
#include "screen.h"
#include "defs.h"

bool check_display();
void calibrate_display();
void get_inputs();
void MCP2515_ISR();
void error_overlay();
void main_display();
void launch_display();
void diag_display();
bool boot_display();

#endif // MAIN_H
