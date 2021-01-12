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


// Lowest order bit indicates whether display should be in Celsius (0)
// or Fahrenheit (1). Readable and Writeable.
extern unsigned char THERMO_STATUS_PORT;

// Controls thermometer display. Readable and writable.
extern int THERMO_DISPLAY_PORT;

// Show the thermometer display as ACII on the screen
void print_thermo_display();

// utility to format an in integer as a string of bits
char *asbits(int x, int bits);

#endif
