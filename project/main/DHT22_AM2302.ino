#ifdef DHT22_DEF
// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

/****************************************************************************/
/* DHT22_AM2302 Definitions                                                 */
/****************************************************************************/
#define DHTPIN PA8     // Digital pin connected to the DHT sensor 
// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT dht(DHTPIN, DHTTYPE);

uint32_t delayMS = 500;

void Dht22Setup()
{
  // Initialize device.
  dht.begin();
  ulTimeRD_DHT22 = millis();
}

void Dht22Read()
{
  stPayloadData.fDHT22Hum = dht.readHumidity();
  stPayloadData.fDHT22Temp = dht.readTemperature();
  if (isnan(stPayloadData.fDHT22Hum) || isnan(stPayloadData.fDHT22Temp)) DEB.println("Falha na leitura do sensor DHT22");
  return;
}
#endif
