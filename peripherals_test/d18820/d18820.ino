/*deps: 
- OneWire by Jim Studt, Tom Pollard, Robin James, Glenn Trewitt, Jason Dangel, Guillermo Lovato, Paul Stoffregen, Scott Roberts, Bertrik Sikken, Mark Tillotson, Ken Butcher, Roger Clark, Love Nystrom
- DallasTemperature by Miles Burton <miles@mnetcs.com>, Tim Newsome <nuisance@casualhacker.net>, Guil Barros <gfbarros@bappos.com>, Rob Tillaart <rob.tillaart@gmail.com>
*/

/*
obs: a linha de dados (amarela) precisa de resistor (~4kOhm) ligando ao Vcc. Nosso circuito está montado errado (verificado dia 28/04)
*/

//testado com arduino mega 2560
//retirado de https://curtocircuito.com.br/blog/Categoria%20Arduino/como-utilizar-o-ds18b20

#include "OneWire.h"
#include "DallasTemperature.h"

OneWire tempCommWire(2);

DallasTemperature tempSensor(&tempCommWire);

void setup() {
  Serial.begin(9600);
  tempSensor.begin();  
}

void loop() {
    tempSensor.requestTemperatures();
    Serial.println(tempSensor.getTempCByIndex(0));
}
