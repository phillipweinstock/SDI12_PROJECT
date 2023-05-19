#ifndef MACROS_H
#define MACROS_H
#include <Arduino.h>
/*
    Hard coded for digital pin Seven, if desired a compile time lookup table could be used.
    The purpose of these macros is to switch the direction pin faster than the arduino lib
    It is roughy 300x faster.
*/
#define RECEIVEF REG_PIOC_SODR |= (0x1 << 23);
#define SENDF REG_PIOC_CODR |= (0x1 << 23);
/*
    This is only due to the delay in the pulling of the line not being slow enough for the SDI-USB converter
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
    This is in order to clear the Serial buffer after we send otherwise it will be reinterpreted as input
*/
#define CLR12()                        \
    {                                  \
        sdi12.flush();                 \
        sdi12.end();                   \
        sdi12.begin(1200, SERIAL_7E1); \
    \ 
}
#endif