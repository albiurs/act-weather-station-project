/*
 * Arduino Indoor Weather Station Project
 *
 * REQUIRES the following Arduino libraries:
 * 	- DHT_sensor_library		https://github.com/adafruit/DHT-sensor-library
 * 	- Adafruit_Unified_Sensor	https://github.com/adafruit/Adafruit_Sensor
 * 	- Wire						I2C interface library
 * 	- DS3231					RTC library
 * 	- SSD1351 					library for the 1.5" 16-bit Color OLED with SSD1351 driver chip
 * 	  GFX tutorial page: https://learn.adafruit.com/adafruit-gfx-graphics-library
 */

#include "Arduino.h"
#include "DHT.h"				// DHT sensor library
#include <DS3231.h>				// RTC DS3231 library
#include <Wire.h>				// I2C wire interface library
#include <Adafruit_GFX.h>		// Adafruit 1.5" 16-bit Color OLED library
#include <Adafruit_SSD1351.h>	// Adafruit 1.5" 16-bit Color OLED  Driver
#include <SPI.h>				// SPI Master library for Arduino (required by Adafruit_GFX.h)


// Debug Sketch
#define DEBUG // activate "debug switch"
#ifdef DEBUG
#define DBUG_PRINT(x) Serial.print(x)		// activate print
#define DEBUG_PRINTLINE(x) Serial.println(x)	// activate print line
#else
#define DEBUG_PRINT(x)						// do nothing
#define DEBUG_PRINTLINE(x)					// do nothing
#endif


// DHT
#define DHTPIN 8			// Digital pin connected to the DHT sensor
#define DHTTYPE DHT22		// DHT type = DHT22

DHT dht(DHTPIN, DHTTYPE);	// Initialize DHT sensor.


// RTC DS3231
DS3231 Clock;		// initialize DS3231 RTC
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;

byte year, month, date, DoW, hour, minute, second;


// Adafruit 1.5" 16-bit Color OLED
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

float p = 3.1415926;	// pi required for graphic demo




/*****************
 * ARDUINO SETUP *
 ****************/
void setup()
{

	// config serial port & serial monitor to Baud rate 9600 bits/s
	Serial.begin(9600); // serial return of Arduino's output
	//DEBUG_PRINTLINE("my debug message");

	// Start the I2C interface
	DEBUG_PRINTLINE("Start the I2C interface");
	Wire.begin();

	// Start DHT measurement
	DEBUG_PRINTLINE(F("Start DHT measurement"));
	dht.begin();

	// VERYIMPORTANT COMMENTS BELOW!!!
	// If you want to set the time, change these numbers to the date and time
	// you want to set to, and then upload it to the arduino.
	//
	// once you have finished setting the time, comment out the following clock.set functions
	// and then re-upload it to the board. Otherwise your clock will reset
	// every time you open the serial monitor.

	//Clock.setSecond(8);//Set the second
	//Clock.setMinute(58);//Set the minute
	//Clock.setHour(19);  //Set the hour
	//Clock.setDoW(5);    //Set the day of the week
	//Clock.setDate(16);  //Set the date of the month
	//Clock.setMonth(8);  //Set the month of the year
	//Clock.setYear(19);  //Set the year (Last two digits of the year)

	// Start OLED
	tft.begin();

	// You can optionally rotate the display by running the line below.
	// Note that a value of 0 means no rotation, 1 means 90 clockwise,
	// 2 means 180 degrees clockwise, and 3 means 270 degrees clockwise.
	//tft.setRotation(1);
	// NOTE: The test pattern at the start will NOT be rotated!  The code
	// for rendering the test pattern talks directly to the display and
	// ignores any rotation.

}




/****************
 * ARDUINO LOOP *
 ***************/
void loop()
{

	/*
	 * DHT Sensor Read
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

	tft.fillScreen(BLACK);
	tft.setCursor(0, 5);
	tft.setTextColor(BLUE);
	tft.setTextSize(1);
	tft.print("Luft: ");
	tft.setTextSize(0);
	tft.println();
	tft.setTextSize(2);
	tft.print(h);
	tft.println("%");
	tft.setTextSize(0);
	tft.println();
	delay(200);

	tft.setTextColor(RED);
	tft.setTextSize(1);
	tft.print("Temp.: ");
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
	tft.print("Index: ");
	tft.setTextSize(0);
	tft.println();
	tft.setTextSize(2);
	tft.print(hic);
	tft.println("°C");
	delay(10000);


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

	// New line on display
	Serial.print('\n');

	// Print Date & Time
	delay(1500);
	tft.setCursor(0, 5);
	tft.fillScreen(BLACK);
	tft.setTextColor(WHITE);
	tft.setTextSize(3);
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
	delay(10000);


	/*
	 * OLED Loop
	 */
	uint16_t time = millis();
	tft.fillRect(0, 0, 128, 128, BLACK);
	time = millis() - time;

	Serial.println(time, DEC);
	delay(500);

	lcdTestPattern();
	delay(500);

	tft.invert(true);
	delay(100);
	tft.invert(false);
	delay(100);

	tft.fillScreen(BLACK);
	testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", WHITE);
	delay(500);

	// tft print function!
	tftPrintTest();
	delay(500);

	//a single pixel
	tft.drawPixel(tft.width()/2, tft.height()/2, GREEN);
	delay(500);

	// line draw test
	testlines(YELLOW);
	delay(500);

	// optimized lines
	testfastlines(RED, BLUE);
	delay(500);


	testdrawrects(GREEN);
	delay(1000);

	testfillrects(YELLOW, MAGENTA);
	delay(1000);

	tft.fillScreen(BLACK);
	testfillcircles(10, BLUE);
	testdrawcircles(10, WHITE);
	delay(1000);

	testroundrects();
	delay(500);

	testtriangles();
	delay(500);

	//Serial.println("done");
	//delay(1000);

}




/*************
 * FUNCTIONS *
 ************/

/*
 * OLED Color Display Functions
 */
void testlines(uint16_t color) {
	tft.fillScreen(BLACK);
	for (uint16_t x=0; x < tft.width()-1; x+=6) {
		tft.drawLine(0, 0, x, tft.height()-1, color);
	}
	for (uint16_t y=0; y < tft.height()-1; y+=6) {
		tft.drawLine(0, 0, tft.width()-1, y, color);
	}

	tft.fillScreen(BLACK);
	for (uint16_t x=0; x < tft.width()-1; x+=6) {
		tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
	}
	for (uint16_t y=0; y < tft.height()-1; y+=6) {
		tft.drawLine(tft.width()-1, 0, 0, y, color);
	}

	tft.fillScreen(BLACK);
	for (uint16_t x=0; x < tft.width()-1; x+=6) {
		tft.drawLine(0, tft.height()-1, x, 0, color);
	}
	for (uint16_t y=0; y < tft.height()-1; y+=6) {
		tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
	}

	tft.fillScreen(BLACK);
	for (uint16_t x=0; x < tft.width()-1; x+=6) {
		tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
	}
	for (uint16_t y=0; y < tft.height()-1; y+=6) {
		tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
	}

}

void testdrawtext(char *text, uint16_t color) {
	tft.setCursor(0,0);
	tft.setTextColor(color);
	tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
	tft.fillScreen(BLACK);
	for (uint16_t y=0; y < tft.height()-1; y+=5) {
		tft.drawFastHLine(0, y, tft.width()-1, color1);
	}
	for (uint16_t x=0; x < tft.width()-1; x+=5) {
		tft.drawFastVLine(x, 0, tft.height()-1, color2);
	}
}

void testdrawrects(uint16_t color) {
	tft.fillScreen(BLACK);
	for (uint16_t x=0; x < tft.height()-1; x+=6) {
		tft.drawRect((tft.width()-1)/2 -x/2, (tft.height()-1)/2 -x/2 , x, x, color);
	}
}

void testfillrects(uint16_t color1, uint16_t color2) {
	tft.fillScreen(BLACK);
	for (uint16_t x=tft.height()-1; x > 6; x-=6) {
		tft.fillRect((tft.width()-1)/2 -x/2, (tft.height()-1)/2 -x/2 , x, x, color1);
		tft.drawRect((tft.width()-1)/2 -x/2, (tft.height()-1)/2 -x/2 , x, x, color2);
	}
}

void testfillcircles(uint8_t radius, uint16_t color) {
	for (uint8_t x=radius; x < tft.width()-1; x+=radius*2) {
		for (uint8_t y=radius; y < tft.height()-1; y+=radius*2) {
			tft.fillCircle(x, y, radius, color);
		}
	}
}

void testdrawcircles(uint8_t radius, uint16_t color) {
	for (uint8_t x=0; x < tft.width()-1+radius; x+=radius*2) {
		for (uint8_t y=0; y < tft.height()-1+radius; y+=radius*2) {
			tft.drawCircle(x, y, radius, color);
		}
	}
}

void testtriangles() {
	tft.fillScreen(BLACK);
	int color = 0xF800;
	int t;
	int w = tft.width()/2;
	int x = tft.height();
	int y = 0;
	int z = tft.width();
	for(t = 0 ; t <= 15; t+=1) {
		tft.drawTriangle(w, y, y, x, z, x, color);
		x-=4;
		y+=4;
		z-=4;
		color+=100;
	}
}

void testroundrects() {
	tft.fillScreen(BLACK);
	int color = 100;

	int x = 0;
	int y = 0;
	int w = tft.width();
	int h = tft.height();
	for(int i = 0 ; i <= 24; i++) {
		tft.drawRoundRect(x, y, w, h, 5, color);
		x+=2;
		y+=3;
		w-=4;
		h-=6;
		color+=1100;
		Serial.println(i);
	}
}

void tftPrintTest() {
	tft.fillScreen(BLACK);
	tft.setCursor(0, 5);
	tft.setTextColor(RED);
	tft.setTextSize(1);
	tft.println("Hello World!");
	tft.setTextColor(YELLOW);
	tft.setTextSize(2);
	tft.println("Hello World!");
	tft.setTextColor(BLUE);
	tft.setTextSize(3);
	tft.print(1234.567);
	delay(1500);
	tft.setCursor(0, 5);
	tft.fillScreen(BLACK);
	tft.setTextColor(WHITE);
	tft.setTextSize(0);
	tft.println("Hello World!");
	tft.setTextSize(1);
	tft.setTextColor(GREEN);
	tft.print(p, 6);
	tft.println(" Want pi?");
	tft.println(" ");
	tft.print(8675309, HEX); // print 8,675,309 out in HEX!
	tft.println(" Print HEX!");
	tft.println(" ");
	tft.setTextColor(WHITE);
	tft.println("Sketch has been");
	tft.println("running for: ");
	tft.setTextColor(MAGENTA);
	tft.print(millis() / 1000);
	tft.setTextColor(WHITE);
	tft.print(" seconds.");
}

void mediabuttons() {
	// play
	tft.fillScreen(BLACK);
	tft.fillRoundRect(25, 10, 78, 60, 8, WHITE);
	tft.fillTriangle(42, 20, 42, 60, 90, 40, RED);
	delay(500);
	// pause
	tft.fillRoundRect(25, 90, 78, 60, 8, WHITE);
	tft.fillRoundRect(39, 98, 20, 45, 5, GREEN);
	tft.fillRoundRect(69, 98, 20, 45, 5, GREEN);
	delay(500);
	// play color
	tft.fillTriangle(42, 20, 42, 60, 90, 40, BLUE);
	delay(50);
	// pause color
	tft.fillRoundRect(39, 98, 20, 45, 5, RED);
	tft.fillRoundRect(69, 98, 20, 45, 5, RED);
	// play color
	tft.fillTriangle(42, 20, 42, 60, 90, 40, GREEN);
}

/**************************************************************************/
/*!
    @brief  Renders a simple test pattern on the screen
 */
/**************************************************************************/
void lcdTestPattern(void)
{
	static const uint16_t PROGMEM colors[] =
	{ RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA, BLACK, WHITE };

	for(uint8_t c=0; c<8; c++) {
		tft.fillRect(0, tft.height() * c / 8, tft.width(), tft.height() / 8,
				pgm_read_word(&colors[c]));
	}
}


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
