/*
Teste de pinos ESP WROOM32
*/

#include <Wire.h>
#include <SPI.h>
#include <max6675.h>

int SO = 16;
int CS = 17;
int sck = 18;
MAX6675 module(sck, CS, SO);

float temperature;

void setup() {   
  Serial.begin(115200);
}

void loop() {
  temperature = module.readCelsius(); 
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(F("°C "));
  delay(3000);
}