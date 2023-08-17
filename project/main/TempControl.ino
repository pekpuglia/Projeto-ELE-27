#include "OneWireSTM.h"
#include "DallasTemperature.h"

unsigned long ulTimeTempControl;

//dados p transmitir
struct TempControlData
{
    float internalTempC;
    float externalTempC;
    long timestamp_millis;
    bool control;
};

/*
    Constantes derivadas dos requisitos
*/

//freq medida e armazenamento
const long internalTemperatureMeasurementRateMillis = 5000;
const float internalTemperatureGoal = 15.0;
const float internalTemperatureTolerance = 2.0;

/*
    Lógica de controle
*/
bool isOn = false;

//calcula o comando necessário
bool controlLogic(float measuredTemp) {
    if (measuredTemp < internalTemperatureGoal - internalTemperatureTolerance)
        return true;
    if (measuredTemp > internalTemperatureGoal + internalTemperatureTolerance)
        return false;

    return isOn;
}

/*
    Sensores de temperatura
*/

//colocar pinos certos!!!!!!
OneWire payloadTempWire(PB8);
OneWire externalTempWire(PB9);

DallasTemperature payloadTempSensor(&payloadTempWire);
DallasTemperature externalTempSensor(&externalTempWire);

float internalTemperature() {
    payloadTempSensor.requestTemperatures();
    return payloadTempSensor.getTempCByIndex(0);
}

float externalTemperature() {
    externalTempSensor.requestTemperatures();
    return externalTempSensor.getTempCByIndex(0);
}

/*
    Controle de potência
*/

//colocar pino certo!!!
const uint8 transistorControlPin = 0;

void controlOutput() {
    if (isOn)
        digitalWrite(transistorControlPin, HIGH);
    else
        digitalWrite(transistorControlPin, LOW);
}