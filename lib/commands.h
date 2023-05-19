#ifndef COMMAND_H
#define COMMAND_H
#include <Arduino.h>
// const unsigned long hash(const char *str) {
//   unsigned long hash = 5381;
//   int c;

//   while ((c = *str++))
//     hash = ((hash << 5) + hash) + c;
//   return hash;
// }//DJB hashing function
class CommandLine
{
public:
    Stream *device_ref; // Sloppy abstraction due to requiring the implementation of << in command parser
    CommandLine(Stream *Serialx, char *buf, u_int8_t buffsize, int direct = -1)
    {
        device_ref = Serialx;
        diro = direct;
        buffer = buf;
        bufferSize = buffsize;
        ndx = 0;
        readchars = 0;
        if (directionality())
        {
            pinMode(diro, OUTPUT);
        }
    };
    int serialread();
    void resetstatus();

    uint8_t readchars;

private:

    // static const byte bufferSize = 200;
    char *buffer;
    int bufferSize;
    // static char buffer[bufferSize];
    int bufReady = false;
    int diro;
    byte ndx;
    constexpr bool directionality()
    {
        return (diro != -1);
    };
};
void CommandLine::resetstatus(){
    bufReady = false;
}


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

                // Serial.print(rc);
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
enum commands
{
    noCommand = 50,
    ackAct,
    sdId,
    chnAd,
    adQry,
    strtM,
    strtMCrc,
    sdDa,
    adM,
    admCrc,
    strtV,
    strtC,
    strtCCrc,
    adC,
    adCCrc,
    conM,
    conMCrc
};
#endif
