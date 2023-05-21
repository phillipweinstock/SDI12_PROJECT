#ifndef MENUS_H
#define MENUS_H
#include <Wire.h>
#include <Adafruit_GFX.h> // Cor
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Arduino.h>
#include "..\lib\sensors.h"
#define PB_1 2
#define PB_2 3
#define PB_3 4
#define PB_4 5
    char temperature[15]; // String(0);
    char pressure[15]; // String(0);
    char humidity[15]; // String(0);
    char gas[15];  //String(0);
enum display_settings
{
    TFT_CS = 10,
    TFT_RST = 6,
    TFT_DC = 7,
    TFT_SCLK = 13,
    TFT_MOSI = 11
};
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
uint16_t lux = 0;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
void clsscreen()
{
    if (previous_state != loop_state)
    {
        tft.fillScreen(ST7735_BLACK);
        loop_state = previous_state;
    }
}
void eject_SD()
{
    //clsscreen();
    tft.fillScreen(ST77XX_RED);
}
void LightSensor()
{
     //clsscreen();
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
        lux = sensors.lightmeter.readLightLevel();
        if (lux <= -2)
        {
            // lightMeter.
        }
        tft.setTextSize(2);
        tft.setCursor(5, 30);
        tft.setTextColor(ST77XX_WHITE);
        tft.print(lux);
    }
    if (delay1.flag == MASK)
    {
        tft.setTextSize(2);
        tft.setCursor(5, 30);
        tft.setTextColor(ST77XX_BLACK);
        tft.print(lux);
    }
}
void Light_disp()
{
    tft.fillScreen(ST7735_BLACK);
     //clsscreen();
    loop_state = LIGHT;
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(5, 10);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(2);
    tft.print("Light Value: ");
}
void Disp_menu()
{
    tft.fillScreen(ST7735_BLACK);
     //clsscreen();
    loop_state = MENU;
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(55, 8);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(2);
    tft.print("MENU");
    tft.setCursor(5, 40);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.print("(1) View this menu \n");
    tft.setCursor(5, 60);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.print("(2) AirQ sensor reading \n");
    tft.setCursor(5, 80);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.print("(3) Eject SD card \n");
    tft.setCursor(5, 100);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.print("(4) Light sensor reading \n");
};
void AirSense()
{

    //clsscreen();
    tft.setTextSize(1);

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

        tft.setCursor(80, 40);
        tft.setTextColor(ST77XX_WHITE);
        tft.print(temperature);
        tft.setCursor(60, 60);
        tft.setTextColor(ST77XX_WHITE);
        tft.print(pressure);
        tft.setCursor(60, 80);
        tft.setTextColor(ST77XX_WHITE);
        tft.print(humidity);
        tft.setCursor(30, 100);
        tft.setTextColor(ST77XX_WHITE);
        tft.print(gas);
    }
    if (delay2.flag == MASK)
    {
        tft.setTextSize(1);
        tft.setCursor(60, 60);
        tft.setTextColor(ST77XX_BLACK);
        tft.print(pressure);
        tft.setTextSize(1);
        tft.setCursor(80, 40);
        tft.setTextColor(ST77XX_BLACK);
        tft.print(temperature);
        tft.setTextSize(1);
        tft.setCursor(60, 80);
        tft.setTextColor(ST77XX_BLACK);
        tft.print(humidity);
        tft.setTextSize(1);
        tft.setCursor(30, 100);
        tft.setTextColor(ST77XX_BLACK);
        tft.print(gas);
    }
}
void Airq()
{
    tft.fillScreen(ST7735_BLACK);
    // clsscreen();
    loop_state = AIRQ;
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(15, 10);
    tft.setTextColor(ST77XX_CYAN);
    tft.setTextSize(2);
    tft.print("AirQ Sensor");
    tft.setCursor(5, 40);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(1);
    tft.print("Temperature: ");
    tft.setCursor(115, 40);
    tft.print("C");
    tft.setCursor(5, 60);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(1);
    tft.print("Pressure: ");
    tft.setCursor(105, 60);
    tft.print("kPa");
    tft.setCursor(5, 80);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(1);
    tft.print("Humidity: ");
    tft.setCursor(100, 80);
    tft.print("%");
    tft.setCursor(5, 100);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(1);
    tft.print("Gas: ");
    tft.setCursor(65, 100);
    tft.print("kOhms");
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

    tft.initR(INITR_BLACKTAB);
    tft.setRotation(3);
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(7, 10);
    tft.setTextColor(ST77XX_GREEN);
    tft.setTextSize(1);
    attachInterrupt(PB_2, Airq, LOW);
    attachInterrupt(PB_1, Disp_menu, LOW);
    attachInterrupt(PB_3, eject_SD, LOW);
    attachInterrupt(PB_4, Light_disp, LOW);
    tft.print("Welcome! press button 1 \n to view the menu");
}
extern void updateMenu(){
delay1.c_time = delay2.c_time = millis();
  switch (loop_state)
  {
  case MENU:
  {
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
  }
  break;
  };

}

#endif