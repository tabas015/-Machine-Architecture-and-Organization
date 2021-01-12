
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thermo.h"

#define CELSIUS_FLAG    0
#define FAHRENHEIT_FLAG 1

int arr[10]={0b1111110,0b0001100,0b0110111,0b0011111,0b1001101,0b1011011,0b1111011,0b0001110,0b1111111,0b1011111};
// array of bit masks

// Uses the two global variables (ports) THERMO_SENSOR_PORT and
// THERMO_STATUS_PORT to set the temp structure. If THERMO_SENSOR_PORT
// is above its maximum trusted value, associated with +50.0 deg C,
// does not alter temp and returns 1. Otherwise, sets fields of temp
// based on converting sensor value to degrees and checking whether
// Celsius or Fahrenheit display is in effect. Returns 0 on successful
// set. This function DOES NOT modify any global variables but may
// access global variables.

int set_temp_from_ports(temp_t *temp){
    int max_val = 64000;
   	if (THERMO_SENSOR_PORT> max_val){
        return 1;
    }
	else {
		short tempt = THERMO_SENSOR_PORT/64 - 500;
		int n = (THERMO_SENSOR_PORT % 64);
		if (n >= 32){
			tempt += 1; 
		}
		
		if (!(THERMO_STATUS_PORT & 0b00000001)){  //masking to check if it's equal to 0
			temp-> is_fahrenheit = 0;
			temp->tenths_degrees = tempt;
		}
		else{
			temp-> is_fahrenheit = 1;
			short ftemp = (tempt * 9) / 5 + 320;
			temp-> tenths_degrees = ftemp;
		}
		return 0;
	}
}


// Alters the bits of integer pointed to by display to reflect the
// temperature in struct arg temp. If temp has a temperature value
// that is below minimum or above maximum temperature allowable or if
// an improper indication of celsius/fahrenheit is given, does nothing
// and returns 1. Otherwise, calculates each digit of the temperature
// and changes bits at display to show the temperature according to
// the pattern for each digit. This function DOES NOT modify any
// global variables but may access global variables.

int set_display_from_temp(temp_t temp, int *display){
	//minimum celcius -500 and maximum celcius is +500
	//minimum fahrenheit -580 and maximum fahrenheit +1220 
	if( (temp.is_fahrenheit==0 && (temp.tenths_degrees < -500 ||
	temp.tenths_degrees > +500))
	|| (temp.is_fahrenheit==1 && (temp.tenths_degrees < -580 ||
	temp.tenths_degrees > +1220))
	|| (temp.is_fahrenheit!=0 && temp.is_fahrenheit!=1)){
		return 1;          // error conditions
	}

	int disp =0; // transfer this to the display
	int c = temp.tenths_degrees; // initialize with temp, use in repeated division
	if (c < 0){            //checking for negative celcius
		disp |= 0b0000001; // 0b0000001 = negative
		disp = disp << 7;	// shift
		c = -c;		// negate quotient so remaining values are all positive
	}
	int temp_tenths = c % 10; // tenths place
	c = c / 10;
	int temp_ones = c % 10; // ones place
	c = c / 10;
	int temp_tens = c % 10; // tens place
	c = c / 10;
	int temp_hundreds = c % 10; // hundreds place

	if(temp_hundreds != 0){ // blank for hundreds?
		disp |= arr[temp_hundreds]; // nope
		disp = disp << 7;
	}
	if(temp_hundreds !=0 || temp_tens !=0){ // blank for tens?
		disp |=arr[temp_tens];
		disp = disp << 7;
	}
	disp |=arr[temp_ones]; // ones digit always there
	disp = disp << 7;
	disp |= arr[temp_tenths]; // tenths digit always there

	if(temp.is_fahrenheit){ // set bits associated C/F
		disp |= (0x1 << 29); // position of deg F bit
	}
	else{
		disp |= (0x1 << 28); // position of deg C bit
	}

	*display = disp; // deref-set the pointed to integer
	return 0;
}

// Called to update the thermometer display. Makes use of
// set_temp_from_ports() and set_display_from_temp() to access
// temperature sensor then set the display. Checks these functions and
// if they indicate an error, makes not changes to the display.
// Otherwise modifies THERMO_DISPLAY_PORT to set the display.
//
// CONSTRAINT: Does not allocate any heap memory as malloc() is NOT
// available on the target microcontroller. Uses stack and global
// memory only.


int thermo_update(){
	temp_t temp = {};
	int ret;
	ret = set_temp_from_ports(&temp);
	if(ret!=0){
		return 1;
	}
	int disp = 0;
	ret = set_display_from_temp(temp,&disp);
	if(ret != 0){
		return 1;
	}
	THERMO_DISPLAY_PORT = disp;
	return 0;
}


