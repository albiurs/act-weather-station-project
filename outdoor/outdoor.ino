/*
   Arduino Weather Station Project
   Sketch for outdoor module
   REQUIRES the following libraries:
   - Adafruit_Unified_Sensor v1.0.3	Adafruit Unified Sensor: https://github.com/adafruit/Adafruit_Sensor
   - Wire						   	     I2C interface library (Arduino built-in)
   - Adafruit GFX Library		 v1.5.6	Adafruit GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
   - SPI						         SPI Master library for Arduino (Arduino built-in)
 	 - NRFLite					       2.2.2	nRF24L01+ 2.4 GHz Transceiver library: https://github.com/dparson55/NRFLite
   - Adafruit_BME280         https://github.com/adafruit/Adafruit_BME280_Library
*/

/*
   Include libraries
*/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Arduino.h"
#include <NRFLite.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

//#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C


unsigned long delayTime;
// lowest and highest sensor readings:

int rainReading = 0;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
	float outTemp;
	float outHum;
	float outPress;
	float outRain;
};

NRFLite _radio;
RadioPacket _radioData;
int rainIntensity = 0;
const int SENSORMIN = 0;     // rain sensor minimum at pin A0 
const int SENSORMAX = 1024;  // rain sensor maximum at pin A0
const float SEALEVELPRESSURE_HPA = 1013.25;
const int ALTITUDE = 408; // altitude of location (Zurich = 408m), used for pressure-calibration
const static uint8_t RADIO_ID = 1;             // Our radio's id.
const static uint8_t DESTINATION_RADIO_ID = 0; // Id of the radio we will transmit to.
const static uint8_t PIN_RADIO_CE = 1;
const static uint8_t PIN_RADIO_CSN = 2;



void setup() {
    Serial.begin(9600);
    while(!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    unsigned status;
    
    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); 
        Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1);
    }
    
    Serial.println("-- Default Test --");
    delayTime = 2000;

    Serial.println();
    //_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100); // THE DEFAULT

	while (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
	{
		Serial.println("Cannot communicate with radio");
		_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100); // THE DEFAULT
		
	}
	if(_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)) {
	  Serial.println("Radio is ready");
	}
}


void loop() { 
  
	
    printValues();
    delay(delayTime);
}


void printValues() {
  
  rainReading = analogRead(A0); // analog pin for rain sensor
	Serial.print("Regen: ");
	Serial.println(analogRead(A0));
	
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");
    
    Serial.print(bme.seaLevelForAltitude(408.0, bme.readPressure()) / 100.0F);
    Serial.println(" hPa Meereshoehe");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
    	// read all the sensor values
	float temperature = bme.readTemperature();
	float humidity    = bme.readHumidity();
	float pressure    = (bme.seaLevelForAltitude(408.0, bme.readPressure()) / 100.0F);

	if(analogRead(A0)>SENSORMAX*0.9) {
	  rainIntensity=0; // dry = no rain
	} else if(analogRead(A0)<SENSORMAX*0.5) {
	  rainIntensity=2; // heavy rain
	} else {
	  rainIntensity=1; // light rain
	}
	Serial.print("Map-Wert: ");
	Serial.println(rainIntensity * 1.0);

		/*
	 * nRF24L01+ Radio TX
	 */
	_radioData.outTemp = temperature;
	_radioData.outHum = humidity;
	_radioData.outPress = pressure;
	
	_radioData.outRain = rainIntensity * 1.0F;

	_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::REQUIRE_ACK); // THE DEFAULT



}
