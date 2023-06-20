/*deps: 
- OneWire by Jim Studt, Tom Pollard, Robin James, Glenn Trewitt, Jason Dangel, Guillermo Lovato, Paul Stoffregen, Scott Roberts, Bertrik Sikken, Mark Tillotson, Ken Butcher, Roger Clark, Love Nystrom
- DallasTemperature by Miles Burton <miles@mnetcs.com>, Tim Newsome <nuisance@casualhacker.net>, Guil Barros <gfbarros@bappos.com>, Rob Tillaart <rob.tillaart@gmail.com>
*/

/*
obs: a linha de dados (amarela) precisa de resistor (~4kOhm) ligando ao Vcc.
*/

//testado com stm
//retirado de https://curtocircuito.com.br/blog/Categoria%20Arduino/como-utilizar-o-ds18b20

#include "OneWireSTM.h"
#include "DallasTemperature.h"

OneWire payloadTempWire(PB8);
OneWire externalTempWire(PB9);


DallasTemperature payloadTempSensor(&payloadTempWire);
DallasTemperature externalTempSensor(&externalTempWire);


void setup() {
  Serial1.begin(9600);
  payloadTempSensor.begin();  
  externalTempSensor.begin();
}

void loop() {
    payloadTempSensor.requestTemperatures();
    externalTempSensor.requestTemperatures();
    Serial1.print(payloadTempSensor.getTempCByIndex(0));
    Serial1.print(" ");
    Serial1.println(externalTempSensor.getTempCByIndex(0));
}
