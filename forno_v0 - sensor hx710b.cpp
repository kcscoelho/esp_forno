/*
Componentes utilizados:
- ESP WROOM32
- Sensor de pressão modelo MPS20N0040D HX710B

Conexões:
Sensor de pressão modelo MPS20N0040D / HX710B
Sensor 1 - Entrada
SCK – Pino 32
OUT – Pino 34
GND – negativo
VCC – positivo

Sensor 2 - Saída
SCK – Pino 33
OUT – Pino 35
GND – negativo
VCC – positivo

Bibliotecas:
- Q2HX711
https://github.com/queuetue/Q2-HX711-Arduino-Library

*/
#include "Wire.h"
#include "SPI.h"
#include "Q2HX711.h"

float pressure1;
float pressure2;

// Pressão
const byte MPS_1_SCK_pin = 32; // clock data pin
const byte MPS_1_OUT_pin = 34; // OUT data pin
const byte MPS_2_SCK_pin = 33; // clock data pin
const byte MPS_2_OUT_pin = 35; // OUT data pin
int avg_size = 10; // # pts to average over

Q2HX711 MPS20N0040D_1(MPS_1_OUT_pin, MPS_1_SCK_pin); // start comm with the HX710B Sensor 1
Q2HX711 MPS20N0040D_2(MPS_2_OUT_pin, MPS_2_SCK_pin); // start comm with the HX710B Sensor 2

void getReadings(){
  // PRESSAO SENSOR 1 ENTRADA
  pressure1 = 0.0; // variable for averaging
  for (int ii=0;ii<avg_size;ii++){
    pressure1 += MPS20N0040D_1.read(); // add multiple ADC readings
    delay(50); // delay between readings
  }
  pressure1 /= avg_size;
  Serial.print("Pressao1: ");
  Serial.println(pressure1,0); // print out the average

  // PRESSAO SENSOR 2 SAIDA
  pressure2 = 0.0; // variable for averaging
  for (int ii=0;ii<avg_size;ii++){
    pressure2 += MPS20N0040D_2.read(); // add multiple ADC readings
    delay(50); // delay between readings
  }
  pressure2 /= avg_size;
  Serial.print("Pressao2: ");
  Serial.println(pressure2,0); // print out the average
}

void setup() {
  Serial.begin(115200);
}

void loop(void) {
  getReadings();
  delay(3000);
}