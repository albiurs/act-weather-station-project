# act-weather-station-project
Arduino-based weather station project; Atomic Clock Team (ACT), School of Engineering, ZHAW Zurich University of Applied Sciences, Switzerland


# Hardware
## Indoor Module
component | usage | image
------------ | ------------- | -------------
[Arduino Mega 2560 REV3](https://store.arduino.cc/usa/mega-2560-r3)| basic board | <img src="https://botland.com.pl/55883-thickbox_default/arduino-mega-2560-rev3-module-a000067.jpg" alt="drawing" width="100"/>
[Adafruit OLED Breakout Board 16-bit Color 1.5" w/microSD h](https://www.digitec.ch/de/s1/product/adafruit-oled-breakout-board-16-bit-color-15-wmicrosd-h-elektronikmodul-5998537)| OLED Display | <img src="https://cdn-learn.adafruit.com/guides/cropped_images/000/000/348/medium640/1431flower_LRG.jpg" alt="drawing" width="100"/>
[Adafruit DHT22 temperature-humidity sensor](https://www.adafruit.com/product/385) | sensor for indoor temperature and humidity | <img src="https://cdn-shop.adafruit.com/970x728/385-00.jpg" alt="drawing" width="100"/>
[DS3231 breakout - Präzisions-Echtzeituhr DS3231, Adafruit](https://www.distrelec.ch/de/praezisions-echtzeituhr-ds3231-adafruit-3013-ds3231-breakout/p/30091211) | real time clock | <img src="https://cdn-learn.adafruit.com/guides/cropped_images/000/001/231/medium640/thump.jpg?1520543977" alt="drawing" width="100"/>
[NRF24L01P-MODULE-PCB - Modul mit PCB Antenne, Nordic Semiconductor](https://www.distrelec.ch/de/modul-mit-pcb-antenne-nordic-semiconductor-nrf24l01p-module-pcb/p/17328568?queryFromSuggest=true) | wireless transiever | <img src="http://robotechshop.com/wp-content/uploads/2016/01/nrf2401.jpg" alt="drawing" width="100"/>

 
## Outdoor Module
component | usage | image
------------ | ------------- | -------------
[Arduino Mega 2560 REV3](https://store.arduino.cc/usa/mega-2560-r3)| basic board | <img src="https://botland.com.pl/55883-thickbox_default/arduino-mega-2560-rev3-module-a000067.jpg" alt="drawing" width="100"/>
[Adafruit BME280 I2C/SPI Temperature Humidity Pressure Sensor](https://www.play-zone.ch/de/adafruit-bme280-i2c-spi-temperature-humidity-pressure-sensor.html) | sensor for indoor temperature and humidity | <img src="https://www.play-zone.ch/media/catalog/product/cache/1/image/650x/0ae87949542eeaf1748dd8a48e1c5f15/2/6/2652-00.jpg" alt="drawing" width="100"/>
[NRF24L01P-MODULE-PCB - Modul mit PCB Antenne, Nordic Semiconductor](https://www.distrelec.ch/de/modul-mit-pcb-antenne-nordic-semiconductor-nrf24l01p-module-pcb/p/17328568?queryFromSuggest=true) | wireless transiever | <img src="http://robotechshop.com/wp-content/uploads/2016/01/nrf2401.jpg" alt="drawing" width="100"/>

# Software
## Indoor Module
### Libraries
REQUIRES the following Arduino libraries:
* DHT_sensor_library: [v1.3.7	DHT22 Sensor](https://github.com/adafruit/DHT-sensor-library)
* Adafruit_Unified_Sensor: [v1.0.3	Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor)
* Wire: I2C interface library (Arduino built-in)
* DS3231: [v1.0.2	RTC library](https://github.com/NorthernWidget/DS3231)
* Adafruit GFX Library: [v1.5.6	Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
* SSD1351:  [v1.2.2	Adafruit SSD1351 for the 1.5" 16-bit Color OLED with SSD1351 driver chip](https://github.com/adafruit/Adafruit-SSD1351-library)
* SPI: SPI Master library for Arduino (Arduino built-in)
* NRFLite: [2.2.2	nRF24L01+ 2.4 GHz Transceiver library](https://github.com/dparson55/NRFLite)
* U8g2lib: [Arduino wrapper for the u8g2 struct and c functions for the u8g2 - 8bit graphics library(https://github.com/olikraus/u8g2/)
## Outdoor Module
