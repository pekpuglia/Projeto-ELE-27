#ifdef BAROMETRO
MS5611 ms5611;   // 0x76 = CSB to VCC; 0x77 = CSB to GND

int BarometroSetup()
{
  // Initialize MS5611 sensor
  // Ultra high resolution: MS5611_ULTRA_HIGH_RES
  // (default) High resolution: MS5611_HIGH_RES
  // Standard: MS5611_STANDARD
  // Low power: MS5611_LOW_POWER
  // Ultra low power: MS5611_ULTRA_LOW_POWER
  //  while (!ms5611.begin(MS5611_ULTRA_HIGH_RES))
  DEB.print("Starting Barometro ...");
  ms5611.begin(MS5611_STANDARD);   // Library in: https://github.com/jarzebski/Arduino-MS5611
  DEB.println("Barometro OK ...");
  return (0); //Indica processo de iniciação OK
}

void BarometroRead()
{
  // Read true temperature & Pressure (without compensation)
  double realTemperature = ms5611.readTemperature();
  long realPressure = ms5611.readPressure();
  double realAltitude = ms5611.getAltitude(realPressure);

  // Read true temperature & Pressure (with compensation)
  double realTemperature2 = ms5611.readTemperature(true);
  long realPressure2 = ms5611.readPressure(true);
  double realAltitude2 = ms5611.getAltitude(realPressure2);

  stPayloadData.dMS5611Temp = ms5611.readTemperature(true);
  stPayloadData.dMS5611Press = ms5611.readPressure(true);
  stPayloadData.dMS5611Alt = ms5611.getAltitude(stPayloadData.dMS5611Press); // using compensation
}
#endif
