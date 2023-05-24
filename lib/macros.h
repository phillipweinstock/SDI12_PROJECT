#ifndef MACROS_H
#define MACROS_H
#include <Arduino.h>
/*
    Hard coded for digital pin Seven, if desired a compile time lookup table could be used.
    The purpose of these macros is to switch the direction pin faster than the arduino lib
    It is roughy 300x faster.
    It is not needed for the project to function with correct behaviour, it's just something nice... IF THE HARDWARE DIDN'T FORCE A DELAY!!!
   (⩺︷⩹)
*/
#define RECEIVEF REG_PIOC_SODR |= (0x1 << 23);
#define SENDF REG_PIOC_CODR |= (0x1 << 23);
/*
    This is only due to the delay in the pulling of the line not being slow enough for the SDI12-USB converter.
    So much time wasted due to no documentation (~_~)
*/
#define SEND()    \
    {             \
        SENDF;    \
        delay(5); \
    }
#define RECIEVE() \
    {             \
        RECEIVEF; \
        delay(5); \
    }

#define sdi12 Serial1
/*
    This is in order to clear the Serial buffer after we send otherwise it will be reinterpreted as input. ಠ▃ಠ
*/
#define CLR12()                        \
    {                                  \
        sdi12.flush();                 \
        sdi12.end();                   \
        sdi12.begin(1200, SERIAL_7E1);\
    }

#define DEBUG 0
#endif