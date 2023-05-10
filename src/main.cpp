#include "../lib/commands.h"
#include "../lib/datalog.h"
#include "../lib/menus.h"
#include "../lib/parser.h"
#include "../lib/sensors.h"
#include <Adafruit_GFX.h> //Required for function prototypes inpractise superceeded by Arduino_GFX
#include <Adafruit_ST7735.h>
#include <Arduino.h>
#include <DueFlashStorage.h>
#include <SPI.h>
#include <Wire.h>
/*
  Intergration by completed by Phillip Weinstock
*/

char sdi12buf[200];
CommandLine sdiBuf = CommandLine(&sdi12, sdi12buf, sizeof(sdi12buf), 7);

struct Config {
  char address;

} first_run;

void setup() {
  first_run.address = '0';
  /*
   First time run?
   set val@add0 to zero, where first run will be 255
   */
  DueFlashStorage fs;
  uint8_t firstRun = fs.read(0);
  if (firstRun) {
    fs.write(1, first_run.address);
    fs.write(0, 0);
    rtc.setClock(__TIME__, __DATE__);
    sdi12_sensor_setup(first_run.address);
  } else {
    char saved = (char)fs.read(1);
    sdi12_sensor_setup(saved);
  }
  Wire1.begin();
  Wire.begin();
  sensors.init();
  Serial.begin(9600);
  sdi12.begin(1200, SERIAL_7E1);

  Menuinit();
  SD_init();
  SENDF;
  CLR12();
  RECEIVEF;
}
uint32_t last_log = 0;
void loop() {
  if (sdiBuf.serialread() && sdiBuf.readchars != 0) {
    Serial.println("Recieved command");
    Serial.println(sdi12buf);
    sdi12_sensor_handle(sdi12buf, sdiBuf.readchars);
    sdiBuf.resetstatus();
    memset(sdi12buf, 0, sizeof(sdi12buf));
    RECEIVEF;
  }
  if (!eject_sd) {
    if (last_log + 10000 <= millis()) // Every 10 seconds
    {
      last_log = millis();
      Serial.println("Sensor logged");
      logsensordata();
    }

  } else {
    SD_eject();
  }

  // updateMenu(millis());

  // Serial.println(sensors.airreading().temp);
  ;
}