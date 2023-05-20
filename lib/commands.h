#ifndef COMMAND_H
#define COMMAND_H
#include <Arduino.h>
class CommandLine
{
public:
   
    CommandLine(Stream *Serialx, char *buf, u_int8_t buffsize, int direct = -1)
    {
        device_ref = Serialx;
        diro = direct;
        buffer = buf;
        bufferSize = buffsize;
        ndx = 0;
        readchars = 0;
        bufReady = false;
        if (directionality())
        {
            pinMode(diro, OUTPUT);
        }
    };
    int serialread();
    void resetstatus();

    uint8_t readchars;

private:
    char *buffer;
    int bufferSize;
    int bufReady;
    int diro;
    byte ndx;
    Stream *device_ref;
    /*This can be removed alongside SDI-12 line macros, when proper hardware is utilised*/
    constexpr bool directionality()
    {
        return (diro != -1);
    };
};

void CommandLine::resetstatus()
{
    bufReady = false;
}
/* Non blocking serial reading*/
int CommandLine::serialread()
{
    {
        bufReady = false;
        if (device_ref->available() > 0)
        {
            char rc = device_ref->read();
            if (rc != '!' && isprint(rc) && rc != 0)
            {
                buffer[ndx++] = rc;

                if (ndx >= bufferSize)
                {
                    ndx = bufferSize - 1;
                }
            }
            else
            {
                buffer[ndx] = '\0'; // terminate the string
                readchars = ndx;
                ndx = 0;
                bufReady = true;
            }
        }
    }
    return bufReady;
}
#endif
