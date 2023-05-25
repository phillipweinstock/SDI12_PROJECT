// This is Mike Library
// This Library is mainly use for checking SD card intallation process and Store
// The Value which achieved from the Sensor to Data Log File
#ifndef DATALOG_H
#include "sensors.h"
#include <Adafruit_I2CDevice.h>
#include <Arduino.h>
#include <RTCDue.h>
#include <RTClib.h>
#include <SPI.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <Wire.h>


#if SPI_DRIVER_SELECT == 2 // Must be set in SdFat/SdFatConfig.h  <----DO--NOT--FORGET!!!!---------
// DO THIS BEFORE YOU COMPILE, OTHERWISE THE PROJECT WON'T WORK

// SD_FAT_TYPE = 0 for SdFat/File as defined in SdFatConfig.h,
// 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#define SD_FAT_TYPE 0

// Chip select may be constant or RAM variable.
const uint8_t SD_CS_PIN = A3;

// Pin numbers in templates must be constants.
const uint8_t SOFT_MISO_PIN = 12;
const uint8_t SOFT_MOSI_PIN = 11;
const uint8_t SOFT_SCK_PIN = 13;

// SdFat software SPI template
SoftSpiDriver<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> softSpi;
// Speed argument is ignored for software SPI.
#if ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(0), &softSpi)
#else // ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(0), &softSpi)
#endif // ENABLE_DEDICATED_SPI

#if SD_FAT_TYPE == 0
SdFat sd;
File file;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 file;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile file;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile file;
#else // SD_FAT_TYPE
#error Invalid SD_FAT_TYPE
#endif // SD_FAT_TYPE
#endif

/* Create an rtc object and select Slowclock source

    This is not idea and is just for demo purposes, I cannot get the external
   rtc to work on the due, gd knows why...
 */
// RTCDue rtc(RC);
RTCDue rtc(XTAL);
enum SD_settings { SD_CS = A3, SD_MISO = 12, SD_SCLK = 13, SD_MOSI = 11 };
// RTC_DS1307 rtc; I'm not in the mood for faulty hardware I don't know
// Im more in the mood for faulty hardware I do know
File logfile;
bool eject_triggered_previously = false;
char logbuffer[200];
void logsensordata() {
  airoutput output = sensors.airreading();
  Lightoutput lightoutput = sensors.lightreading();
  sprintf(logbuffer, "%u,%2.2f,%2.2f,%u,%u,%u", rtc.unixtime(), output.temp,
          output.humidity, output.pressure, lightoutput.lux,
          output.gasresistance);
  logfile.println(logbuffer);
  logfile.flush();
}

/*
    The SD eject is needed so we can make sure no data is being written to the
   card Thus avoid corrupted files Once The SD is ejected the board will need to
   reboot start the datalog again.
*/

void SD_eject() {
  if (eject_triggered_previously) {
    return;
  }
  logfile.close();
  sd.end();
  eject_triggered_previously = true;
}
void SD_init() {
  rtc.begin();

  if (!sd.begin(SD_CONFIG)) {
    sd.initErrorHalt();
  }
#if DEBUG
  Serial.println(F("Initialization done."));
#endif
  logfile = sd.open("sensor_log.CSV", O_RDWR | O_CREAT | O_SYNC);
  if (!logfile) {
    sd.errorHalt(F("open failed"));
  }
  if (logfile.size() == 0) {
#if DEBUG
    Serial.println(F("Writing CSV header"));
#endif
    logfile.println(F("EPOCH,Temperature,Humidity,Pressure,Light_intensity,Gas_"
                      "Resistance")); // make sure that the coloumbs headder are
                                      // alawsys there
    logfile.flush();
    delay(10);
  }
}

#endif
