#include <ArduinoIoTCloud.h>
#include <WiFiConnectionManager.h>

const char THING_ID[] = "b2504fe2-192c-4f2b-82aa-dff8184c089d";

const char SSID[]     = SECRET_SSID;    // Network SSID (name)
const char PASS[]     = SECRET_PASS;    // Network password (use for WPA, or use as key for WEP)


float temperature;
float pressure;
float humidity;
float light_intensity;

void initProperties(){

  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(temperature, READ, 2 * SECONDS, NULL);
  ArduinoCloud.addProperty(pressure, READ, 2 * SECONDS, NULL);
  ArduinoCloud.addProperty(humidity, READ, 2 * SECONDS, NULL);
  ArduinoCloud.addProperty(light_intensity, READ, 2 * SECONDS, NULL);

}

ConnectionManager *ArduinoIoTPreferredConnection = new WiFiConnectionManager(SSID, PASS);
