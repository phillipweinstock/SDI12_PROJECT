#ifndef MENUS_H
#define MENUS_H

/* the following section defines the required libreries
for the menu code to work and also includes a link back
to the sensor reading code in order to diplay sensor
readings as part of the menu option */

#include "sensors.h"
#include <Adafruit_ST7735.h>
#include <Arduino.h>
#include <Arduino_GFX.h>
#include <Arduino_GFX_Library.h>
#include <DueTimer.h>
#include <SPI.h>
#include <Wire.h>


/* the following section defines the required digital
pins on the arduino to be used to trigger differnt menu
options further in the code */

#define PB_1 2
#define PB_2 3
#define PB_3 4
#define PB_4 5

/* The following 'enum' defines the pin numbers for various settings of the TFT
display. Each setting is assigned a constant value (pin)using the enum. These
definitions are used to configure and control the TFT display in the code:

- `TFT_CS`: The pin number for the chip select (CS) pin of the TFT display. It
is assigned pin 10.
- `TFT_RST`: The pin number for the reset (RST) pin of the TFT display. It is
assigned pin 6.
- `TFT_DC`: The pin number for the data/command (DC) pin of the TFT display. It
is assigned pin 7.
- `TFT_SCLK`: The pin number for the serial clock (SCLK) pin of the TFT display.
It is assigned pin 13.
- `TFT_MOSI`: The pin number for the serial data (MOSI) pin of the TFT display.
It is assigned pin 11.
 */

enum display_settings {
  TFT_CS = 10,
  TFT_RST = 6,
  TFT_DC = 7,
  TFT_SCLK = 13,
  TFT_MOSI = 11
};

/* These lines of code create the necessary objects for the TFT display,
enabling communication and interaction with the display through the defined
bus and display objects (bus and tft, respectively). */

Arduino_DataBus *bus =
    new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_RST);
Arduino_GFX *tft = new Arduino_ST7735(
    bus, -1 /* RST */, 0 /* rotation */, false /* IPS */, 128 /* width */,
    160 /* height */, 0 /* col offset 1 */, 0 /* row offset 1 */,
    0 /* col offset 2 */, 0 /* row offset 2 */);

/* These character arrays provide storage space to hold the sensor data as
strings, allowing for further processing or display on the TFT display. The
lengths of the arrays determine the maximum number of characters that can be
stored in each array. */

char lux[5];
char temperature[15];
char pressure[15];
char humidity[15];
char gas[15];
char resistance[15];
airoutput airinfo = {0, 0, 0, 0};
bool eject_sd; // STUPID GLOBAL VARIABLE

/* These two functions are responsible for writing sensor values to the TFT
display. These functions provide a convenient way to display sensor readings on
the TFT display using the specified text color and formatting. these functions
are called as part of the menu option when selected using the corresponding push
button */

void WriteLightVal(uint16_t Color) {
  sprintf(lux, "%2.2f", sensors.lightmeter.readLightLevel());
  tft->setTextSize(2);
  tft->setCursor(5, 30);
  tft->setTextColor(Color);
  tft->print(lux);
}
void WriteAirQVal(uint16_t Color) {
  // airinfo = sensors.airreading();
  //  Serial.print("Print pressure ");

  // Serial.println(pressure);
  // sprintf(pressure, "%u", airinfo.pressure);
  // sprintf(temperature, "%2.2f", airinfo.temp);
  // sprintf(humidity, "%2.2f", airinfo.humidity);
  // sprintf(resistance, "%lu", (unsigned long)airinfo.gasresistance);
  // tft->setTextSize(1);
  // tft->setCursor(80, 40);
  // tft->setTextColor(Color);
  // tft->print(temperature);
  // tft->setCursor(60, 60);
  // tft->print(pressure);
  // tft->setCursor(60, 80);
  // tft->print(humidity);
  // tft->setCursor(30, 100);
  // tft->print(resistance);
}

/* These additional functions are related to displaying menu options and the
static text that is part of the sensor readings options from the menu on the
the TFT display. these functions are called in conjunction with the dynamic
sensor readings as part of the menu option when selected using the corresponding
push button */

void static_AIRQ() {
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
  tft->print(F("Gas: "));
  tft->setCursor(105, 100);
  tft->print(F("kOhms"));
  tft->setTextColor(ST77XX_YELLOW);
  tft->setTextSize(1);
  // sensors.airreading();//Calling an update
  // void (*bs)(void);
  // bs = par
  // Feature not a bug, displays startup reading or updated reading after aM!
  memcpy(&airinfo, &(sensors.menu_workaround), sizeof(airoutput));
  sprintf(pressure, "%u", airinfo.pressure);
  sprintf(temperature, "%2.2f", airinfo.temp);
  sprintf(humidity, "%2.2f", airinfo.humidity);
  sprintf(resistance, "%lu", (unsigned long)airinfo.gasresistance);
  tft->setTextSize(1);
  tft->setCursor(80, 40);
  tft->setTextColor(ST7735_ORANGE);
  tft->print(temperature);
  tft->setCursor(60, 60);
  tft->print(pressure);
  tft->setCursor(60, 80);
  tft->print(humidity);
  tft->setCursor(30, 100);
  tft->print(resistance);
}

void static_LightSensor() {
  tft->setCursor(5, 10);
  tft->setTextColor(ST77XX_YELLOW);
  tft->setTextSize(2);
  tft->print(F("Light Value: "));
}

void menu_options() {
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
  tft->print(F("(3) Light sensor reading \n"));
  tft->setCursor(5, 100);
  tft->setTextColor(ST77XX_WHITE);
  tft->setTextSize(1);
  tft->print(F("(4) Eject SD card  \n"));
}

void eject_SD() {
  // detachInterrupt(PB_4);
  tft->fillScreen(ST77XX_RED);
  tft->setCursor(5, 60);
  tft->setTextColor(ST77XX_WHITE);
  tft->setTextSize(1);
  tft->print(F("SD Card ejected \n"));
  eject_sd = true;
  // attachInterrupt(PB_4, eject_SD, LOW);
}

/* the following function is driven by the button interrupt
when the option to display the light sensor reading is chosen
from the menu. the function additionally calls the static and
dynamic part of the format displayed from the above functions */

void Light_disp() {
  // detachInterrupt(PB_3);
  static_LightSensor();
  WriteLightVal(ST7735_WHITE);
  // attachInterrupt(PB_3, Light_disp, LOW);
}

/* the following function is driven by the button interrupt
when the option to display the menu options is chosen.
the function additionally calls the static menu options
from the above functions */

void Disp_menu() {
  // detachInterrupt(PB_1);
  menu_options();
  // attachInterrupt(PB_1, Disp_menu, LOW);
};

/* the following function is driven by the button interrupt
when the option to display the air quality sensor reading is chosen
from the menu. the function additionally calls the static and
dynamic part of the format displayed from the above functions */

void Airq() {
  // Disappointing display of not usings a struct properly,
  // sensors.bme.performReading();
  // airinfo.temp = sensors.bme.temperature;
  // airinfo.pressure = sensors.bme.pressure;
  // airinfo.gasresistance = sensors.bme.gas_resistance;
  // airinfo.humidity = sensors.bme.humidity;
  static_AIRQ();
  // WriteAirQVal(ST7735_RED);
  //  attachInterrupt(PB_2, Airq, LOW);
}

void isr_workaround() {
  if (!digitalRead(PB_1)) {
    tft->fillScreen(ST77XX_BLACK);
    Disp_menu();
    return;
  }
  if (!digitalRead(PB_2)) {
    tft->fillScreen(ST77XX_BLACK);
    Airq();
    return;
  }
  if (!digitalRead(PB_3)) {
    tft->fillScreen(ST77XX_BLACK);
    Light_disp();
    return;
  }
  if (!digitalRead(PB_4)) {
    tft->fillScreen(ST77XX_BLACK);
    eject_SD();
    return;
  }
}
/* The Menuinit function is responsible for initializing the menu system
 Overall, the function sets up the necessary pin modes, initializes the TFT
 display, displays the menu options, and attaches interrupts to handle button
 presses and trigger corresponding functions. */

extern void Menuinit() {

  pinMode(PB_1, INPUT_PULLUP);
  pinMode(PB_2, INPUT_PULLUP);
  pinMode(PB_3, INPUT_PULLUP);
  pinMode(PB_4, INPUT_PULLUP);
  eject_sd = false;
  tft->begin();
  tft->setRotation(3);
  tft->fillScreen(ST77XX_BLACK);
  menu_options();
  Timer3.attachInterrupt(isr_workaround).setFrequency(10).start();
}

extern void updateMenu(uint32_t current_time) {}

#endif

// #ifndef MENUS_H
// #define MENUS_H
// #include <Wire.h>
// // #include <Adafruit_GFX.h> // Cor
// #include <Arduino_GFX.h>
// #include <Arduino_GFX_Library.h>
// #include <Adafruit_ST7735.h>
// #include <SPI.h>
// #include <Arduino.h>
// #include "..\lib\sensors.h"
// #define PB_1 2
// #define PB_2 3
// #define PB_3 4
// #define PB_4 5
// enum display_settings
// {
//     TFT_CS = 10,
//     TFT_RST = 6,
//     TFT_DC = 7,
//     TFT_SCLK = 13,
//     TFT_MOSI = 11
// };
// struct statem
// {
//     int flag;
//     unsigned long interval;
//     unsigned long p_time;
//     unsigned long c_time;
// };

// struct statem delay1, delay2;
// volatile int previous_state;
// volatile int loop_state;
// char lux[5]; //= 0;
// // Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK,
// TFT_RST); Arduino_DataBus *bus = new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCLK,
// TFT_MOSI, TFT_RST); Arduino_GFX *tft = new Arduino_ST7735(bus, -1 /* RST */,
// 0 /* rotation */, false /* IPS */,
//                                       128 /* width */, 160 /* height */, 0 /*
//                                       col offset 1 */, 0 /* row offset 1 */,
//                                       0 /* col offset 2 */, 0 /* row offset 2
//                                       */);
// char temperature[15];
// char pressure[15];
// char humidity[15];
// char gas[15];
// char resistance[15];
// // char light[5];
// airoutput airinfo;

// void WriteLightVal(uint16_t Color)
// {
//     tft->setTextSize(2);
//     tft->setCursor(5, 30);
//     tft->setTextColor(Color);
//     tft->print(lux);
// }
// void WriteAirQVal(uint16_t Color)
// {
//     tft->setCursor(80, 40);
//     tft->setTextColor(Color);
//     tft->print(temperature);
//     tft->setCursor(60, 60);
//     tft->print(pressure);
//     tft->setCursor(60, 80);
//     tft->print(humidity);
//     tft->setCursor(30, 100);
//     ;
//     tft->print(resistance);
// }

// enum
// {
//     WRITE,
//     MASK,
//     STOP
// };
// enum
// {
//     MENU,
//     LIGHT,
//     AIRQ
// };

// void clsscreen()
// {
//     if (previous_state != loop_state)
//     {
//         tft->fillScreen(ST7735_BLACK);
//         loop_state = previous_state;
//     }
// }
// inline void eject_SD()
// {
//     // clsscreen();
//     detachInterrupt(PB_3);
//     tft->fillScreen(ST77XX_RED);
//     attachInterrupt(PB_3, eject_SD, LOW);
// }
// inline void LightSensor()
// {
//     // clsscreen();
//     if ((delay1.p_time + delay1.interval <= delay1.c_time))
//     {                                  // WE ARE CHECKING THE INTERVAL AND
//     MAKING SURE WE ARE ALREADY IN A STATE OF WAITING
//         delay1.flag += 1;              // W E NOW SWITCH STATE
//         delay1.p_time = delay1.c_time; // RESET OUR TIMER
//         if (delay1.flag > STOP)
//         {
//             delay1.flag = WRITE;
//         }
//     }
//     if (delay1.flag == WRITE)
//     {

//         // if (lux <= -2)
//         //{
//         //  lightMeter.

//         WriteLightVal(ST7735_WHITE);
//     }
//     if (delay1.flag == MASK)
//     {
//         WriteLightVal(ST7735_BLACK);
//         sprintf(lux, "%2.2f", sensors.lightmeter.readLightLevel()); //=
//         sensors.lightmeter.readLightLevel();
//     }
// }
// inline void Light_disp()
// {
//     detachInterrupt(PB_4);
//     // tft->fillScreen(ST7735_BLACK);
//     // clsscreen();
//     loop_state = LIGHT;
//     // tft->fillScreen(ST7735_BLACK);
//     tft->setCursor(5, 10);
//     tft->setTextColor(ST77XX_YELLOW);
//     tft->setTextSize(2);
//     tft->print(F("Light Value: "));
//     attachInterrupt(PB_4, Light_disp, LOW);
// }
// inline void Disp_menu()
// {
//     detachInterrupt(PB_1);
//     // tft->fillScreen(ST7735_BLACK);
//     // clsscreen();
//     loop_state = MENU;
//     // tft->fillScreen(ST7735_BLACK);
//     tft->setCursor(55, 8);
//     tft->setTextColor(ST77XX_YELLOW);
//     tft->setTextSize(2);
//     tft->print(F("MENU"));
//     tft->setCursor(5, 40);
//     tft->setTextColor(ST77XX_WHITE);
//     tft->setTextSize(1);
//     tft->print(F("(1) View this menu \n"));
//     tft->setCursor(5, 60);
//     tft->setTextColor(ST77XX_WHITE);
//     tft->setTextSize(1);
//     tft->print(F("(2) AirQ sensor reading \n"));
//     tft->setCursor(5, 80);
//     tft->setTextColor(ST77XX_WHITE);
//     tft->setTextSize(1);
//     tft->print(F("(3) Eject SD card \n"));
//     tft->setCursor(5, 100);
//     tft->setTextColor(ST77XX_WHITE);
//     tft->setTextSize(1);
//     tft->print(F("(4) Light sensor reading \n"));
//     attachInterrupt(PB_1, Disp_menu, LOW);
// };
// void AirSense()
// {

//     // clsscreen();
//     tft->setTextSize(1);
//     airinfo = sensors.airreading();
//     sprintf(pressure, "%u", airinfo.pressure);
//     sprintf(temperature, "%2.2f", airinfo.temp);
//     sprintf(humidity, "%2.2f", airinfo.humidity);
//     sprintf(resistance, "%lu", (unsigned long)airinfo.gasresistance);
//     if ((delay2.p_time + delay2.interval <= delay2.c_time))
//     {
//         delay2.flag += 1;              // W E NOW SWITCH STATE
//         delay2.p_time = delay2.c_time; // RESET OUR TIMER
//         if (delay2.flag > STOP)
//         {
//             delay2.flag = WRITE;
//         }
//     }
//     if (delay2.flag == WRITE)
//     {

//         WriteAirQVal(ST7735_WHITE);
//     }
//     if (delay2.flag == MASK)
//     {
//         WriteAirQVal(ST7735_BLACK);
//     }
// }
// void Airq()
// {
//     detachInterrupt(PB_2);
//     loop_state = AIRQ;
//     // clsscreen();
//     tft->setCursor(15, 10);
//     tft->setTextColor(ST77XX_CYAN);
//     tft->setTextSize(2);
//     tft->print(F("AirQ Sensor"));
//     tft->setCursor(5, 40);
//     tft->setTextColor(ST77XX_YELLOW);
//     tft->setTextSize(1);
//     tft->print(F("Temperature: "));
//     tft->setCursor(115, 40);
//     tft->print(F("C"));
//     tft->setCursor(5, 60);
//     tft->setTextColor(ST77XX_YELLOW);
//     tft->setTextSize(1);
//     tft->print(F("Pressure: "));
//     tft->setCursor(105, 60);
//     tft->print(F("kPa"));
//     tft->setCursor(5, 80);
//     tft->setTextColor(ST77XX_YELLOW);
//     tft->setTextSize(1);
//     tft->print(F("Humidity: "));
//     tft->setCursor(100, 80);
//     tft->print(F("%"));
//     tft->setCursor(5, 100);
//     tft->setTextColor(ST77XX_YELLOW);
//     tft->setTextSize(1);
//     tft->print(F("Gas: "));
//     tft->setCursor(65, 100);
//     tft->print(F("kOhms"));
//     AirSense();
//     attachInterrupt(PB_2, Airq, LOW);
// }
// extern void Menuinit()
// {
//     previous_state = loop_state = 0;
//     delay1 = {WRITE, 1, 0, 0};
//     delay2 = {WRITE, 1, 0, 0};
//     pinMode(PB_1, INPUT_PULLUP);
//     pinMode(PB_2, INPUT_PULLUP);
//     pinMode(PB_3, INPUT_PULLUP);
//     pinMode(PB_4, INPUT_PULLUP);

//     delay1.p_time = delay1.c_time = millis();
//     delay1.flag = WRITE;

//     // tft->initR(INITR_BLACKTAB);
//     tft->begin();
//     tft->setRotation(3);
//     tft->fillScreen(ST77XX_BLACK);
//     tft->setCursor(7, 10);
//     tft->setTextColor(ST77XX_GREEN);
//     tft->setTextSize(1);
//     attachInterrupt(PB_1, Disp_menu, LOW);
//     attachInterrupt(PB_2, Airq, LOW);
//     attachInterrupt(PB_3, eject_SD, LOW);
//     attachInterrupt(PB_4, Light_disp, LOW);
//     loop_state = MENU;
//     // tft->print("Welcome! press button 1 \n to view the menu");
// }
// int lastupdate = 0;
// extern void updateMenu(uint32_t current_time)
// {

//     //delay1.c_time = delay2.c_time = current_time;
//     // if(current_time < lastupdate + 5000){
//     //     //lastupdate = current_time;
//     //     return;
//     // }
//     // lastupdate = current_time;
//     // if(loop_state != lastupdate){}else{return;}
//     // switch (loop_state)
//     // {
//     // case MENU:
//     // {
//     //     Disp_menu();
//     // }
//     // break;
//     // case LIGHT:
//     // {

//     //     LightSensor();
//     // }
//     // break;
//     // case AIRQ:
//     // {

//     //     AirSense();
//     // }
//     // break;
//     // default:
//     // {
//     //     //clsscreen();
//     // }
//     // break;
//     // };
// }

// #endif