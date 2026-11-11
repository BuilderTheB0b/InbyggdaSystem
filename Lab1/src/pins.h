#ifndef PINS_H_
#define PINS_H_

#include <stdint.h>

/* initialises the 4 pins */
void initPins();

/* switches LED A on if level!=0 or off if level==0*/
void setLED1(uint8_t level);

/* switches LED B on if level!=0 or off if level==0*/
void setLED2(uint8_t level);

/* true if button A is currently being pressed */
uint8_t isButton1Pressed();

/* true if button A is currently being pressed */
uint8_t isButton2Pressed();

#endif