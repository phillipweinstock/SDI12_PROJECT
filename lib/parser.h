#ifndef PARSER_H
#define PARSER_H
#include <Arduino.h>
#include <CRC16.h>
#include <DueFlashStorage.h>
#include "..\lib\macros.h"
#include "..\lib\sensors.h"

class Parser
{
public:
    Parser(char address = '0')
    {
        target = crcreq = false;
        sdi12add[0] = address;
        sdi12add[1] = crcASCII[3] = '\0';
        cont_measure = false;

        // CRC-16-IBM Polynomial 0xA001 final XOR = 0 Bits reflected
        crc = CRC16(0xA001, 0, 0, false, false);
        flash = DueFlashStorage();
    };
    int parse(char *buffer, u_int8_t commandlength);
    DueFlashStorage flash; // I hate this leaky abstraction, I don't have the energy to fix it. ಠ╭╮ಠ
    void setadd(char add);

private:
    bool target;
    int retval;
    bool reading_avaliable;
    bool cont_measure;
    void refresh_vals();
    char sdi12add[2];
    CRC16 crc;
    void calccrc();
    bool crcreq;
    char crcASCII[4];
    char outputbuf[200];
    airoutput air_reading;
    uint16_t lux_reading;
};
void Parser::refresh_vals()
{
    lux_reading = sensors.lightmeter.readLightLevel(); // ick
    air_reading = sensors.airreading();
}
void Parser::setadd(char add)
{
    // run this if the address is different from the default or not first run.
    sdi12add[0] = add;
}
void Parser::calccrc()
{
    /*
     Page 21 Ver 1.4 SDI-12 standard 4.4.12.2
     At first this was harder than I was expecting, after 3 days I realised that I had not used bitwise operations but rather logical ones...
     It seems easy now. ¯\_( ͡° ͜ʖ ͡°)_/¯
    */

    crc.add((uint8_t *)outputbuf, strlen(outputbuf));
    uint16_t crcraw = crc.getCRC();
    crcASCII[0] = 0x40 | (crcraw >> 12);
    crcASCII[1] = 0x40 | ((crcraw >> 6) & 0x3F);
    crcASCII[2] = 0x40 | (crcraw & 0x3F);
    crc.restart();
}
int Parser::parse(char *buffer, u_int8_t commandlength)
{
    /*
        Excluding the break command, we should have full SDI-12 Ver 1.4 Feb 20, 2023 compliance.
        Break command would end up doing nothing anyway, as it cancels a measurement, but this is only the case if a service request exists
        additionally the device cannot sleep anyway due to it's other functions
        TODO: Fix this mess of returns
    */
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

        CLR12();
        return EXIT_SUCCESS;
    }
    if (target && commandlength > 1)
    {
        switch (buffer[1])
        {
        case 'I':
        {
            // TODO: META DATA COMMANDS
            SEND();
            sprintf(outputbuf, "%s14SWINBURNSDIBRG001", sdi12add);
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
            refresh_vals();
            /*
             Shift which character we are checking depending if CRC is requested.
             Then we will select our sensor for instantanious measurement
            */
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
                sprintf(outputbuf, "%s+%lu", sdi12add, air_reading.pressure);
            }
            break;
            case 3:
            {
                sprintf(outputbuf, "%s+%.2f", sdi12add, air_reading.humidity);
            }
            break;
            case 4:
            {
                sprintf(outputbuf, "%s+%lu", sdi12add, air_reading.gasresistance);
            }
            break;

            default:
            {
                sprintf(outputbuf, "%s", sdi12add);
            }
            }

            if (crcreq)
            {
                sdi12.print(outputbuf);
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
        case 'C':
            cont_measure = true;
        case 'M':
        {
            // Supports Measurements + concurrent ones
            // Command is basically the same in implementation.

            // Lets check if CRC has been requested
            crcreq = buffer[2] == 'C';
            /*
                lets check if aM1!..aM9 required for ver 1.2 SDI-12 compliance
                Not implemented, not going to be.... Still compliant due to response
                If command length differs due to CRC request adjust for correct input;
                TODO: sanitize input
            */

            bool adms = false;
            if ((crcreq && commandlength >= 4) || (commandlength >= 3 && !crcreq))
            {
                adms = !adms;
            }

            /*  Full Compliance aM! aMC!  aM(0-9)! aMC(0-9)! aC! aCC! aC(0-9)! aCC(0-9)!.
                ttt is zero no service request is required to indicate the sensor is ready for reading Yay!
                4.4.6 Service request compliance  (*˘︶˘*)
            */
            if (adms)
            {

                sprintf(outputbuf, "%s0000", sdi12add);
            }
            else
            {
                //  This triggers sensor reading
                //  and placement into sending buffer
                refresh_vals();
                reading_avaliable = true; // double ick
                sprintf(outputbuf, "%s0005", sdi12add);
            }
            SEND();

            sdi12.println(outputbuf);
            CLR12();

            if (commandlength == 3)
            {
                // Not implemented, not going to be, this will be optimised out as it is a dead branch.
            }
            return EXIT_SUCCESS;
        }
        break;
        case 'D':
        {
            if (commandlength == 3)
            {
                // We should check the page here but seeing as we have one page assume that it is 0.
                // This is technically non compliant,but this behaviour is acceptible due to one page existing.
                // Not implemented, not going to be, this will be optimised out as it is a dead branch. ╭( ๐ _๐)╮
            }

            if (commandlength == 3 && reading_avaliable)
            {
                reading_avaliable = false;
                if (cont_measure)
                {
                    refresh_vals();
                    cont_measure = false;
                }
                SEND();
                sprintf(outputbuf, "%s+%u+%.2f+%u+%.2f+%lu", sdi12add, lux_reading, air_reading.temp, air_reading.pressure, air_reading.humidity, air_reading.gasresistance);
                if (crcreq)
                {
                    sdi12.print(outputbuf);
                    calccrc();
                    sdi12.println(crcASCII);
                }
                else
                {
                    sdi12.println(outputbuf);
                }

                CLR12();
            }
            return EXIT_SUCCESS;
        }
        break;
        case 'H':
        {
            /*
            5.4 Compliance with High-vol commands.
            ver 1.4, sensors that do not support this command must return zero data values ready.
            */

            sprintf(outputbuf, "%s000000", sdi12add);

            SEND();

            sdi12.println(outputbuf);
            CLR12();
            return EXIT_SUCCESS;
        }
        break;

        case 'A':
        {
            if (isprint(buffer[2]))
            {
                sdi12add[0] = buffer[2];
                /*
                    Now we need to trigger a change in config, for now this is the only thing we are changing, in the future we should use a struct
                    we must disable all interrupts while writing to flash mem, otherwise we will murder our flash page!
                   ("En Garde!")> ╰༼.◕ヮ ◕.༽つ¤=[]——————  ⋌༼ •̀ ⌂ •́ ༽⋋ <("Can I get workers comp?")
                    1 = Disable all interrupts
                    0 = Enable all interrupts
                */
                __set_FAULTMASK(1);
                flash.write(1, sdi12add[0]);
                __set_FAULTMASK(0);

                SEND();
                sdi12.println(sdi12add);
                CLR12();

                return EXIT_SUCCESS;
            }
            retval = EXIT_FAILURE;
        }
        break;
        }
    }
    return retval;
}

#endif