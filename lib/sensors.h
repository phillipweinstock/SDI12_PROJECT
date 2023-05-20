#ifndef SENSORS_H
#define SENSORS_H
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <BH1750.h>

struct airoutput
{
  float temp;
  uint32_t pressure;
  float humidity;
  uint32_t gasresistance;
};
class SensorWrap
{
public:
  void init()
  {

    i2cadd1 = 0x76; // scanI2C();
    bme.begin(i2cadd1, &Wire1);
    /*Apparently this is needed for accuracy
    Thank you Don for bringing this to my attention
    TODO: Ask Saroj to investigate if code below is needed.
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);
    */
    lightmeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire1);
  }
  int i2cadd1;
  SensorWrap(){

  };

  airoutput airreading()
  {
    airoutput output;
    bme.performReading();
    output.temp = bme.temperature;
    output.pressure = bme.pressure / 1000;
    output.humidity = bme.humidity;
    output.gasresistance = bme.gas_resistance / 1000.0;
    return output;
  }
  BH1750 lightmeter = BH1750(0x23);
  Adafruit_BME680 bme = Adafruit_BME680(&Wire1);

private:
};
SensorWrap sensors = SensorWrap();
extern SensorWrap sensors;

#endif