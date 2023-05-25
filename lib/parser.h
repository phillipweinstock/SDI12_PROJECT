#ifndef PARSER_H
#define PARSER_H
#include "macros.h"
#include "sensors.h"
#include <Arduino.h>
#include <CRC16.h>
#include <DueFlashStorage.h>
#include <malloc.h>
/*
    Developed by Phillip Weinstock student ID 102101219
    Contact 102101219@student.swin.edu.au for all Academic/Business enquirys.
    Copyright 2023© ALL RIGHTS RESERVED.
*/
extern char _end;
extern "C" char *sbrk(int i);
char *ramstart = (char *)0x20070000;
char *ramend = (char *)0x20088000;
struct MetaData {
  char shef[3]; // Standard Hydrometeorological Exchange Format
  char units[20];
};

MetaData paramdata[] = {
    {"RP", "1cd*sr/m2"},     // LUX
    {"TA", "C"},             // TEMP
    {"PA", "Kpa"},           // Pressure
    {"XR", "Humidity"},      // Humidity
    {"GR", "Gas Resistance"} // Gas Resistance
};

class Parser {
public:
  Parser(char address = '0') {
    target = crcReq = adms = contMeasure = extraParam = false;
    sdi12add[0] = address;
    sdi12add[1] = crcASCII[3] = '\0';

    // CRC-16-IBM Polynomial 0xA001 final XOR = 0 Bits reflected
    crc = CRC16(0xA001, 0, 0, false, false);
    flash = DueFlashStorage();
  };
  int Parse(char *buffer, u_int8_t commandlength);
  DueFlashStorage flash; // I hate this leaky abstraction, I don't have the
                         // energy to fix it. ಠ╭╮ಠ
  void SetAdd(char add);

private:
  char sdi12add[2], crcASCII[4], outputbuf[200];
  bool target, adms, extraParam, readingAvaliable, crcReq, contMeasure;
  int retval;
  CRC16 crc;
  airoutput airReading;
  uint16_t luxReading;
  void refresh_vals();
  void flush_sdi12();
  void CalcCrc();
};

void Parser::refresh_vals() {
  luxReading = sensors.lightmeter.readLightLevel(); // ick
  airReading = sensors.airreading();
}
void Parser::SetAdd(char add) {
  // run this if the address is different from the default or not first run.
  sdi12add[0] = add;
}
void Parser::CalcCrc() {
  /*
   Page 21 Ver 1.4 SDI-12 standard 4.4.12.2
   At first this was harder than I was expecting, after 3 days I realised that I
   had not used bitwise operations but rather logical ones... It seems easy now.
   ¯\_( ͡° ͜ʖ ͡°)_/¯
  */
  crc.add((uint8_t *)outputbuf, strlen(outputbuf));
  uint16_t crcraw = crc.getCRC();
  crcASCII[0] = 0x40 | (crcraw >> 12);
  crcASCII[1] = 0x40 | ((crcraw >> 6) & 0x3F);
  crcASCII[2] = 0x40 | (crcraw & 0x3F);
  crc.restart();
}
void Parser::flush_sdi12() {
  SEND();
  if (crcReq) {
    sdi12.print(outputbuf);
    CalcCrc();
    sdi12.println(crcASCII);
  } else {
    sdi12.println(outputbuf);
  }

  CLR12();
}
int Parser::Parse(char *buffer, u_int8_t commandlength) {
  /*
      Excluding the break command, we should have full SDI-12 Ver 1.4 Feb 20,
     2023 compliance. Break command would end up doing nothing anyway, as it
     cancels a measurement, but this is only the case if a service request
     exists additionally the device cannot sleep anyway due to it's other
     functions. Service requests could be added by keeping a request stack.
     TODO: Fix this mess of returns
  */
  memset(outputbuf, 0, sizeof(outputbuf));
  target = buffer[0] == sdi12add[0];

  if ((commandlength == 1 && target) ||
      (commandlength == 1 && buffer[0] == '?')) // Trival commands a! ?!
  {
    sprintf(outputbuf, "%s", sdi12add);
    flush_sdi12();
    return EXIT_SUCCESS;
  }

  if (target && commandlength > 1) {
    switch (buffer[1]) {
    case 'V': {
      struct mallinfo mi = mallinfo();

      char *heapend = sbrk(0);
      register char *stack_ptr asm("sp");
      sprintf(outputbuf, "%s+RAMFREE: %d", sdi12add,
              stack_ptr - heapend + mi.fordblks);
      flush_sdi12();
    } break;

    case 'R': {
      // Continous measurement support
      SEND();
      int select;
      crcReq = buffer[2] == 'C';
      refresh_vals();
      /*
       Shift which character we are checking depending if CRC is requested.
       Then we will select our sensor for instantaneous measurement
      */
      if (crcReq) {
        select = buffer[3] - '0';
      } else {
        select = buffer[2] - '0';
      }
      switch (select) {
      case 1: {
        sprintf(outputbuf, "%s+%u", sdi12add, luxReading);
      } break;
      case 2: {
        sprintf(outputbuf, "%s+%.2f", sdi12add, airReading.temp);
      } break;
      case 3: {
        sprintf(outputbuf, "%s+%lu", sdi12add, airReading.pressure);
      } break;
      case 4: {
        sprintf(outputbuf, "%s+%.2f", sdi12add, airReading.humidity);
      } break;
      case 5: {
        sprintf(outputbuf, "%s+%lu", sdi12add, airReading.gasresistance);
      } break;
      case 0: // Some sdi-12 dataloggers are indexed with zero.
      default: {
        sprintf(outputbuf, "%s", sdi12add);
      }
      }
      flush_sdi12();
      break;
    };
    case 'C':
      contMeasure = !contMeasure; // Fall through case, note SDI-12 sensor will
                                  // operate in concurrent measurement mode till
                                  // this command is called again.
    case 'M': {
      // Supports Measurements + concurrent ones
      // Command is basically the same in implementation.

      // Lets check if CRC has been requested
      crcReq = buffer[2] == 'C';
      /*
          lets check if aM1!..aM9 required for ver 1.2 SDI-12 compliance
          Not implemented, not going to be.... Still compliant due to response
          If command length differs due to CRC request adjust for correct input;
          TODO: sanitize input
      */
      adms = false;
      if ((crcReq && commandlength >= 4) || (commandlength >= 3 && !crcReq)) {
        adms = !adms; // Additional measurement pages
      }
      /*  Full Compliance aM! aMC!  aM(0-9)! aMC(0-9)! aC! aCC! aC(0-9)!
         aCC(0-9)!. ttt is zero no service request is required to indicate the
         sensor is ready for reading Yay! 4.4.6 Service request compliance
         (*˘︶˘*)
      */
      if (adms) {
        sprintf(outputbuf, "%s0000", sdi12add);
      } else {
        //  This triggers sensor reading
        //  and placement into sending buffer
        refresh_vals();
        readingAvaliable = true; // double ick
        sprintf(outputbuf, "%s0005", sdi12add);
      }
      flush_sdi12();

      if (commandlength == 3) {
        // Not implemented, not going to be, this will be optimised out as it is
        // a dead branch.
      }
    } break;
    case 'D': {
      if (commandlength == 3) {
        /*
           We should check the page here but seeing as we have one page assume
           that it is 0. This is technically non compliant,but this behaviour is
           acceptible due to one page existing. Not implemented, not going to
           be, this will be optimised out as it is a dead branch. ╭( ๐ _๐)╮

           This can be implemented with ease in the future.
        */
      }

      if (commandlength == 3 && readingAvaliable) {
        readingAvaliable = false;
        if (contMeasure) {
          refresh_vals();
          // contMeasure = false;
        }
        sprintf(outputbuf, "%s+%u+%.2f+%u+%.2f+%lu", sdi12add, luxReading,
                airReading.temp, airReading.pressure, airReading.humidity,
                airReading.gasresistance);
        flush_sdi12();
      }
    } break;
    case 'H': {
      /*
       5.4 Compliance with High-vol commands.
       ver 1.4, sensors that do not support this command must return zero data
       values ready.
      */

      sprintf(outputbuf, "%s000000", sdi12add);
      flush_sdi12();
      return EXIT_SUCCESS;
    } break;

    case 'A': {
      if (isprint(buffer[2])) {
        sdi12add[0] = buffer[2];
        /*
            Now we need to trigger a change in config, for now this is the only
           thing we are changing, in the future we should use a struct we must
           disable all interrupts while writing to flash mem, otherwise we will
           murder our flash page!
            ("En Garde!")> ╰༼.◕ヮ ◕.༽つ¤=[]——————  ⋌༼ •̀ ⌂ •́ ༽⋋ <("Can I get
           workers comp?") 1 = Disable all interrupts 0 = Enable all interrupts
        */
        __set_FAULTMASK(1);
        flash.write(1, sdi12add[0]);
        __set_FAULTMASK(0);
        sprintf(outputbuf, "%s", sdi12add);
        flush_sdi12();
      }
    } break;
    case 'X':
      rstc_start_software_reset(
          RSTC); // REQUEST_EXTERNAL_RESET TODO: Add more extended functions,
                 // place this into a subsection
      break;     // Never reached.
    case 'I': {

      crcReq = buffer[3] == 'C';
      // adms = false;
      extraParam = false;
      int param = -1;
      if (commandlength == 7 || commandlength == 8) {
        extraParam = !extraParam;
        param = buffer[commandlength - 1] - '0';
        // Serial.println(param);

      } // Check if we are looking for parameters
      /*
       This should filter aIC(0-9)! aICC(0-9)! aIM(0-9) aIMC(0-9) aIHA aIHB aI A
       real shame regex is not suitible in this enviroment Now we need to avoid
       false positives from aIM_000! and aIMC_000! type commands Problem is that
       this can still have false negatives, if an actor is attempting to break
       the system this would be a suitible entry point. Good thing it is out of
       scope, anyway if an actor has access to the SDI-12 line then its in a
       insecure enviroment to begin with. How am I going to fix the false
       negatives? what A mess...
       TODO: Replace this steaming pile
      */

      if (((buffer[4] != '_' && crcReq && commandlength >= 5) ||
           (commandlength >= 4 && !crcReq && buffer[3] != '_')) &&
          ((buffer[2] != 'R') && (buffer[2] != 'D'))) {
        sprintf(outputbuf, "%s0000", sdi12add);
        flush_sdi12();
        break;
      }
      Serial.println(commandlength);
      if (commandlength == 2) {
        // Stop early if it is not a metadata command
        sprintf(outputbuf, "%s14SWINBURNSDIBRG001", sdi12add);
        flush_sdi12();
        break;
      }

      switch (buffer[2]) {
      case 'H': {
        sprintf(outputbuf, "%s000000", sdi12add);
      } break;
      case 'M':
      case 'C': {
        if (0 < param && param <= 5) {
          sprintf(outputbuf, "%s,%s,%s;", sdi12add, paramdata[param - 1].shef,
                  paramdata[param - 1].units);
        } else {
          sprintf(outputbuf, "%s0005", sdi12add);
        }
      } break;
      default: {
      } break;
      }
      flush_sdi12();    
    } break;
    }
  }
  return retval;
}

#endif