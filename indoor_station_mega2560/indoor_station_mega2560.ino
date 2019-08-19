/*
 * Arduino Indoor Weather Station Project
 *
 * REQUIRES the following Arduino libraries:
 * 	- DHT_sensor_library		v1.3.7	DHT22 Sensor: https://github.com/adafruit/DHT-sensor-library
 * 	- Adafruit_Unified_Sensor	v1.0.3	Adafruit Unified Sensor: https://github.com/adafruit/Adafruit_Sensor
 * 	- Wire						------	I2C interface library (Arduino built-in)
 * 	- DS3231					v1.0.2	RTC library: https://github.com/NorthernWidget/DS3231
 * 	- Adafruit GFX Library		v1.5.6	Adafruit GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
 * 	- SSD1351 					v1.2.2	Adafruit SSD1351 for the 1.5" 16-bit Color OLED with SSD1351 driver chip: https://github.com/adafruit/Adafruit-SSD1351-library
 * 								GFX tutorial page: https://learn.adafruit.com/adafruit-gfx-graphics-library
 *	- SPI						------	SPI Master library for Arduino (Arduino built-in)
 *	- NRFLite					2.2.2	nRF24L01+ 2.4 GHz Transceiver library: https://github.com/dparson55/NRFLite
 */


/*
 * Include libraries
 */
#include "Arduino.h"
#include "DHT.h"				// DHT sensor library
#include <DS3231.h>				// RTC DS3231 library
#include <Wire.h>				// I2C wire interface library
#include <Adafruit_GFX.h>		// Adafruit 1.5" 16-bit Color OLED library
#include <Adafruit_SSD1351.h>	// Adafruit 1.5" 16-bit Color OLED  Driver
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
#define DHTPIN 8			// Digital pin connected to the DHT sensor
#define DHTTYPE DHT22		// DHT type = DHT22

DHT dht(DHTPIN, DHTTYPE);	// Initialize DHT sensor.


/*
 * RTC DS3231 initialize
 */
DS3231 Clock;		// initialize DS3231 RTC
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;

byte year, month, date, DoW, hour, minute, second;


/*
 * Adafruit 1.5" 16-bit Color OLED Initialize
 */
// Screen dimensions:
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// You can use any (4 or) 5 pins:
#define SCLK_PIN 2
#define MOSI_PIN 3
#define DC_PIN   4
#define CS_PIN   5
#define RST_PIN  6

// Color definitions:
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

// Option 1: use any pins but a little slower
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);
// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
//Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);


/*
 * nRF24L01+ Radio RX initialize
 */

// Radio    Arduino
// CE    -> 9
// CSN   -> 10 (Hardware SPI SS)
// MOSI  -> 11 (Hardware SPI MOSI)
// MISO  -> 12 (Hardware SPI MISO)
// SCK   -> 13 (Hardware SPI SCK)
// IRQ   -> No connection
// VCC   -> No more than 3.6 volts
// GND   -> GND

const static uint8_t RADIO_ID = 0;       // Our radio's id.  The transmitter will send to this id.
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
	float outTemp;
	float outHum;
	float outPress;
	float outIllum;
	float outUva;
	float outUvb;
	float outUvIndex;
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
	 * Start the I2C interface
	 */
	DEBUG_PRINTLINE("Start the I2C interface");
	Wire.begin();


	/*
	 * Start DHT measurement
	 */
	DEBUG_PRINTLINE(F("Start DHT measurement"));
	dht.begin();


	/*
	 * Start RTC > setup date & time
	 */
	// VERYIMPORTANT COMMENTS BELOW!!!
	// If you want to set the time, change these numbers to the date and time
	// you want to set to, and then upload it to the arduino.
	//
	// once you have finished setting the time, comment out the following clock.set functions
	// and then re-upload it to the board. Otherwise your clock will reset
	// every time you open the serial monitor.

	Clock.setSecond(8);//Set the second
	Clock.setMinute(58);//Set the minute
	Clock.setHour(8);  //Set the hour
	Clock.setDoW(6);    //Set the day of the week
	Clock.setDate(17);  //Set the date of the month
	Clock.setMonth(8);  //Set the month of the year
	Clock.setYear(19);  //Set the year (Last two digits of the year)


	/*
	 * Start OLED
	 */
	tft.begin();

	// You can optionally rotate the display by running the line below.
	// Note that a value of 0 means no rotation, 1 means 90 clockwise,
	// 2 means 180 degrees clockwise, and 3 means 270 degrees clockwise.
	//tft.setRotation(1);
	// NOTE: The test pattern at the start will NOT be rotated!  The code
	// for rendering the test pattern talks directly to the display and
	// ignores any rotation.


	// Start nRF24L01+ Radio
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

}




/****************
 * ARDUINO LOOP *
 ***************/
void loop()
{

	/*
	 * RTC read
	 */
	// send what's going on to the serial monitor.
	// Start with the year
	Serial.print("2");
	if (Century) {			// Won't need this for 89 years.
		Serial.print("1");
	} else {
		Serial.print("0");
	}
	Serial.print(Clock.getYear(), DEC);
	Serial.print(' ');
	// then the month
	Serial.print(Clock.getMonth(Century), DEC);
	Serial.print(' ');
	// then the date
	Serial.print(Clock.getDate(), DEC);
	Serial.print(' ');
	// and the day of the week
	Serial.print(Clock.getDoW(), DEC);
	Serial.print(' ');
	// Finally the hour, minute, and second
	Serial.print(Clock.getHour(h12, PM), DEC);
	Serial.print(' ');
	Serial.print(Clock.getMinute(), DEC);
	Serial.print(' ');
	Serial.print(Clock.getSecond(), DEC);
	// Add AM/PM indicator
	if (h12) {
		if (PM) {
			Serial.print(" PM ");
		} else {
			Serial.print(" AM ");
		}
	} else {
		Serial.print(" 24h ");
	}
	Serial.println();
	Serial.println();

	// New line on console
	Serial.print('\n');
	delay(1500);

	// Print values to OLED
	tft.setCursor(0, 5);
	tft.fillScreen(BLACK);
	tft.setTextColor(WHITE);
	tft.setTextSize(2);
	tft.print(Clock.getHour(h12, PM), DEC);
	tft.print(":");
	tft.print(Clock.getMinute(), DEC);
	tft.print(":");
	tft.println(Clock.getSecond(), DEC);
	tft.println();
	delay(200);

	tft.setTextSize(2);
	tft.setTextColor(GREEN);
	tft.print(Clock.getDate(), DEC);
	tft.print(".");
	tft.print(Clock.getMonth(Century), DEC);
	tft.print(".");
	tft.println(Clock.getYear(), DEC);
	delay(2000);


	/*
	 * DHT Sensor Read
	 */
	// Wait a few seconds between measurements.
	//delay(3000);

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

	Serial.print(F("%  Temperature: "));
	Serial.print(t);
	Serial.print(F("°C"));
	//Serial.print(f);
	//Serial.print(F("°F"));
	Serial.print(F("  Heat index: "));
	Serial.print(hic);
	Serial.println(F("°C"));
	//Serial.print(hif);
	//Serial.println(F("°F"));
	Serial.print(F("Humidity: "));
	Serial.print(h);

	Serial.println();
	Serial.println();

	// Print Title to OLED
	tft.fillScreen(BLACK);
	tft.setCursor(0, 5);
	tft.setTextColor(WHITE);
	tft.setTextSize(2);
	tft.println("Indoor");
	tft.setTextSize(0);
	tft.println();

	// Print values to OLED
	tft.setTextColor(RED);
	tft.setTextSize(1);
	tft.print("Temperatur: ");
	tft.setTextSize(0);
	tft.println();
	tft.setTextSize(2);
	tft.print(t);
	tft.println("°C");
	tft.setTextSize(0);
	tft.println();
	delay(200);

	tft.setTextColor(MAGENTA);
	tft.setTextSize(1);
	tft.print("Temp.-Index: ");
	tft.setTextSize(0);
	tft.println();
	tft.setTextSize(2);
	tft.print(hic);
	tft.println("°C");
	tft.setTextSize(0);
	tft.println();
	delay(200);

	tft.setTextColor(BLUE);
	tft.setTextSize(1);
	tft.print("Luftfeuchtigkeit: ");
	tft.setTextSize(0);
	tft.println();
	tft.setTextSize(2);
	tft.print(h);
	tft.println("%");
	tft.setTextSize(0);
	tft.println();

	delay(2000);


	/*
	 * nRF24L01+ Radio RX
	 */

	while (_radio.hasData())
	{
		_radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.
	}

	String msg1 = "";
	msg1 += _radioData.outTemp;
	msg1 += "°C";

	String msg2 = "";
	msg2 += _radioData.outHum;
	msg2 += "%";

	String msg3 = "";
	msg3 += _radioData.outPress;
	msg3 += "kPa";

	String msg4 = "";
	msg4 += _radioData.outIllum;
	msg4 += "lx";

	String msg5 = "";
	msg5 += _radioData.outUva;
	msg5 += "";

	String msg6 = "";
	msg6 += _radioData.outUvb;
	msg6 += "";

	String msg7 = "";
	msg7 += _radioData.outUvIndex;
	msg7 += "";

	Serial.print("Temp.: ");
	Serial.println(msg1);

	Serial.print("Luftfeuchtigkeit: ");
	Serial.println(msg2);

	Serial.print("Luftdruck: ");
	Serial.println(msg3);

	Serial.print("Illuminanz: ");
	Serial.println(msg4);

	Serial.print("UVA: ");
	Serial.println(msg5);

	Serial.print("UVB: ");
	Serial.println(msg6);

	Serial.print("UV Index: ");
	Serial.println(msg7);

	// Print Title to OLED
	tft.fillScreen(BLACK);
	tft.setCursor(0, 5);
	tft.setTextColor(WHITE);
	tft.setTextSize(2);
	tft.println("Outdoor");
	tft.setTextSize(0);
	tft.println();

	// Print values to OLED

	tft.setTextColor(RED);
	tft.setTextSize(1);
	tft.println("Temperatur:");
	tft.setTextSize(2);
	tft.println(msg1);
	tft.setTextSize(0);
	tft.println();

	tft.setTextColor(MAGENTA);
	tft.setTextSize(1);
	tft.println("Luftfeuchtigkeit:");
	tft.setTextSize(2);
	tft.println(msg2);
	tft.setTextSize(0);
	tft.println();

	tft.setTextColor(BLUE);
	tft.setTextSize(1);
	tft.println("Luftdruck:");
	tft.setTextSize(2);
	tft.println(msg3);
	tft.setTextSize(0);
	tft.println();

	delay(2000);

	// Print Title to OLED
	tft.fillScreen(BLACK);
	tft.setCursor(0, 5);
	tft.setTextColor(WHITE);
	tft.setTextSize(2);
	tft.println("Outdoor");
	tft.setTextSize(0);
	tft.println();

	// Print values to OLED

	tft.setTextColor(RED);
	tft.setTextSize(1);
	tft.println("Illuminanz:");
	tft.setTextSize(1);
	tft.println(msg4);
	tft.setTextSize(0);
	tft.println();

	tft.setTextColor(MAGENTA);
	tft.setTextSize(1);
	tft.println("UVA:");
	tft.setTextSize(1);
	tft.println(msg5);
	tft.setTextSize(0);
	tft.println();

	tft.setTextColor(BLUE);
	tft.setTextSize(1);
	tft.println("UVB:");
	tft.setTextSize(1);
	tft.println(msg6);
	tft.setTextSize(0);
	tft.println();

	tft.setTextColor(BLUE);
	tft.setTextSize(1);
	tft.println("UV Index:");
	tft.setTextSize(1.5);
	tft.println(msg6);
	tft.setTextSize(0);
	tft.println();

	delay(2000);

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
	DEBUG_PRINTLINE(buf);
}

void print (const char str[], float number) {
	char buf[100];
	char strbuf[10];
	dtostrf(number, 3, 3, strbuf);
	sprintf(buf, "%s %s", str, strbuf);
	DEBUG_PRINTLINE(buf);
}
