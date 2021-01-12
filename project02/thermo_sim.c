// thermo_sim.c: DO NOT MODIFY
//
// Thermometer simulator support functions.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thermo.h"

#define THERMO_MAX_BITS 30

// Convenience struct for representing the thermometer display  as characters 
typedef struct {
  char chars[5][32];
} thermo_display;


// Global variable used to control the THERMO display. Making changes to
// this variable will change the thermostat display. Type ensures 32 bits.
int THERMO_DISPLAY_PORT = 0;

// Temperature above -50 C in 0.1/64 degrees C increments.
// 
//      0 is -50.0 C
//     64 is -49.9 C
//    128 is -49.8 C
//  16000 is -25.0 C
//  32000 is   0.0 C
//  48000 is  25.0 C
//  64000 is  50.0 C
// 
// Always positive. In a real machine this would be set by physical
// hardware and read only.
unsigned short THERMO_SENSOR_PORT = 0;

// Indicates status of themometer. Low order bit is 0 for Celsius
// display and 1 for Fahrenheit.
unsigned char THERMO_STATUS_PORT = 0;

//         ~~   ~~   ~~           0
//        |    |  | |  |          1
//    ~~   ~~   ~~       O        2
//           | |  | |  |  F       3
//         ~~   ~~ o ~~           4
//01234567890123456789012345678901 
//0         1         2         3

// position of decimal place is at row 4, col 17

// Reset thermo_display to be empty 
void reset_thermo_display(thermo_display *thermo){
  int i,j;
  for(i=0; i<5; i++){
    for(j=0; j<32; j++){
      thermo->chars[i][j] = ' ';
    }
    thermo->chars[i][31] = '\0';
  }
  thermo->chars[4][17]= 'o';
}  

// Print an thermo_display 
void internal_print_thermo_display(thermo_display *thermo){
  int i=0;
  for(i=0; i<5; i++){
    printf("%s\n",thermo->chars[i]);
  }
}  

// Data and functions to set the state of the thermo clock display 

// Position and char in display 
typedef struct {
  int r,c; int ch;
} charpos;
    
// Collection of characters corresponding to one bit in the state being set 
typedef struct {
  int len;                      // number of chars corresponding to this bit
  charpos pos[2];               // position of chars for this bit
} charpos_coll;

// Correspondence of bit positions to which characters should be set 
charpos_coll bits2chars[THERMO_MAX_BITS] = {
  { .len=2, .pos={{ 2,19,'~'}, { 2,20,'~'},} }, //  0
  { .len=2, .pos={{ 0,19,'~'}, { 0,20,'~'},} }, //  1
  { .len=1, .pos={{ 1,21,'|'},             } }, //  2
  { .len=1, .pos={{ 3,21,'|'},             } }, //  3
  { .len=2, .pos={{ 4,19,'~'}, { 4,20,'~'},} }, //  4
  { .len=1, .pos={{ 3,18,'|'},             } }, //  5
  { .len=1, .pos={{ 1,18,'|'},             } }, //  6

  { .len=2, .pos={{ 2,14,'~'}, { 2,15,'~'},} }, //  7 
  { .len=2, .pos={{ 0,14,'~'}, { 0,15,'~'},} }, //  8 
  { .len=1, .pos={{ 1,16,'|'},             } }, //  9 
  { .len=1, .pos={{ 3,16,'|'},             } }, // 10 
  { .len=2, .pos={{ 4,14,'~'}, { 4,15,'~'},} }, // 11 
  { .len=1, .pos={{ 3,13,'|'},             } }, // 12 
  { .len=1, .pos={{ 1,13,'|'},             } }, // 13 

  { .len=2, .pos={{ 2, 9,'~'}, { 2,10,'~'},} }, // 14   
  { .len=2, .pos={{ 0, 9,'~'}, { 0,10,'~'},} }, // 15   
  { .len=1, .pos={{ 1,11,'|'},             } }, // 16   
  { .len=1, .pos={{ 3,11,'|'},             } }, // 17   
  { .len=2, .pos={{ 4, 9,'~'}, { 4,10,'~'},} }, // 18   
  { .len=1, .pos={{ 3, 8,'|'},             } }, // 19   
  { .len=1, .pos={{ 1, 8,'|'},             } }, // 20   

  { .len=2, .pos={{ 2, 4,'~'}, { 2, 5,'~'},} }, // 21    
  { .len=2, .pos={{ 0, 4,'~'}, { 0, 5,'~'},} }, // 22    
  { .len=1, .pos={{ 1, 6,'|'},             } }, // 23    
  { .len=1, .pos={{ 3, 6,'|'},             } }, // 24    
  { .len=2, .pos={{ 4, 4,'~'}, { 4, 5,'~'},} }, // 25    
  { .len=1, .pos={{ 3, 3,'|'},             } }, // 26    
  { .len=1, .pos={{ 1, 3,'|'},             } }, // 27    

  { .len=2, .pos={{ 0, 23,'o'}, { 1, 24,'C'},} }, // 28
  { .len=2, .pos={{ 2, 23,'o'}, { 3, 24,'F'},} }, // 29
  // { .len=2, .pos={{ 3, 23,L'°'}, { 3, 1,'C'},} }, // 28
  // { .len=2, .pos={{ 4, 23,L'°'}, { 4, 1,'F'},} }, // 29
};

// Assumes ints are at least 32 bits 
void set_thermo_display(thermo_display *thermo, int disp){
  int i,j;
  int mask = 0x1;
  reset_thermo_display(thermo);
  for(i=0; i<THERMO_MAX_BITS; i++){
    //    printf("Checking %d\n",i);
    if( disp & (mask << i) ){ // ith bit set, fill in characters 
      //      printf("%d IS SET\n",i);
      charpos_coll coll = bits2chars[i];
      //      printf("Coll len: %d\n",coll.len);
      for(j=0; j<coll.len; j++){
        //        printf("Inner iter %d\n",j);
        charpos pos = coll.pos[j];
        thermo->chars[pos.r][pos.c] = pos.ch;
        // print_thermo_display(thermo);
      }
    }
  }
}


// Use the global THERMO_DISPLAY_PORT to print the time 
void print_thermo_display(){
  thermo_display thermo;
  set_thermo_display(&thermo, THERMO_DISPLAY_PORT);
  internal_print_thermo_display(&thermo);
  return;
}


// Print the most signficant (right-most) to least signficant bit in
// the integer passed in 
void showbits(int x, int bits){
  int i;
  int mask = 0x1;
  for(i=bits-1; i>=0; i--){
    int shifted_mask = mask << i;
    if(shifted_mask & x){
      putchar('1');
    } else {
      putchar('0');
    }
  }
  // Equivalent short version 
  //    (x&(1<<i)) ? putchar('1'): putchar('0');
}

char *asbits(int x, int bits){
  static char buffer[256];
  int i,idx=0;
  int mask = 0x1;
  for(i=bits-1; i>=0; i--, idx++){
    int shifted_mask = mask << i;
    buffer[idx]= (shifted_mask & x) ? '1' : '0';
  }
  buffer[idx] = '\0';
  return buffer;
}

