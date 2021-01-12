// test_thermo_update.c: testing program for functions in thermo_update.c

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "thermo.h"

#define PRINT_TEST sprintf(sysbuf,"awk 'NR==(%d+1){P=1;print \"{\"} P==1 && /ENDTEST/{P=0; print \"}\"} P==1{print}' %s", __LINE__, __FILE__); \
                   system(sysbuf);

void print_temp(temp_t temp){
  printf("{\n"); 
  printf("  .tenths_degrees = %d,\n",temp.tenths_degrees);
  printf("  .is_fahrenheit  = %d,\n",temp.is_fahrenheit);
  printf("}\n");
}


void print_ports(){
  printf("%-20s : %d\n","THERMO_SENSOR_PORT", THERMO_SENSOR_PORT);
  printf("%-20s : %s\n","THERMO_STATUS_PORT", asbits(THERMO_STATUS_PORT, CHAR_BITS));
  printf("%-20s : %s\n","THERMO_DISPLAY_PORT",asbits(THERMO_DISPLAY_PORT,INT_BITS));
}

int main(int argc, char **argv){
  // int i;
  if(argc < 2){
    printf("usage: %s <test_name>\n", argv[0]);
    return 1;
  }
  char *test_name = argv[1];
  char sysbuf[1024];

  // // malloc() some space to allow for easier debugging 
  // tempt_t *temp = malloc(sizeof(temp_t));
  // memset(temp, 0, sizeof(temp_t));
  // int *disp = malloc(sizeof(int));
  // *disp = 0;

  if(0){}

  else if( strcmp( test_name, "set_temp_from_ports() 0 C" )==0 ) {
    PRINT_TEST;
    // Check sensor value of 0 and status of all 0
    THERMO_SENSOR_PORT  = 0;
    THERMO_STATUS_PORT  = 0b0000;
    THERMO_DISPLAY_PORT = -1;
    temp_t *actual = calloc(1,sizeof(temp_t));
    int ret = set_temp_from_ports(actual);
    printf("ret: %d\n",ret);
    printf("actual = "); print_temp(*actual);
    print_ports();
    free(actual);
  } // ENDTEST

  else if( strcmp( test_name, "set_temp_from_ports() 0 F" )==0 ) {
    PRINT_TEST;
    // Check sensor value of 0 and status of 1 to turn to Farenheit
    THERMO_SENSOR_PORT  = 0;
    THERMO_STATUS_PORT  = 0b0001;
    THERMO_DISPLAY_PORT = -1;
    temp_t *actual = calloc(1,sizeof(temp_t));
    int ret = set_temp_from_ports(actual);
    printf("ret: %d\n",ret);
    printf("actual = "); print_temp(*actual);
    print_ports();
    free(actual);
  } // ENDTEST

  else if( strcmp( test_name, "set_temp_from_ports() 64 C" )==0 ) {
    PRINT_TEST;
    // Should result in -499 tenths degrees Celsius due to each 64
    // sensor ticks being a tenth degree Celsius
    THERMO_SENSOR_PORT  = 64;
    THERMO_STATUS_PORT  = 0b0000;
    THERMO_DISPLAY_PORT = -1;
    temp_t *actual = calloc(1,sizeof(temp_t));
    int ret = set_temp_from_ports(actual);
    printf("ret: %d\n",ret);
    printf("actual = "); print_temp(*actual);
    print_ports();
    free(actual);
  } // ENDTEST

  else if( strcmp( test_name, "set_temp_from_ports() 120 C" )==0 ) {
    PRINT_TEST;
    // Check that this rounds "up" towards 498 tenths degrees Celsius
    THERMO_SENSOR_PORT  = 120;
    THERMO_STATUS_PORT  = 0b0000;
    THERMO_DISPLAY_PORT = -1;
    temp_t *actual = calloc(1,sizeof(temp_t));
    int ret = set_temp_from_ports(actual);
    printf("ret: %d\n",ret);
    printf("actual = "); print_temp(*actual);
    print_ports();
    free(actual);
  } // ENDTEST

  else if( strcmp( test_name, "set_temp_from_ports() 32000 C" )==0 ) {
    PRINT_TEST;
    // Should be an even 0.0 degrees C as senor is 500 tenths degrees
    // above minimum value -500
    THERMO_SENSOR_PORT  = 32000;
    THERMO_STATUS_PORT  = 0b0000;
    THERMO_DISPLAY_PORT = -1;
    temp_t *actual = calloc(1,sizeof(temp_t));
    int ret = set_temp_from_ports(actual);
    printf("ret: %d\n",ret);
    printf("actual = "); print_temp(*actual);
    print_ports();
    free(actual);
  } // ENDTEST

  else if( strcmp( test_name, "set_temp_from_ports() 32000 F" )==0 ) {
    PRINT_TEST;
    // Should be 32 degrees fahrenheit: freezing
    THERMO_SENSOR_PORT  = 32000;
    THERMO_STATUS_PORT  = 0b0001;
    THERMO_DISPLAY_PORT = -1;
    temp_t *actual = calloc(1,sizeof(temp_t));
    int ret = set_temp_from_ports(actual);
    printf("ret: %d\n",ret);
    printf("actual = "); print_temp(*actual);
    print_ports();
    free(actual);
  } // ENDTEST


  else if( strcmp( test_name, "set_temp_from_ports() 18000 F" )==0 ) {
    PRINT_TEST;
    // Checks rounding/digit placement in Farenheit
    THERMO_SENSOR_PORT  = 18000;
    THERMO_STATUS_PORT  = 0b0001;
    THERMO_DISPLAY_PORT = -1;
    temp_t *actual = calloc(1,sizeof(temp_t));
    int ret = set_temp_from_ports(actual);
    printf("ret: %d\n",ret);
    printf("actual = "); print_temp(*actual);
    print_ports();
    free(actual);
  } // ENDTEST


  else if( strcmp( test_name, "set_temp_from_ports() 18105 C" )==0 ) {
    PRINT_TEST;
    // Checks rounding/digit placement in Celsius
    THERMO_SENSOR_PORT  = 18105;
    THERMO_STATUS_PORT  = 0b0000;
    THERMO_DISPLAY_PORT = -1;
    temp_t *actual = calloc(1,sizeof(temp_t));
    int ret = set_temp_from_ports(actual);
    printf("ret: %d\n",ret);
    printf("actual = "); print_temp(*actual);
    print_ports();
    free(actual);
  } // ENDTEST


  else if( strcmp( test_name, "set_temp_from_ports() 64000 F" )==0 ) {
    PRINT_TEST;
    // Max possible sensor value shown in Farenheit
    THERMO_SENSOR_PORT  = 64000;
    THERMO_STATUS_PORT  = 0b0000;
    THERMO_DISPLAY_PORT = -1;
    temp_t *actual = calloc(1,sizeof(temp_t));
    int ret = set_temp_from_ports(actual);
    printf("ret: %d\n",ret);
    printf("actual = "); print_temp(*actual);
    print_ports();
    free(actual);
  } // ENDTEST

  ////////////////////////////////////////////////////////////////////////////////
  else if( strcmp( test_name, "set_temp_from_ports() fail sensor val" )==0 ) {
    PRINT_TEST;
    // Sensor value above maximum allowed, fail returning 1
    THERMO_SENSOR_PORT  = 65431;
    THERMO_STATUS_PORT  = 0b0000;
    THERMO_DISPLAY_PORT = -1;
    temp_t *actual = calloc(1,sizeof(temp_t));
    int ret = set_temp_from_ports(actual);
    printf("ret: %d\n",ret);
    printf("actual = "); print_temp(*actual);
    print_ports();
    free(actual);
  } // ENDTEST


  ////////////////////////////////////////////////////////////////////////////////
  else if( strcmp( test_name, "set_temp_from_ports() C Ignore Status Bits" )==0 ) {
    PRINT_TEST;
    // Upper bits in THERMO_STATUS_PORT are set and they should be
    // IGNORED; do NOT change to Farenheit
    THERMO_SENSOR_PORT  = 18105;
    THERMO_STATUS_PORT  = 0b1010;
    THERMO_DISPLAY_PORT = -1;
    temp_t *actual = calloc(1,sizeof(temp_t));
    int ret = set_temp_from_ports(actual);
    printf("ret: %d\n",ret);
    printf("actual = "); print_temp(*actual);
    print_ports();
    free(actual);
  } // ENDTEST


  else if( strcmp( test_name, "set_temp_from_ports() F Ignore Status Bits" )==0 ) {
    PRINT_TEST;
    // All bits of status port are 1's but only the last bit should be
    // used to indicate Farenheit
    THERMO_SENSOR_PORT  = 18000;
    THERMO_STATUS_PORT  = -1;   // integer all 1's
    THERMO_DISPLAY_PORT = -1;
    temp_t *actual = calloc(1,sizeof(temp_t));
    int ret = set_temp_from_ports(actual);
    printf("ret: %d\n",ret);
    printf("actual = "); print_temp(*actual);
    print_ports();
    free(actual);
  } // ENDTEST

  ////////////////////////////////////////////////////////////////////////////////
  else if( strcmp( test_name, "set_display_from_temp() {-500, 0}" )==0 ) {
    PRINT_TEST;
    // Check that set_display_from_temp() sets uses the passed in
    // temp_t struct to change the 'display' variable which has its
    // address passed in to the function; all PORTS variables should
    // be ignored and remain unchanged through the
    // set_display_from_temp() call.
    THERMO_SENSOR_PORT  = -1;
    THERMO_STATUS_PORT  = -1;
    THERMO_DISPLAY_PORT = -1;
    temp_t actual = {
      .tenths_degrees = -500,
      .is_fahrenheit  = 0,
    };
    int *display = malloc(sizeof(int)); *display = -1;
    int ret = set_display_from_temp(actual, display);
    printf("ret: %d\n",ret);
    print_temp(actual);
    printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    print_ports();
    free(display);
  } // ENDTEST

  else if( strcmp( test_name, "set_display_from_temp() {-500 1}" )==0 ) {
    PRINT_TEST;
    // Check that set_display_from_temp() sets uses the passed in
    // temp_t struct to change the 'display' variable which has its
    // address passed in to the function; all PORTS variables should
    // be ignored and remain unchanged through the
    // set_display_from_temp() call.
    THERMO_SENSOR_PORT  = -1;
    THERMO_STATUS_PORT  = -1;
    THERMO_DISPLAY_PORT = -1;
    temp_t actual = {
      .tenths_degrees = -500,
      .is_fahrenheit  = 1,
    };
    int *display = malloc(sizeof(int)); *display = -1;
    int ret = set_display_from_temp(actual, display);
    printf("ret: %d\n",ret);
    print_temp(actual);
    printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    print_ports();
    free(display);
  } // ENDTEST

  else if( strcmp( test_name, "set_display_from_temp() -289 0" )==0 ) {
    PRINT_TEST;
    // Check that set_display_from_temp() sets uses the passed in
    // temp_t struct to change the 'display' variable which has its
    // address passed in to the function; all PORTS variables should
    // be ignored and remain unchanged through the
    // set_display_from_temp() call.
    THERMO_SENSOR_PORT  = -1;
    THERMO_STATUS_PORT  = -1;
    THERMO_DISPLAY_PORT = -1;
    temp_t actual = {
      .tenths_degrees = -500,
      .is_fahrenheit  = 1,
    };
    int *display = malloc(sizeof(int)); *display = -1;
    int ret = set_display_from_temp(actual, display);
    printf("ret: %d\n",ret);
    print_temp(actual);
    printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    print_ports();
    free(display);
  } // ENDTEST

  else if( strcmp( test_name, "set_display_from_temp() 289 1" )==0 ) {
    PRINT_TEST;
    // Check that set_display_from_temp() sets uses the passed in
    // temp_t struct to change the 'display' variable which has its
    // address passed in to the function; all PORTS variables should
    // be ignored and remain unchanged through the
    // set_display_from_temp() call.
    THERMO_SENSOR_PORT  = -1;
    THERMO_STATUS_PORT  = -1;
    THERMO_DISPLAY_PORT = -1;
    temp_t actual = {
      .tenths_degrees = 289,
      .is_fahrenheit  = 1,
    };
    int *display = malloc(sizeof(int)); *display = -1;
    int ret = set_display_from_temp(actual, display);
    printf("ret: %d\n",ret);
    print_temp(actual);
    printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    print_ports();
    free(display);
  } // ENDTEST

  else if( strcmp( test_name, "set_display_from_temp() 479 0" )==0 ) {
    PRINT_TEST;
    // Check that set_display_from_temp() sets uses the passed in
    // temp_t struct to change the 'display' variable which has its
    // address passed in to the function; all PORTS variables should
    // be ignored and remain unchanged through the
    // set_display_from_temp() call.
    THERMO_SENSOR_PORT  = -1;
    THERMO_STATUS_PORT  = -1;
    THERMO_DISPLAY_PORT = -1;
    temp_t actual = {
      .tenths_degrees = 479,
      .is_fahrenheit  = 0,
    };
    int *display = malloc(sizeof(int)); *display = -1;
    int ret = set_display_from_temp(actual, display);
    printf("ret: %d\n",ret);
    print_temp(actual);
    printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    print_ports();
    free(display);
  } // ENDTEST

  else if( strcmp( test_name, "set_display_from_temp() 45 0" )==0 ) {
    PRINT_TEST;
    // Check that a blank is properly filled in for the left-most
    // digit; will be 4.5 C
    THERMO_SENSOR_PORT  = -1;
    THERMO_STATUS_PORT  = -1;
    THERMO_DISPLAY_PORT = -1;
    temp_t actual = {
      .tenths_degrees = 45,
      .is_fahrenheit  = 0,
    };
    int *display = malloc(sizeof(int)); *display = -1;
    int ret = set_display_from_temp(actual, display);
    printf("ret: %d\n",ret);
    print_temp(actual);
    printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    print_ports();
    free(display);
  } // ENDTEST

  else if( strcmp( test_name, "set_display_from_temp() 7 1" )==0 ) {
    PRINT_TEST;
    // Check that a blank followed by a 0 is properly filled in for
    // the left-two digits; will be 0.7 F
    THERMO_SENSOR_PORT  = -1;
    THERMO_STATUS_PORT  = -1;
    THERMO_DISPLAY_PORT = -1;
    temp_t actual = {
      .tenths_degrees = 7,
      .is_fahrenheit  = 1,
    };
    int *display = malloc(sizeof(int)); *display = -1;
    int ret = set_display_from_temp(actual, display);
    printf("ret: %d\n",ret);
    print_temp(actual);
    printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    print_ports();
    free(display);
  } // ENDTEST

  else if( strcmp( test_name, "set_display_from_temp() -82 1" )==0 ) {
    PRINT_TEST;
    // Check that a blank followed by a - (negative) is properly filled in for
    // the left-two digits; will be -8.2 F
    THERMO_SENSOR_PORT  = -1;
    THERMO_STATUS_PORT  = -1;
    THERMO_DISPLAY_PORT = -1;
    temp_t actual = {
      .tenths_degrees = -82,
      .is_fahrenheit  = 1,
    };
    int *display = malloc(sizeof(int)); *display = -1;
    int ret = set_display_from_temp(actual, display);
    printf("ret: %d\n",ret);
    print_temp(actual);
    printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    print_ports();
    free(display);
  } // ENDTEST

  else if( strcmp( test_name, "set_display_from_temp() -3 0" )==0 ) {
    PRINT_TEST;
    // Check that a blank followed by a - (negative) then a zero is
    // properly filled in for the left-two digits; will be -0.3 C
    THERMO_SENSOR_PORT  = -1;
    THERMO_STATUS_PORT  = -1;
    THERMO_DISPLAY_PORT = -1;
    temp_t actual = {
      .tenths_degrees = -3,
      .is_fahrenheit  = 0,
    };
    int *display = malloc(sizeof(int)); *display = -1;
    int ret = set_display_from_temp(actual, display);
    printf("ret: %d\n",ret);
    print_temp(actual);
    printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    print_ports();
    free(display);
  } // ENDTEST

  else if( strcmp( test_name, "set_display_from_temp() 1205 1" )==0 ) {
    PRINT_TEST;
    // Near upper range for Fahrenheit, 120.5 F
    THERMO_SENSOR_PORT  = -1;
    THERMO_STATUS_PORT  = -1;
    THERMO_DISPLAY_PORT = -1;
    temp_t actual = {
      .tenths_degrees = 1205,
      .is_fahrenheit  = 1,
    };
    int *display = malloc(sizeof(int)); *display = -1;
    int ret = set_display_from_temp(actual, display);
    printf("ret: %d\n",ret);
    print_temp(actual);
    printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    print_ports();
    free(display);
  } // ENDTEST

  ////////////////////////////////////////////////////////////////////////////////
  else if( strcmp( test_name, "set_display_from_temp() errors" )==0 ) {
    PRINT_TEST;
    // Checks that set_display_from temp checks for max/min temps in
    // the temp_t struct that is passed in
    THERMO_SENSOR_PORT  = -1;
    THERMO_STATUS_PORT  = -1;
    THERMO_DISPLAY_PORT = -1;
    int *display = malloc(sizeof(int)); *display = -1;
    {
      printf("Fail above MAX Celsius\n");
      temp_t actual = {
        .tenths_degrees = 745,
        .is_fahrenheit  = 0,
      };
      int ret = set_display_from_temp(actual, display);
      printf("ret: %d\n",ret);
      printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    }
    {
      printf("Fail below MIN Celsius\n");
      temp_t actual = {
        .tenths_degrees = -621,
        .is_fahrenheit  = 0,
      };
      int ret = set_display_from_temp(actual, display);
      printf("ret: %d\n",ret);
      printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    }
    {
      printf("Fail above MAX Fahrenheit\n");
      temp_t actual = {
        .tenths_degrees = 1439,
        .is_fahrenheit  = 0,
      };
      int ret = set_display_from_temp(actual, display);
      printf("ret: %d\n",ret);
      printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    }
    {
      printf("Fail below MIN Fahrenheit\n");
      temp_t actual = {
        .tenths_degrees = -596,
        .is_fahrenheit  = 0,
      };
      int ret = set_display_from_temp(actual, display);
      printf("ret: %d\n",ret);
      printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    }
    {
      printf("Fail if is_farenheit field not 0/1 \n");
      temp_t actual = {
        .tenths_degrees = 320,
        .is_fahrenheit  = 4,
      };
      int ret = set_display_from_temp(actual, display);
      printf("ret: %d\n",ret);
      printf("%-20s : %s\n","display",asbits(*display, INT_BITS));
    }
    free(display);
  } // ENDTEST

  ////////////////////////////////////////////////////////////////////////////////
  else if( strcmp( test_name, "thermo_update() 0 C" )==0 ) {
    PRINT_TEST;
    // Thermo update with min sensor value, Celsius mode
    THERMO_SENSOR_PORT  = 0;
    THERMO_STATUS_PORT  = 0b0000;
    THERMO_DISPLAY_PORT = -1;
    int ret = thermo_update();
    printf("ret: %d\n",ret);
    print_ports();
  } // ENDTEST
  
  else if( strcmp( test_name, "thermo_update() 1288 F" )==0 ) {
    PRINT_TEST;
    // Thermo update with small sensor value, Fahrenheit mode
    THERMO_SENSOR_PORT  = 1288;
    THERMO_STATUS_PORT  = 0b0001;
    THERMO_DISPLAY_PORT = -1;
    int ret = thermo_update();
    printf("ret: %d\n",ret);
    print_ports();
  } // ENDTEST
  
  else if( strcmp( test_name, "thermo_update() 58172 C" )==0 ) {
    PRINT_TEST;
    // Thermo update with high sensor value, Celsius mode
    THERMO_SENSOR_PORT  = 58172;
    THERMO_STATUS_PORT  = 0b0000;
    THERMO_DISPLAY_PORT = -1;
    int ret = thermo_update();
    printf("ret: %d\n",ret);
    print_ports();
  } // ENDTEST

  else if( strcmp( test_name, "thermo_update() 58172 F" )==0 ) {
    PRINT_TEST;
    // Thermo update with high sensor value, Fahrenheit mode
    THERMO_SENSOR_PORT  = 58172;
    THERMO_STATUS_PORT  = 0b0001;
    THERMO_DISPLAY_PORT = -1;
    int ret = thermo_update();
    printf("ret: %d\n",ret);
    print_ports();
  } // ENDTEST

  else if( strcmp( test_name, "thermo_update() 0 deg C" )==0 ) {
    PRINT_TEST;
    // Thermo update with mid sensor value, 0.0 deg Celsius, upper
    // bits of status port are non-zero
    THERMO_SENSOR_PORT  = 64*10*50;
    THERMO_STATUS_PORT  = 0b0110;    // non-zero upper bits
    THERMO_DISPLAY_PORT = -1;
    int ret = thermo_update();
    printf("ret: %d\n",ret);
    print_ports();
  } // ENDTEST

  else if( strcmp( test_name, "thermo_update() 32 deg F" )==0 ) {
    PRINT_TEST;
    // Thermo update with mid sensor value, 32.0 deg Celsius, upper
    // bits of status port are non-zero
    THERMO_SENSOR_PORT  = 64*10*50;
    THERMO_STATUS_PORT  = 0b1101;    // non-zero upper bits
    THERMO_DISPLAY_PORT = -1;
    int ret = thermo_update();
    printf("ret: %d\n",ret);
    print_ports();
  } // ENDTEST


  else if( strcmp( test_name, "thermo_update() sensor val fails" )==0 ) {
    PRINT_TEST;
    // Thermo update with high sensor value, Fahrenheit mode
    THERMO_SENSOR_PORT  = 64*521;
    THERMO_STATUS_PORT  = 0b1111;
    THERMO_DISPLAY_PORT = -1;
    int ret = thermo_update();
    printf("ret: %d\n",ret);
    print_ports();
  } // ENDTEST

  else{
    printf("No test named '%s' found\n",test_name);
    return 1;
  }


  // free(temp);
  // free(disp);
  return 0;
}
