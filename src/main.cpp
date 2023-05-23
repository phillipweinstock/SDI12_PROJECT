#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h> //Required for function prototypes inpractise superceeded by Arduino_GFX
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <DueFlashStorage.h>
#include "../lib/commands.h"
#include "../lib/parser.h"
#include "../lib/sensors.h"
#include "../lib/menus.h"
#include "../lib/datalog.h"

char sdi12buf[200];
CommandLine sdiBuf = CommandLine(&sdi12, sdi12buf, sizeof(sdi12buf), 7);
Parser commandHandler = Parser('0');

struct Config
{
  char address;

} first_run;

void setup()
{
  first_run.address = '0';
  /*
   First time run?
   set val@add0 to zero, where first run will be 255
   */
  uint8_t firstRun = commandHandler.flash.read(0);
  if (firstRun)
  {

    commandHandler.flash.write(1, first_run.address);
    commandHandler.flash.write(0, 0);
    rtc.setClock(__TIME__, __DATE__);
  }
  else
  {
    commandHandler.setadd(commandHandler.flash.read(1));
  }
  Wire1.begin();
  Wire.begin();
  sensors.init();
  Serial.begin(9600);
  sdi12.begin(1200, SERIAL_7E1);
  SD_init();
  Menuinit();
  // tft.initR(INITR_BLACKTAB);
  // tft.setRotation(3);
  // tft.fillScreen(ST77XX_BLACK);
  SENDF;
  CLR12();
  RECEIVEF;
}
uint32_t last_log = 0;
void loop()
{
  if (sdiBuf.serialread() && sdiBuf.readchars != 0)
  {
    Serial.println("Recieved command");
    Serial.println(sdi12buf);
    commandHandler.parse(sdi12buf, sdiBuf.readchars);
    sdiBuf.resetstatus();
    memset(sdi12buf, 0, sizeof(sdi12buf));
    RECEIVEF;
  }
  if (!eject_sd)
  {
    if (last_log + 10000 <= millis())//Every 10 seconds
    {
      last_log = millis();
      Serial.println("Sensor logged");
      logsensordata();
    }
    
  }
  else
  {
    eject_SD();
  }

  // updateMenu(millis());

  // Serial.println(sensors.airreading().temp);
  ;
}