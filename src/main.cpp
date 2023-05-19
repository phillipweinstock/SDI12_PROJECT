#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <BH1750.h>
#include <Adafruit_GFX.h> // Cor
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "../lib/commands.h"
#include "../lib/parser.h"
#include "../lib/sensors.h"

#define TFT_CS 10
#define TFT_RST 6
#define TFT_DC 7
#define TFT_SCLK 13
#define TFT_MOSI 11

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
char sdi12buf[200];

bool target = false;
CommandLine sdiBuf = CommandLine(&sdi12, sdi12buf, sizeof(sdi12buf), 7);
Parser commandHandle = Parser('0');
// Adafruit_BME680 bme;     // create BME680 object
// BH1750 lightMeter(0x23); // create BH1750 object with I2C address

// lilParser parser;

void setup()
{
  
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  Wire1.begin();
  sensors.init();
  Serial.begin(9600);
  Serial.println("PRINT TO SERIAL");
  sdi12.begin(1200, SERIAL_7E1);
  SENDF;
  sdi12.println("HELLOSDI12");
  //delay(50);
  
  CLR12();
  RECEIVEF;
 // scanI2C();
}

void loop()
{
 if (sdiBuf.serialread() && sdiBuf.readchars != 0)
  {
    Serial.println("Recieved command");
    Serial.println(sdi12buf);
    commandHandle.parse(sdi12buf,sdiBuf.readchars);
    sdiBuf.resetstatus();
    memset(sdi12buf, 0, sizeof(sdi12buf));
    RECEIVEF;
    }

}