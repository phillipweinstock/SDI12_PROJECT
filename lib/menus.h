#ifndef MENUS_H
#define MENUS_H
#include <Wire.h>
// #include <Adafruit_GFX.h> // Cor
#include <Arduino_GFX.h>
#include <Arduino_GFX_Library.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Arduino.h>
#include "..\lib\sensors.h"
#define PB_1 2
#define PB_2 3
#define PB_3 4
#define PB_4 5
enum display_settings
{
    TFT_CS = 10,
    TFT_RST = 6,
    TFT_DC = 7,
    TFT_SCLK = 13,
    TFT_MOSI = 11
};
struct statem
{
    int flag;
    unsigned long interval;
    unsigned long p_time;
    unsigned long c_time;
};

struct statem delay1, delay2;
volatile int previous_state;
volatile int loop_state;
char lux[5]; //= 0;
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
Arduino_DataBus *bus = new Arduino_SWSPI(TFT_DC,TFT_CS,TFT_SCLK,TFT_MOSI,TFT_RST);
Arduino_GFX *tft = new Arduino_ST7735(bus, -1 /* RST */, 0 /* rotation */, false /* IPS */,
    128 /* width */, 160 /* height */, 0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col offset 2 */, 0 /* row offset 2 */);
char temperature[15];
char pressure[15];
char humidity[15];
char gas[15];
char resistance[15];
//char light[5];
airoutput airinfo;

void WriteLightVal(uint16_t Color){
        tft->setTextSize(2);
        tft->setCursor(5, 30);
        tft->setTextColor(Color);
        tft->print(lux);
}
void WriteAirQVal(uint16_t Color){
        tft->setCursor(80, 40);
        tft->setTextColor(Color);
        tft->print(temperature);
        tft->setCursor(60, 60);
        tft->setTextColor(Color);
        tft->print(pressure);
        tft->setCursor(60, 80);
        tft->setTextColor(Color);
        tft->print(humidity);
        tft->setCursor(30, 100);
        tft->setTextColor(Color);
        tft->print(resistance);
}

enum
{
    WRITE,
    MASK,
    STOP
};
enum
{
    MENU,
    LIGHT,
    AIRQ
};



void clsscreen()
{
    if (previous_state != loop_state)
    {
        tft->fillScreen(ST7735_BLACK);
        loop_state = previous_state;
    }
}
void eject_SD()
{
     //clsscreen();
    detachInterrupt(PB_3);
    tft->fillScreen(ST77XX_RED);
    attachInterrupt(PB_3, eject_SD, LOW);
}
void LightSensor()
{
    // clsscreen();
    if ((delay1.p_time + delay1.interval <= delay1.c_time))
    {                                  // WE ARE CHECKING THE INTERVAL AND MAKING SURE WE ARE ALREADY IN A STATE OF WAITING
        delay1.flag += 1;              // W E NOW SWITCH STATE
        delay1.p_time = delay1.c_time; // RESET OUR TIMER
        if (delay1.flag > STOP)
        {
            delay1.flag = WRITE;
        }
    }
    if (delay1.flag == WRITE)
    {
        
        //if (lux <= -2)
        //{
            // lightMeter.
        
        WriteLightVal(ST7735_WHITE);
    }
    if (delay1.flag == MASK)
    {
       WriteLightVal(ST7735_BLACK);
       sprintf(lux,"%2.2f",sensors.lightmeter.readLightLevel()); //= sensors.lightmeter.readLightLevel();
    }
}
void Light_disp()
{
    detachInterrupt(PB_4);
    //tft->fillScreen(ST7735_BLACK);
     //clsscreen();
    loop_state = LIGHT;
    //tft->fillScreen(ST7735_BLACK);
    tft->setCursor(5, 10);
    tft->setTextColor(ST77XX_YELLOW);
    tft->setTextSize(2);
    tft->print(F("Light Value: "));
    attachInterrupt(PB_4, Light_disp, LOW);
}
void Disp_menu()
{
    detachInterrupt(PB_1);
    //tft->fillScreen(ST7735_BLACK);
     //clsscreen();
    loop_state = MENU;
    //tft->fillScreen(ST7735_BLACK);
    tft->setCursor(55, 8);
    tft->setTextColor(ST77XX_YELLOW);
    tft->setTextSize(2);
    tft->print(F("MENU"));
    tft->setCursor(5, 40);
    tft->setTextColor(ST77XX_WHITE);
    tft->setTextSize(1);
    tft->print(F("(1) View this menu \n"));
    tft->setCursor(5, 60);
    tft->setTextColor(ST77XX_WHITE);
    tft->setTextSize(1);
    tft->print(F("(2) AirQ sensor reading \n"));
    tft->setCursor(5, 80);
    tft->setTextColor(ST77XX_WHITE);
    tft->setTextSize(1);
    tft->print(F("(3) Eject SD card \n"));
    tft->setCursor(5, 100);
    tft->setTextColor(ST77XX_WHITE);
    tft->setTextSize(1);
    tft->print(F("(4) Light sensor reading \n"));
    attachInterrupt(PB_1, Disp_menu, LOW);
};
void AirSense()
{

    // clsscreen();
    tft->setTextSize(1);

    if ((delay2.p_time + delay2.interval <= delay2.c_time))
    {
        delay2.flag += 1;              // W E NOW SWITCH STATE
        delay2.p_time = delay2.c_time; // RESET OUR TIMER
        if (delay2.flag > STOP)
        {
            delay2.flag = WRITE;
        }
    }
    if (delay2.flag == WRITE)
    {

       WriteAirQVal(ST7735_WHITE);
    }
    if (delay2.flag == MASK)
    {
       WriteAirQVal(ST7735_BLACK);
        airinfo = sensors.airreading();
        sprintf(pressure, "%u", airinfo.pressure);
        sprintf(temperature, "%2.2f", airinfo.temp);
        sprintf(humidity, "%2.2f", airinfo.humidity);
        sprintf(resistance, "%lu", (unsigned long)airinfo.gasresistance);
    }
}
void Airq()
{
    detachInterrupt(PB_2);
    loop_state = AIRQ;
    //clsscreen();
    tft->setCursor(15, 10);
    tft->setTextColor(ST77XX_CYAN);
    tft->setTextSize(2);
    tft->print(F("AirQ Sensor"));
    tft->setCursor(5, 40);
    tft->setTextColor(ST77XX_YELLOW);
    tft->setTextSize(1);
    tft->print(F("Temperature: "));
    tft->setCursor(115, 40);
    tft->print(F("C"));
    tft->setCursor(5, 60);
    tft->setTextColor(ST77XX_YELLOW);
    tft->setTextSize(1);
    tft->print(F("Pressure: "));
    tft->setCursor(105, 60);
    tft->print(F("kPa"));
    tft->setCursor(5, 80);
    tft->setTextColor(ST77XX_YELLOW);
    tft->setTextSize(1);
    tft->print(F("Humidity: "));
    tft->setCursor(100, 80);
    tft->print(F("%"));
    tft->setCursor(5, 100);
    tft->setTextColor(ST77XX_YELLOW);
    tft->setTextSize(1);
    tft->print(F("Gas: "));
    tft->setCursor(65, 100);
    tft->print(F("kOhms"));
    attachInterrupt(PB_2, Airq, LOW);
}
extern void Menuinit()
{
    previous_state = loop_state = 0;
    delay1 = {WRITE, 1, 0, 0};
    delay2 = {WRITE, 1, 0, 0};
    pinMode(PB_1, INPUT_PULLUP);
    pinMode(PB_2, INPUT_PULLUP);
    pinMode(PB_3, INPUT_PULLUP);
    pinMode(PB_4, INPUT_PULLUP);

    delay1.p_time = delay1.c_time = millis();
    delay1.flag = WRITE;

    //tft->initR(INITR_BLACKTAB);
    tft->begin();
    tft->setRotation(3);
    tft->fillScreen(ST77XX_BLACK);
    tft->setCursor(7, 10);
    tft->setTextColor(ST77XX_GREEN);
    tft->setTextSize(1);
    attachInterrupt(PB_1, Disp_menu, LOW);
    attachInterrupt(PB_2, Airq, LOW);
    attachInterrupt(PB_3, eject_SD, LOW);
    attachInterrupt(PB_4, Light_disp, LOW);
    loop_state = MENU;
     //tft->print("Welcome! press button 1 \n to view the menu");
}
extern void updateMenu(uint32_t current_time)
{
    delay1.c_time = delay2.c_time = current_time;
    
    switch (loop_state)
    {
    case MENU:
    {
        Disp_menu();
    }
    break;
    case LIGHT:
    {

        LightSensor();
    }
    break;
    case AIRQ:
    {

        AirSense();
    }
    break;
    default:
    {
        //clsscreen();
    }
    break;
    };
}

#endif