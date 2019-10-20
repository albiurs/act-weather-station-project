/*
   Arduino Weather Station Project
   Sketch for indoor module
   REQUIRES the following Arduino libraries:
   - DHT_sensor_library		   v1.3.7	DHT22 Sensor: https://github.com/adafruit/DHT-sensor-library
   - Adafruit_Unified_Sensor v1.0.3	Adafruit Unified Sensor: https://github.com/adafruit/Adafruit_Sensor
   - Wire						   	     I2C interface library (Arduino built-in)
   - DS3231					         v1.0.2	RTC library: https://github.com/NorthernWidget/DS3231
   - Adafruit GFX Library		 v1.5.6	Adafruit GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
   - SSD1351 					       v1.2.2	Adafruit SSD1351 for the 1.5" 16-bit Color OLED with SSD1351 driver chip: https://github.com/adafruit/Adafruit-SSD1351-library
 	 - SPI						         SPI Master library for Arduino (Arduino built-in)
 	 - NRFLite					       2.2.2	nRF24L01+ 2.4 GHz Transceiver library: https://github.com/dparson55/NRFLite
   - U8g2lib                 Bitmaps
*/

/*
   Include libraries
*/
#include "Arduino.h"
#include "DHT.h"				      // DHT sensor library
#include <DS3231.h>			      // RTC DS3231 library
#include <Wire.h>			      	// I2C wire interface library
#include <Adafruit_GFX.h>		  // Adafruit 1.5" 16-bit Color OLED library
#include <Adafruit_SSD1351.h>	// Adafruit 1.5" 16-bit Color OLED  Driver
#include <SPI.h>			      	// SPI Master library for Arduino (required by Adafruit_GFX.h and NRFLite.h)
#include <NRFLite.h>			    // nRF24L01+ Transceiver library
#include <U8g2lib.h>          // Bitmaps

/*
   Include bitmaps (char arrays)
*/
#include "xmbBits.h" // Bitmaps

/*
  Include functions for screen displays
*/
#include "screens.h"

/*
   Debug initialize
*/
#define DBUG // activate "debug switch"
#ifdef  DBUG
#define DBUG_PRINT(x) Serial.print(x)				// activate print
#define DBUG_PRINT2ARGS(x, y) Serial.print(x, y)	// activate print with two arguments
#define DBUG_PRINTLN(x) Serial.println(x)			// activate print line
#define DBUG_PRINTLN2ARGS(x, y) Serial.println(x, y)// activate print line with two arguments
#else
#define DBUG_PRINT(x)								// do nothing
#define DBUG_PRINT2ARGS(x, y)						// do nothing
#define DBUG_PRINTLN(x)								// do nothing
#define DBUG_PRINTLN2ARGS(x, y)						// do nothing
#endif

/*
   declare constants
*/
int const SCRNDELAY = 10000; // delay between screen-changes in msec

/*
   declare variables
*/
float pressure;
bool showWelcomeScreens = true;
char dowString[7][3] = {
  "Mo", "Di", "Mi", "Do", "Fr", "Sa", "So"
};

char monthString[12][10] = {
  "Januar", "Februar", "Maerz", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"
};

/*
   declare functions
*/
void showScreenZero();
void showScreenOne();
void showScreenTwo();
void showScreenThree();
void showScreenFour();
void showScreenFive();
void showScreenSix();
void showScreenSeven();
void showScreenEight();

/*
   DHT initialize
*/
#define DHTPIN 8			    // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22		  // DHT type = DHT22
DHT dht(DHTPIN, DHTTYPE);	// Initialize DHT sensor.

/*
   RTC DS3231 initialize
*/
DS3231 Clock;		// initialize DS3231 RTC
bool Century = false;
bool h12 = false;
bool PM = false;

/*
   Adafruit 1.5" 16-bit Color OLED Initialize
*/
// Screen dimensions:
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128

#define SCLK_PIN 2
#define MOSI_PIN 3
#define DC_PIN   4
#define CS_PIN   5
#define RST_PIN  6

// Color definitions:
// http://www.barth-dev.de/online/rgb565-color-picker/
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define ARROWDOWNCOL    0xDAC0 //red
#define ARROWUPCOL      0x2C60 //green
#define SUNNYCOL        0xFEAF
#define STORMYCOL       0x9492
#define STORMCOL        0xFA1F
#define CLOUDYCOL       0x0492
#define RAINYCOL        0x0492
#define WARNINGCOL      0xF920
#define FROSTCOL        0x77FF
#define ZHAWCOL         0x0314 // blue
#define SCRNONECOL      0xFC80 // nice brown
#define SCRNTWOCOL      0xDAC0 // nice red
#define SCRNTHREECOL    0x0300 // nice green
#define SCRNFOURCOL     0x7B51
#define INFOCOL         0x55DD

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);

/*
   nRF24L01+ Radio RX initialize
*/
const static uint8_t RADIO_ID = 0;       // Our radio's id. The transmitter will send to this id.
const static uint8_t PIN_RADIO_CE = 14;
const static uint8_t PIN_RADIO_CSN = 15;

struct RadioPacket // package up to 32 bytes to be sent/received.
{
  float outTemp;
  float outHum;
  float outPress;
  float outRain;
};

NRFLite _radio;
RadioPacket _radioData;


/*****************
   ARDUINO SETUP
 ****************/
void setup()
{

  /*
     Start serial port & serial monitor to Baud rate 9600 bits/s
  */
  DBUG_PRINTLN("Start serial monitor");
  Serial.begin(9600); // serial return of Arduino's output


  /*
     Start the I2C interface
  */
  DBUG_PRINTLN("Start the I2C interface");
  Wire.begin();

  /*
     Start DHT measurement
  */
  DBUG_PRINTLN(F("Start DHT measurement"));
  dht.begin();


  /*
     Start RTC > setup date & time
  */
  // VERYIMPORTANT COMMENTS BELOW!!!
  // If you want to set the time, change these numbers to the date and time
  // you want to set to, and then upload it to the arduino.
  //
  // once you have finished setting the time, comment out the following clock.set functions
  // and then re-upload it to the board. Otherwise your clock will reset
  // every time you open the serial monitor.

  //Clock.setSecond(10);//Set the second
  /*
    Clock.setMinute(56);//Set the minute
    Clock.setHour(15);  //Set the hour

    Clock.setDoW(6);    //Set the day of the week (1 for Monday through 7 for Sunday)
    Clock.setDate(5);  //Set the date of the month
    Clock.setMonth(10);  //Set the month of the year
    Clock.setYear(19);  //Set the year (Last two digits of the year)
  */


  /*
     Start OLED
  */
  DBUG_PRINTLN("Start OLED");
  tft.begin();


  /*
     Start nRF24L01+ Radio
  */
  DBUG_PRINTLN("Start Radio");
  _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100); // THE DEFAULT

  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
  {
    DBUG_PRINTLN("Cannot communicate with radio");
  }

}

/****************
   ARDUINO LOOP
 ***************/
void loop() {

  /*
     RTC read
  */
  // send what's going on to the serial monitor.
  // Start with the year
  DBUG_PRINT("2");
  if (Century) {			// Won't need this for 89 years.
    DBUG_PRINT("1");
  } else {
    DBUG_PRINT("0");
  }
  DBUG_PRINT2ARGS(Clock.getYear(), DEC);
  DBUG_PRINT(' ');
  // then the month
  DBUG_PRINT2ARGS(Clock.getMonth(Century), DEC);
  DBUG_PRINT(' ');
  // then the date
  DBUG_PRINT2ARGS(Clock.getDate(), DEC);
  DBUG_PRINT(' ');
  // and the day of the week
  DBUG_PRINT2ARGS(Clock.getDoW(), DEC);
  DBUG_PRINT(' ');
  // Finally the hour, minute, and second
  DBUG_PRINT2ARGS(Clock.getHour(h12, PM), DEC);
  DBUG_PRINT(' ');
  DBUG_PRINT2ARGS(Clock.getMinute(), DEC);
  DBUG_PRINT(' ');
  DBUG_PRINT2ARGS(Clock.getSecond(), DEC);




  /*
     DHT Sensor Read
  */
  DBUG_PRINT("Feuchtigkeit Indoor: ");
  DBUG_PRINTLN(dht.readHumidity());
  DBUG_PRINT("Temperatur Indoor (Grad Celsius): ");
  DBUG_PRINTLN(dht.readTemperature());



  // Check if any reads failed and exit early (to try again).
  if (isnan(dht.readTemperature()) || isnan(dht.readHumidity())) {
    DBUG_PRINTLN(F("Failed to read from DHT sensor!"));
    return;
  }

  DBUG_PRINTLN("Indoor:");
  DBUG_PRINT(F("Temperature: "));
  DBUG_PRINT(dht.readTemperature());
  DBUG_PRINTLN(F(" °C"));
  DBUG_PRINT(F("Humidity: "));
  DBUG_PRINT(dht.readHumidity());
  DBUG_PRINTLN(F(" %"));
  DBUG_PRINTLN();
  DBUG_PRINTLN();

  /*
     nRF24L01+ Radio RX
  */
  DBUG_PRINT("radio has data: ");
  DBUG_PRINTLN(_radio.hasData());
  while (_radio.hasData()) {
    _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.
  }

  DBUG_PRINTLN("Outdoor:");
  DBUG_PRINT(F("Temperature: "));
  DBUG_PRINT(_radioData.outTemp);
  DBUG_PRINTLN(F(" °C"));

  DBUG_PRINT(F("Humidity: "));
  DBUG_PRINT(_radioData.outHum);
  DBUG_PRINTLN(F(" %"));

  DBUG_PRINT(F("Pressure: "));
  DBUG_PRINT(_radioData.outPress);
  DBUG_PRINTLN(F(" hPa"));

  DBUG_PRINT(F("Rain: "));
  DBUG_PRINT(_radioData.outRain);
  DBUG_PRINTLN(F(" (Range from 0 to 2, 0 = dry)"));


  /*
     show screens
  */
  if(showWelcomeScreens){
    showWelcomeScreens = false;
    showScreenZero();    // Welcome
    delay(SCRNDELAY);
  }
  showScreenOne();     // date & time
  delay(SCRNDELAY);
  showScreenTwo();     // indoor temperature & humidity
  delay(SCRNDELAY);
  if (_radio.hasData() > 0 && _radioData.outPress > 0 && _radioData.outHum > 0) { // show outdoor values when available
    showScreenThree(); // outdoor temperature & humidity
    delay(SCRNDELAY);
    showScreenFour();  // pressure
    delay(SCRNDELAY);
    if (_radioData.outRain > 0) {
      DBUG_PRINTLN("radio has data....");   
      showScreenEight();  // rain alert
      delay(SCRNDELAY);
    }
    if (_radioData.outTemp < 0) {
      showScreenSix();   // frost alert
      delay(SCRNDELAY);
    }
    if (_radioData.outPress <= 980) {
      showScreenSeven();  // storm alert
      delay(SCRNDELAY);
    }
  } else {
    showScreenFive();  // info "no connection to outdoor module"
    delay(SCRNDELAY);
  }
}

