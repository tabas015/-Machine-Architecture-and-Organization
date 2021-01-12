// thermo_sim.c: DO NOT MODIFY
//
// Thermometer simulator main program and supporting functions. Read
// temperatuer sensor value and mode (C / F) from the command line and
// show the results of running functions from thermo_update.c on the
// screen. 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thermo.h"

#define CELSIUS_FLAG    0
#define FAHRENHEIT_FLAG 1

int main(int argc, char **argv){

  if(argc < 3){                 // Set TIME_OF_DAY_SEC to the actual time
    printf("usage: %s {sensor_val} {C | F}\n",argv[0]);
    printf("  sensor_val: positive integer\n");
    return 0;
  }
  THERMO_SENSOR_PORT = atoi(argv[1]);
  printf("THERMO_SENSOR_PORT set to: %u\n", THERMO_SENSOR_PORT);

  unsigned int temp_max = 1000 * 128;
  if(THERMO_SENSOR_PORT > temp_max){
    printf("Sensor value %u exceeds max %u\n",THERMO_SENSOR_PORT,temp_max);
    return 1;
  }

  if(argv[2][0]=='C' || argv[2][0]=='c'){
    THERMO_STATUS_PORT |= CELSIUS_FLAG;
  }
  else if(argv[2][0]=='F' || argv[2][0]=='f'){
    THERMO_STATUS_PORT |= FAHRENHEIT_FLAG;
  }
  else{
    printf("Unknown display mode: '%s'\n",argv[2]);
    printf("Should be 'C' or 'F'\n");
    return 1;
  }

  temp_t temp = {};
  int result = set_temp_from_ports(&temp);
  printf("set_temp_from_sensors(&temp );\n");

  printf("temp is {\n"); 
  printf("  .tenths_degrees = %d\n",temp.tenths_degrees);
  printf("  .is_fahrenheit  = %d\n",temp.is_fahrenheit);
  printf("}\n");

  int quo = temp.tenths_degrees / 10;
  int rem = temp.tenths_degrees % 10;
  if(rem < 0){
    rem *= -1;
  }
  char *sym = temp.is_fahrenheit ? "deg F" : "deg C";
  printf("Simulated temp is: %d.%d %s\n",quo,rem,sym);

  if(result != 0){
    printf("set_temp_from_ports() returned non-zero: %d\n",result);
    return 1;
  }

  printf("\nChecking results for display bits\n");
  
  int display = 0;
  result = set_display_from_temp(temp, &display);
  printf("set_display_from_temp(temp, &display);\n");

  printf("\ndisplay is:\n");
  printf("        3         2         1         0\n");
  printf("index: 10987654321098765432109876543210\n");
  printf("bits:  %s\n", asbits(display, INT_BITS));
  printf("guide:  |    |    |    |    |    |    |\n");
  printf("index:  30        20        10        0\n");

  if(result != 0){
    printf("set_display_from_temp() returned non-zero: %d\n",result);
    return 1;
  }

  printf("\nRunning thermo_update()\n");

  THERMO_DISPLAY_PORT = -1;     // set all bits to ensure they are cleared by the thermo_update() call
  thermo_update();

  printf("\nTHERMO_DISPLAY_PORT is:\n");
  printf("index:  3         2         1         0\n");
  printf("index: 10987654321098765432109876543210\n");
  printf("bits:  %s\n", asbits(THERMO_DISPLAY_PORT, INT_BITS));
  printf("guide:  |    |    |    |    |    |    |\n");
  printf("index:  30        20        10        0\n");


  printf("\nThermometer Display:\n");
  print_thermo_display();

  return 0;
}
