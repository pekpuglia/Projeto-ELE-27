
//quais pinos funcionam no analogwrite???
//PA3 funciona
//PA15 diz que funciona mas não funciona
void setup() {
    pinMode(PA3, OUTPUT);
}

void loop() {
    analogWrite(PA3, 255);
}