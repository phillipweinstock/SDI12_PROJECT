#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <BH1750.h>
#include <Adafruit_GFX.h> // Cor
#include <Adafruit_ST7735.h>
#include <SPI.h>
// #include <lilParser.h>
#include "../lib/commands.h"
#define sdi12 Serial1
#define SEND digitalWrite(7, LOW)
#define RECIEVE digitalWrite(7, HIGH)
#define RECEIVEF REG_PIOC_SODR |=(0x1 <<23);
#define SENDF REG_PIOC_CODR |=(0x1 <<23); 
//DIRTY BIT HACKING
//#define SENDF(b) ( (b)<8 ? PORTD |=(1<<(b)) : PORTB |=(1<<(b-8))             )
//#define RECIEVEF(b) ( (b)<8 ? PORTD &=~(1<<(b)) : PORTB &=~(1<<(b-8))             )
#define TFT_CS 10
#define TFT_RST 6
#define TFT_DC 7
#define TFT_SCLK 13
#define TFT_MOSI 11

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
char sdi12buf[200];
char sdi12add[2] = "a";
bool target = false;
CommandLine sdiBuf = CommandLine(&sdi12, sdi12buf, sizeof(sdi12buf), 7);
// Adafruit_BME680 bme;     // create BME680 object
// BH1750 lightMeter(0x23); // create BH1750 object with I2C address

// lilParser parser;

void setup()
{
  // parser.addCmd(adQry, "?");
  //  put your setup code here, to run once:
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  Serial.begin(9600);
  // pinMode(7, OUTPUT);
  Serial.println("PRINT TO SERIAL");
  sdi12.begin(1200, SERIAL_7E1);
  SEND;
  sdi12.println("HELLOSDI12");
  delay(100);
  RECIEVE;
}

void loop()
{
  // put your main code here, to run repeatedly:

  // SEND;
  // sdi12.println(" banana");
  // delay(15);
  // target = false;

  if (sdiBuf.serialread() && sdiBuf.readchars != 0)
  {
    sdiBuf.resetstatus();
    target = sdi12buf[0] == sdi12add[0];
    Serial.println("Command recieved");
    Serial.println(sdi12buf);
    // Serial.print("First char: ");Serial.println(sdi12buf[0]);
    // Serial.print("second char: ");
    // Serial.println(sdi12buf[1]);
    Serial.print("Chars read:");
    Serial.println(sdiBuf.readchars);
    Serial.println(((target) ? "We are the target" : "We are not the target"));
    if (sdiBuf.readchars == 1)
    {
      if (target)
      {
        Serial.println("Acknowledge Active");
        SEND;
        // delay(100);
        sdi12.println(sdi12add); // VERY WEIRD
        delay(20);
        // RECIEVE;
      }

      if (sdi12buf[0] == '?')
      {
        Serial.println("Address Query");
        SEND;
        sdi12.println(sdi12add);
        delay(100);
      } // Address Query
    }
    if (target && sdiBuf.readchars >= 2)
    {
      Serial.println("More advanced commands");
      switch (sdi12buf[1])
      {
      case 'I':
      {
        Serial.println("Send Identification");
        //SEND;
        //SENDF(7);
        //delay(100);
        SENDF // 2.4ns
        delay(5);
        sdi12.print(sdi12add);
        sdi12.print("01SWINBURNSDIBRG001\r\n");
        
        sdi12.flush();
        sdi12.
        //sdi12.end();
        //sdi12.begin(1200,SERIAL_7E1);
      //  delay(100);
      }
      break;
      case 'M':
      {
        Serial.println("Start measurement");
        SEND;
        sdi12.print(sdi12add);
        sdi12.println("0022");
        delay(100);
        // This is where we will have a call back function to the sensor code
      }
      break;
      case 'D':
      {
        // if measurement ready return formatted string to sdi12 line
        // check if parameter is set up correctly
        Serial.println("Send Data");
        if (sdiBuf.readchars != 3)
        {
          Serial.println("Not enough parameters");
          break; // This means the command is trash
        }
        int selected_page = sdi12buf[2] - '0';
      }
      }
    }

    // if(sdi12buf[0] == sdi12add[0] ){
    //   target = true;
    // }
    // if(target && sdiBuf.readchars ==1){
    //   SEND;
    //   sdi12.print(' ');
    //   sdi12.print(sdi12add);
    //   delay(10);
    //   Serial.print("Ack");
    //   sdi12.print(F("\r\n"));
    //   delay(10);
    memset(sdi12buf, 0, sizeof(sdi12buf));
  } // ACK Active command
  RECEIVEF;
  // Serial.println("Command recieved");
  // Serial.println(sdi12buf);
  // Serial.print("First char: ");Serial.println(sdi12buf[0]);
  // Serial.print("second char: ");Serial.println(sdi12buf[1]);
  // Serial.print("Chars read:");Serial.println(sdiBuf.readchars);
  // memset(sdi12buf,0, sizeof(sdi12buf));
}