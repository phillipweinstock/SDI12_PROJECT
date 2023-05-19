#ifndef PARSER_H
#define PARSER_H

#include <Arduino.h>
#include <CRC16.h>
#include "..\lib\macros.h"
#include "..\lib\sensors.h"
// Which measurement type we are selecting.

class Parser
{
public:
    Parser(char address = '0')
    {
        target = crcreq = false;
        sdi12add[0] = address;
        sdi12add[1] = crcASCII[3] = '\0';

        crc = CRC16(0xA001, 0, 0, false, false);
    };
    int parse(char *buffer, u_int8_t commandlength);

private:
    bool target;
    int retval;
    bool reading_avaliable;
    char sdi12add[2];
    // CRC-16-IBM Polynomial 0xA001 final XOR = 0 Bits reflected
    CRC16 crc;
    void calccrc();
    bool crcreq;
    char crcASCII[4];
    char outputbuf[200];

    // time_t previous_t;

    airoutput air_reading;
    uint16_t lux_reading;
    // void vprint(int cargs = 0, ...)
    // {
    //     va_list args;
    //     va_start(args, cargs);
    //     for (int i = 0; i < cargs - 1; i++)
    //     {
    //         sdi12.print(va_arg(args, char *));
    //     }
    //     sdi12.println(va_arg(args, char *));
    //     va_end(args);
    // }
};
void Parser::calccrc()
{
    crc.add((uint8_t *)outputbuf, strlen(outputbuf));
    // Page 21 Ver 1.4 SDI-12 standard 4.4.12.2
    uint16_t crcraw = crc.getCRC();
    crcASCII[0] = 0x40 | (crcraw >> 12);
    crcASCII[1] = 0x40 | ((crcraw >> 6) & 0x3F);
    crcASCII[2] = 0x40 | (crcraw & 0x3F);
    crc.restart();
    //  sdi12.println(crcASCII);
}
int Parser::parse(char *buffer, u_int8_t commandlength)
{
    target = buffer[0] == sdi12add[0];
    if (commandlength == 1 && target)
    {
        SEND();
        sdi12.println(sdi12add);
        CLR12();
        return EXIT_SUCCESS;
    }
    if (commandlength == 1 && buffer[0] == '?')
    {
        SEND();
        sdi12.println(sdi12add);
        Serial.println("Recieved Query");
        CLR12();
        return EXIT_SUCCESS;
    }
    if (target && commandlength > 1)
    {
        switch (buffer[1])
        {
        case 'I':
        {
            SEND();
            sprintf(outputbuf, "%s10SWINBURNSDIBRG001", sdi12add);
            sdi12.println(outputbuf);
            CLR12();
            return EXIT_SUCCESS;
        }
        break;
        case 'R':
        {
            // Continous measurement support
            SEND();
            int select;
            crcreq = buffer[2] == 'C';
            lux_reading = sensors.lightmeter.readLightLevel(); // ick
            air_reading = sensors.airreading();
            if (crcreq)
            {
                select = buffer[3] - '0';
            }
            else
            {
                select = buffer[2] - '0';
            }
            switch (select)
            {
            case 0:
            {
                sprintf(outputbuf, "%s+%u", sdi12add, lux_reading);
            }
            break;
            case 1:
            {
                sprintf(outputbuf, "%s+%.2f", sdi12add, air_reading.temp);
            }
            break;
            case 2:
            {
                sprintf(outputbuf, "%s+%u", sdi12add, air_reading.pressure);
            }
            break;
            case 3:
            {
                sprintf(outputbuf, "%s+%.2f", sdi12add, air_reading.humidity);
            }
            break;
            case 4:
            {
                sprintf(outputbuf, "%s+%u", sdi12add, air_reading.gasresistance);
            }
            break;

            default:
            {
                sprintf(outputbuf, "%s", sdi12add);
            }
            }
            // int bufindex = 2 + 1*crcreq;
            // Shift which character we are checking
            // if CRC is requested
            if (crcreq)
            {
                sdi12.print(outputbuf);
                Serial.println("output CRC");
                calccrc();
                sdi12.println(crcASCII);
            }
            else
            {
                sdi12.println(outputbuf);
            }

            CLR12();
            return EXIT_SUCCESS;
        };
        case 'M':
        case 'C':
        {
            // Supports Measurements + concurrent ones
            crcreq = buffer[2] == 'C';
            SEND();
            lux_reading = sensors.lightmeter.readLightLevel(); // ick
            air_reading = sensors.airreading();
            reading_avaliable = true; // double ick
            sprintf(outputbuf, "%s0005", sdi12add);
            sdi12.println(outputbuf);
            CLR12();

            if (commandlength == 3)
            {
                // Not implemented yet... change measurement page
            }
            // sensors.
            //  This triggers sensor reading
            //  and placement into sending buffer
            return EXIT_SUCCESS;
        }
        break;
        case 'D':
        {
            if (commandlength == 3)
            {
                // We should check the page here but seeing as we have one page assume that it is 0
            }

            // lux_reading = sensors.lightmeter.readLightLevel(); // ick
            // air_reading = sensors.airreading();// double ick
            // if(previous_t + 200)
            if (commandlength == 3 && reading_avaliable)
            {
                reading_avaliable = false;
                SEND();
                sprintf(outputbuf, "%s+%u+%.2f+%u+%.2f+%u", sdi12add, lux_reading, air_reading.temp, air_reading.pressure, air_reading.humidity, air_reading.gasresistance);
                if (crcreq)
                {
                    sdi12.print(outputbuf);
                    //Serial.println("output CRC");
                    calccrc();
                    sdi12.println(crcASCII);
                }
                else
                {
                    sdi12.println(outputbuf);
                }
                

                CLR12();
            }
            // sensors.
            //  This triggers sensor reading
            //  and placement into sending buffer
            return EXIT_SUCCESS;
        }
        break;
        case 'A':
        {
            if (isprint(buffer[2]))
            {
                sdi12add[0] = buffer[2];
                SEND();
                sdi12.println(sdi12add);
                CLR12();
                // Now we need to trigger a change in config
                return EXIT_SUCCESS;
            }
            return EXIT_FAILURE;
        }
        break;
        }
    }

    return retval;
}

#endif