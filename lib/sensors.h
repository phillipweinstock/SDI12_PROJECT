#ifndef SENSORS_H
#define SENSORS_H
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <BH1750.h>
/*
  struct keeps all the data togher in an makeing to code more modualar and easer to use in many diffrent applactions
  this same code is called out in both datalog, Screen and SDI12
*/

struct airoutput
{
  float temp;
  uint32_t pressure;
  float humidity;
  uint32_t gasresistance;
};
struct Lightoutput
{
  uint16_t lux;
};

class SensorWrap
{
public:
  void init()
  {

    i2cadd1 = 0x76; // scanI2C();
    bme.begin(i2cadd1, &Wire1);
    /*
      oversampling mode takes a Average of "x" number of measurments
      wer're havent gotten any crazy outline looking data so we're not using it but it would defenetly somthing I would do for something out in the "outside"

      bme.setTemperatureOversampling(BME680_OS_8X);
      bme.setHumidityOversampling(BME680_OS_2X);
      bme.setPressureOversampling(BME680_OS_4X);
      bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
      bme.setGasHeater(320, 150);
    */

    lightmeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire1);
    airreading(); // This is a workaround  to a  stupid allocation glitch w/h menu code,
    lightreading();
  }
  airoutput menu_workaround;
  int i2cadd1;
  SensorWrap(){

  };
  // this is causing the sensors to crash when entering Parsas code

  //__attribute__((noinline))

  /*
  acutal functions are actioned here
 */

  airoutput airreading()
  {
    airoutput output;
    bme.performReading();
    // bme.beginReading();
    // bme.endReading();
    output.temp = bme.temperature;
    output.pressure = bme.pressure / 1000;
    output.humidity = bme.humidity;
    output.gasresistance = bme.gas_resistance / 1000.0;
    memcpy(&menu_workaround, &output, sizeof(airoutput)); // I am not needed when standalone is disabled
    return output;
  }
  Lightoutput lightreading()
  {
    Lightoutput output;
    output.lux = lightmeter.readLightLevel();
    return output;
  }

  BH1750 lightmeter = BH1750(0x23);
  Adafruit_BME680 bme = Adafruit_BME680(&Wire1);

private:
};
SensorWrap sensors = SensorWrap();
extern SensorWrap sensors;
/*
  turns out the entern sensorWrap is need to action the code in other bits of code great use of like 3-4days learing this
*/


#endif