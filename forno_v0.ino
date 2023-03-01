/*
Teste de pinos ESP WROOM32
*/

#include <Wire.h>
#include <SPI.h>
#include <max6675.h>

int SO = 23;
int CS = 24;
int sck = 25;
MAX6675 module(sck, CS, SO);

void setup() {   
  Serial.begin(115200);
}

void loop() {
  float temperature = module.readCelsius(); 
  Serial.print("Temperature: " + temperature);
  Serial.println(F("°C "));   
  
}