
//quais pinos funcionam no analogwrite???
//PA3 funciona
//PA15 diz que funciona mas não funciona

int controlPin = PB4;

#include "OneWireSTM.h"
#include "DallasTemperature.h"

OneWire payloadTempWire(PB8);
OneWire externalTempWire(PB9);


DallasTemperature payloadTempSensor(&payloadTempWire);
DallasTemperature externalTempSensor(&externalTempWire);

void setup() {
    pinMode(controlPin, OUTPUT);
    Serial1.begin(9600);
    payloadTempSensor.begin();  
    externalTempSensor.begin();
}

void loop() {
    analogWrite(controlPin, 255);
    payloadTempSensor.requestTemperatures();
    externalTempSensor.requestTemperatures();
    Serial1.print(payloadTempSensor.getTempCByIndex(0));
    Serial1.print(" ");
    Serial1.println(externalTempSensor.getTempCByIndex(0));
}