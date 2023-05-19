#ifndef SENSORS_H
#define SENSORS_H
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <BH1750.h>

// Adafruit_BME680 bme; // create BME680 object
// BH1750  lightMeter = BH1750(); //lightMeter(0x23); // create BH1750 object with I2C address
uint8_t scanI2C() {
  uint8_t addr;
  uint8_t error, address;
  int nDevices;
  
  Serial.println("Scanning I2C bus for BME680 sensor...");
  
  nDevices = 0;
  for (address = 1; address < 127; address++ ) {
    Wire1.beginTransmission(address);
    error = Wire1.endTransmission();
    if (error == 0) {
      Serial.print("Device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.print(address,HEX);
      Serial.println("!");
      if (address == 0x76 || address == 0x77) {
        addr = address;
        break;
      }
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  
  return addr;
}
  struct airoutput{
    float temp;
    uint32_t pressure;
    float humidity;
    uint32_t gasresistance;
  };
class SensorWrap
{
public:
  void init(){

      i2cadd1 = 0x76;//scanI2C();
      bme.begin(i2cadd1,&Wire1);
      lightmeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE,0x23,&Wire1);
      
  }
  int i2cadd1;
  SensorWrap(){


  };

  airoutput airreading(){
     airoutput output;
     bme.performReading();
     output.temp = bme.temperature;
     output.pressure = bme.pressure / 1000;
     output.humidity = bme.humidity;
     output.gasresistance = bme.gas_resistance /1000.0;
     return output;
  }
  BH1750 lightmeter = BH1750(0x23);
  Adafruit_BME680 bme = Adafruit_BME680(&Wire1);

  

private:
};
SensorWrap sensors = SensorWrap();
extern SensorWrap sensors;

#endif