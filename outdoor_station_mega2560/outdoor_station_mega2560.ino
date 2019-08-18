/*
 * Arduino Outdoor Weather Station Project
 *
 * REQUIRES the following Arduino libraries:
 * 	- DHT_sensor_library		v1.3.7	DHT22 Sensor: https://github.com/adafruit/DHT-sensor-library
 * 	- Adafruit_Unified_Sensor	v1.0.3	Adafruit Unified Sensor: https://github.com/adafruit/Adafruit_Sensor
 *	- SPI						------	SPI Master library for Arduino (Arduino built-in)
 *	- NRFLite					2.2.2	nRF24L01+ 2.4 GHz Transceiver library: https://github.com/dparson55/NRFLite
 */


/*
 * Include libraries
 */
#include "Arduino.h"
#include "DHT.h"				// DHT sensor librar
#include <SPI.h>				// SPI Master library for Arduino (required by Adafruit_GFX.h and NRFLite.h)
#include <NRFLite.h>			// nRF24L01+ Transceiver library


/*
 * Debug initialize
 */
#define DEBUG // activate "debug switch"
#ifdef DEBUG
#define DBUG_PRINT(x) Serial.print(x)		// activate print
#define DEBUG_PRINTLINE(x) Serial.println(x)	// activate print line
#else
#define DEBUG_PRINT(x)						// do nothing
#define DEBUG_PRINTLINE(x)					// do nothing
#endif


/*
 * DHT initialize
 */
#define DHTPIN 3			// Digital pin connected to the DHT sensor
#define DHTTYPE DHT22		// DHT type = DHT22

DHT dht(DHTPIN, DHTTYPE);	// Initialize DHT sensor.


/*
 * nRF24L01+ Radio TX initialize
 */

// Radio    Arduino
// CE    -> 1
// CSN   -> 2	(Hardware SPI SS)
// MOSI  -> 11	(Hardware SPI MOSI)
// MISO  -> 12	(Hardware SPI MISO)
// SCK   -> 13	(Hardware SPI SCK)
// IRQ   -> No	connection
// VCC   -> No	more than 3.6 volts
// GND   -> GND

const static uint8_t RADIO_ID = 1;             // Our radio's id.
const static uint8_t DESTINATION_RADIO_ID = 0; // Id of the radio we will transmit to.
const static uint8_t PIN_RADIO_CE = 1;
const static uint8_t PIN_RADIO_CSN = 2;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
	uint8_t FromRadioId;
	uint32_t OnTimeMillis;
	uint32_t FailedTxCount;
};

NRFLite _radio;
RadioPacket _radioData;




/*****************
 * ARDUINO SETUP *
 ****************/
void setup()
{

	/*
	 * Start serial port & serial monitor to Baud rate 9600 bits/s
	 */
	Serial.begin(9600); // serial return of Arduino's output
	//DEBUG_PRINTLINE("my debug message");


	/*
	 * Start DHT measurement
	 */
	DEBUG_PRINTLINE(F("Start DHT measurement"));
	dht.begin();


	/*
	 * Start nRF24L01+ Radio
	 */
	// By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
	// Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
	// You can run the 'ChannelScanner' example to help select the best channel for your environment.
	// You can assign a different bitrate and channel as shown below.
	//   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)
	//   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
	_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100); // THE DEFAULT

	if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
	{
		Serial.println("Cannot communicate with radio");
		while (1); // Wait here forever.
	}

	_radioData.FromRadioId = RADIO_ID;

}




/****************
 * ARDUINO LOOP *
 ***************/
void loop()
{

	/*
	 * DHT Sensor read
	 */
	// Wait a few seconds between measurements.
	delay(3000);

	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	float h = dht.readHumidity();
	// Read temperature as Celsius (the default)
	float t = dht.readTemperature();
	// Read temperature as Fahrenheit (isFahrenheit = true)
	float f = dht.readTemperature(true);

	// Check if any reads failed and exit early (to try again).
	if (isnan(h) || isnan(t) || isnan(f)) {
		Serial.println(F("Failed to read from DHT sensor!"));
		return;
	}

	// Compute heat index in Fahrenheit (the default)
	//float hif = dht.computeHeatIndex(f, h);
	// Compute heat index in Celsius (isFahreheit = false)
	float hic = dht.computeHeatIndex(t, h, false);

	Serial.print(F("Humidity: "));
	Serial.print(h);
	Serial.print(F("%  Temperature: "));
	Serial.print(t);
	Serial.print(F("°C "));
	//Serial.print(f);
	//Serial.print(F("°F"));
	Serial.print(F("  Heat index: "));
	Serial.print(hic);
	Serial.println(F("°C "));
	//Serial.print(hif);
	//Serial.println(F("°F"));
	Serial.println();
	Serial.println();


	/*
	 * nRF24L01+ Radio RX
	 */
	_radioData.OnTimeMillis = millis();

	Serial.print("Sending ");
	Serial.print(_radioData.OnTimeMillis);
	Serial.print(" ms");

	// By default, 'send' transmits data and waits for an acknowledgement.  If no acknowledgement is received,
	// it will try again up to 16 times.  You can also perform a NO_ACK send that does not request an acknowledgement.
	// The data packet will only be transmitted a single time so there is no guarantee it will be successful.  Any random
	// electromagnetic interference can sporatically cause packets to be lost, so NO_ACK sends are only suited for certain
	// types of situations, such as streaming real-time data where performance is more important than reliability.
	//   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::NO_ACK)
	_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::REQUIRE_ACK); // THE DEFAULT

	if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))) // Note how '&' must be placed in front of the variable name.
	{
		Serial.println("...Success");
	}
	else
	{
		Serial.println("...Failed");
		_radioData.FailedTxCount++;
	}

	delay(1000);


}




/*************
 * FUNCTIONS *
 ************/

/*
 * Debug functions
 */
void print (const char str[], int number) {
	char buf[100];
	sprintf(buf, "%s %d", str, number);
	DEBUG_PRINTLN(buf);
}

void print (const char str[], float number) {
	char buf[100];
	char strbuf[10];
	dtostrf(number, 3, 3, strbuf);
	sprintf(buf, "%s %s", str, strbuf);
	DEBUG_PRINTLN(buf);
}
