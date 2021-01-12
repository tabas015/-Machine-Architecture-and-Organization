#ifndef THERMO_H
#define THERMO_H 1

// size in bits of some types
#define INT_BITS   (sizeof(int)*8)
#define SHORT_BITS (sizeof(short)*8)
#define CHAR_BITS  (sizeof(char)*8)

////////////////////////////////////////////////////////////////////////////////
// thermostat structs/functions

// Breaks temperature down into constituent parts
typedef struct{
  short tenths_degrees;         // actual temp in tenths of degrees
  char is_fahrenheit;           // 0 for celsius, 1 for fahrenheit
} temp_t;

// Functions to implement for thermometer problem
int set_temp_from_ports(temp_t *temp);
int set_display_from_temp(temp_t temp, int *display);
int thermo_update();

////////////////////////////////////////////////////////////////////////////////
// thermo_sim.c structs/functions; do not modify


// Set by the sensor to indicate temperature. Value is a positive int
// in units of 0.1 / 64 deg C above -50.0 deg C.
extern unsigned short THERMO_SENSOR_PORT;

// Set and cleared by a button press on the thermometer to indicate
// whether display should be in Celsius or Fahrenheit; a 0 value
// indicates Celsius while a nonzero value indicates Farenheit.
//Instead, you will need to access its value to 
//determine various aspects of the temperature. Note the 
//type which is unsigned short: only positive values are 
//present in it and the variable will have 16 bits. 
//the temperature sensor has 
//a limited range and uses units of 1/64th of a 
//tenth degree above -50.0 deg C. 
//This leads to the following equivalences.
/*ensor Value	Celsius	Notes
0	-50.0	Minimum sensor val, MINIMUM measurable temp
31	-50.0	Remainder rounded down
32	-49.9	Remainder rounded up
64	-49.9	Exactly one tenth degree above -50.0 deg C
128	-49.8	Exactly two tenths degree above -50.0 deg C
640	-49.0	Exactly ten tenths = one degree above -50.0 deg C
6400	-40.0	Ten degrees above -50.0 C
31360	-1.0	49.0 degrees above -50.0
31680	-0.5	49.5 degrees above -50.0
32000	0.0	50.0 degrees above -50.0
32064	0.1	50.1 degrees above -50.0
38400	10.0	60.0 degrees above -50.0
64000	50.0	100.0 degrees above -50.0, MAXIMUM measurable temp
64001	err	Sensor error
*/



// Lowest order bit indicates whether display should be in Celsius (0)
// or Fahrenheit (1). Readable and Writeable.
extern unsigned char THERMO_STATUS_PORT;

// Lowest order bit indicates whether display should be in Celsius (0)
// or Fahrenheit (1). Other bits of the variable should be ignored as
// they indicate status not relevant to the display. Use bit masking
// to extract the low order bit.
// Controls thermometer display. Readable and writable.

extern int THERMO_DISPLAY_PORT;

// Show the thermometer display as ACII on the screen
void print_thermo_display();

// utility to format an in integer as a string of bits
/*You may presume that the THERMO_DISPLAY_PORT is a 32-bit integer.
30 bits are used to control the full temperature display.
Bits 0-6 control the tenths place
Bits 7-13 control the ones place
Bits 14-20 control the tens place
Bits 21-27 control the hundreds place
Bit 28 controls whether degrees Celsius is shown
Bit 29 controls whether degrees Fahrenheit is shown
Bits 30 and 31 are not used and should always be 0.
*/
char *asbits(int x, int bits);

#endif



/*Bits that are set (equal to 1) will turn on (darken) one bar of the display digit
Bits that are clear (equal to 0) will turn off one bar of the digit
7 bits are required to control the display of one digit
The bits are arranged with the low order bit (bit 0) for the middle bar and remaining bits control bars starting from the top going around clockwise
Bit 0 middle
Bit 1 top
Bit 2 upper right
Bit 3 lower right
Bit 4 bottom
Bit 5 lower left
Bit 6 upper left
*/