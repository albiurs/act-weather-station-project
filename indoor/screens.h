/*
   functions to call screens
 ************************************************************************
   - showScreenOne():   date & time
   - showScreenTwo():   indoor temperature & humidity
   - showScreenThree(): outdoor temperature & humidity
   - showScreenFour():  pressure
   - showScreenFive():  no outdoor values available i
   - showScreenSix():   frost alert
   - showScreenSeven(): storm alert
   - showScreenEight(): rain alert
*/
void showScreenZero() {
  // clear screen
  tft.fillScreen(WHITE);
  tft.drawXBitmap( 0, 0, zhawLogo_bits, zhawLogo_width, zhawLogo_height, ZHAWCOL);
  delay(SCRNDELAY);
  tft.fillScreen(BLACK);
  tft.drawXBitmap( 0, 0, title_bits, title_width, title_height, SCRNTHREECOL);
  delay(SCRNDELAY/2);
  tft.fillScreen(BLACK);
  tft.setCursor(5,5);
  tft.setTextColor(SCRNONECOL);
  tft.setTextSize(1);
  tft.println("Arduino Project by");
  tft.println();
  tft.setTextColor(WHITE);
  tft.setCursor(5,30);
  tft.print("Urs Albisser");
  tft.setCursor(5,45);
  tft.print("Regula Engelhardt");
  tft.setCursor(5,60);
  tft.print("Bernhard Kaempf");
  tft.setCursor(5,75);
  tft.print("Serge Kaulitz");
  tft.setCursor(20,100);
  tft.print("26. Oktober 2019");
}
void showScreenOne() {
  /* Screen ONE date & time
   ************************************************************
     time (format: hh:mm)
     date (format: month & dow in plain text German)
  */
  // clear screen
  tft.fillScreen(BLACK);
  // show time in upper half of screen
  tft.drawXBitmap( 0, 10, clock_bits, clock_width, clock_height, SCRNONECOL);
  tft.setCursor(50, 10);
  tft.setTextColor(SCRNONECOL);
  tft.setTextSize(1);
  tft.print("Uhr");

  tft.setCursor(50, 25);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  char buf[5];
  sprintf(buf, "%02d:%02d", Clock.getHour(h12, PM), Clock.getMinute());
  tft.print(buf);

  // show date in bottom half of screen
  tft.drawXBitmap( 0, 70, calendar_bits, calendar_width, calendar_height, SCRNONECOL);
  tft.setCursor(50, 70);
  tft.setTextColor(SCRNONECOL);
  tft.setTextSize(1);
  tft.print(monthString[Clock.getMonth(Century) - 1]);
  //tft.print("20");
  //tft.print(Clock.getYear(), DEC);



  tft.setCursor(50, 85);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(dowString[Clock.getDoW() - 1]);
  tft.print(" ");
  tft.print(Clock.getDate(), DEC);
  tft.print(".");
  tft.setCursor(50, 105);
  tft.print("20");
  tft.print(Clock.getYear(), DEC);



}

void showScreenTwo() {
  /* Screen TWO indoor temperature & humidity
  ************************************************************
  */
  // clear screen
  tft.fillScreen(BLACK);
  // show indoor temperature in upper half of screen
  tft.setCursor(0, 5);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.println("Indoor");

  tft.drawXBitmap( 0, 20, temphigh_bits, temphigh_width, temphigh_height, SCRNTWOCOL);
  tft.setCursor(50, 20);
  tft.setTextColor(SCRNTWOCOL);
  tft.println("Grad Celsius");
  tft.setCursor(50, 35);
  // Print values to OLED
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(dht.readTemperature());

  // show indoor humidity in bottom half of screen
  tft.drawXBitmap( 0, 80, humidity_bits, humidity_width, humidity_height, SCRNTWOCOL);
  tft.setCursor(50, 80);
  tft.setTextColor(SCRNTWOCOL);
  tft.setTextSize(1);
  tft.println("Prozent (%)");

  tft.setCursor(50, 95);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(dht.readHumidity());



}

void showScreenThree() {
  /* Screen THREE outdoor temperature & humidity
  ************************************************************
    show only when data available
  */


  // clear screen
  tft.fillScreen(BLACK);

  // show outdoor temperature in upper half of screen
  tft.setCursor(0, 5);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.println("Outdoor");

  tft.drawXBitmap( 0, 20, temphigh_bits, temphigh_width, temphigh_height, SCRNTHREECOL);
  tft.setCursor(50, 20);
  tft.setTextColor(SCRNTHREECOL);
  tft.println("Grad Celsius");

  tft.setCursor(50, 35);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(_radioData.outTemp);

  // show outdoor humidity in lower half of screen
  tft.drawXBitmap( 0, 80, humidity_bits, humidity_width, humidity_height, SCRNTHREECOL);
  tft.setCursor(50, 80);
  tft.setTextColor(SCRNTHREECOL);
  tft.setTextSize(1);
  tft.println("Prozent (%)");

  tft.setCursor(50, 95);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(_radioData.outHum);


}

void showScreenFour() {
  /* Screen FOUR pressure
  ************************************************************
  */

  // clear screen
  tft.fillScreen(BLACK);

  // show pressure in upper half of screen
  tft.setCursor(0, 5);
  tft.setTextColor(SCRNFOURCOL);
  tft.setTextSize(1);
  tft.println("Luftdruck");

  tft.setCursor(100, 20);
  tft.setTextColor(SCRNFOURCOL);
  tft.print("hPa");
  tft.setCursor(0, 30);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);


  tft.print(_radioData.outPress);

  if (_radioData.outPress < 1000) {
    //-980 sehr tief, stürmisch - 980-1000 tief, regnerisch
    tft.drawXBitmap( 34, 60, stormy_bits, stormy_width, stormy_height, STORMYCOL);
  } else if (_radioData.outPress < 1020) {
    // 1000-1020 normal, wechselhaft
    tft.drawXBitmap( 34, 60, cloudy_bits, cloudy_width, cloudy_height, CLOUDYCOL);
  } else {
    // 1020-1040 hoch, sonnig 1040- sehr hoch, sehr trocken
    tft.drawXBitmap( 34, 60, sunny_bits, sunny_width, sunny_height, SUNNYCOL);
  }

  if (pressureOld < pressure) {
    tft.drawXBitmap( 100, 35, arrowUp_bits, arrowUp_width, arrowUp_height, ARROWUPCOL);
  } else {
    tft.drawXBitmap( 100, 35, arrowDown_bits, arrowDown_width, arrowDown_height, ARROWDOWNCOL);
  }
  pressureOld = pressure;


}

void showScreenFive() {
  /* Screen FIVE show info
  ************************************************************
    no outdoor values available
  */
  tft.fillScreen(BLACK);
  tft.drawXBitmap( 10, 10, info_bits, info_width, info_height, INFOCOL);
  tft.drawXBitmap( 65, 10, nodata_bits, nodata_width, nodata_height, WARNINGCOL);
  tft.setCursor(10, 60);
  tft.setTextSize(1);
  tft.print("Keine Outdoor-Daten");
  tft.setCursor(10, 80);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("erwarte");
  tft.setCursor(10, 100);
  tft.print("Daten");


}

void showScreenSix() {
  tft.fillScreen(BLACK);
  tft.drawXBitmap( 10, 10, warning_bits, warning_width, warning_height, WARNINGCOL);
  tft.drawXBitmap( 65, 10, frost_bits, frost_width, frost_height, FROSTCOL);
  tft.setCursor(10, 60);
  tft.setTextSize(1);
  tft.print("Temperatur < 0 Grad");
  tft.setCursor(10, 80);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Achtung");
  tft.setCursor(10, 100);
  tft.print("Frost!");

}

void showScreenSeven() {
  tft.fillScreen(BLACK);
  tft.drawXBitmap( 10, 10, warning_bits, warning_width, warning_height, WARNINGCOL);
  tft.drawXBitmap( 65, 10, storm_bits, storm_width, storm_height, STORMCOL);
  tft.setCursor(10, 60);
  tft.setTextSize(1);
  tft.print("Tiefer Luftdruck");
  tft.setCursor(10, 80);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Achtung");
  tft.setCursor(10, 100);
  tft.print("Sturm!");

}

void showScreenEight() {
  tft.fillScreen(BLACK);
  tft.drawXBitmap( 10, 10, umbrella_bits, umbrella_width, umbrella_height, WHITE);
  if (_radioData.outRain > 1) {
    tft.drawXBitmap( 65, 10, storm_bits, storm_width, storm_height, STORMCOL);
  } else {
    tft.drawXBitmap( 65, 10, rainy_bits, rainy_width, rainy_height, RAINYCOL);
  }
  tft.setCursor(10, 60);
  tft.setTextSize(1);
  if (_radioData.outRain < 1) {
    tft.print("starker Regen");
  } else {
    tft.print("leichter Regen");
  }
  tft.setCursor(10, 80);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Schirm");
  tft.setCursor(10, 100);
  tft.print("mitnehmen");

}
