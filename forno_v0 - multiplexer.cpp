/*
Componentes utilizados:
- ESP WROOM32
- Termo par tipo K modelo MAX6675
- Multiplexador 74HC4067 / HP4067 CMOS 16 Canais

Conexões:
Sensor de temperatura termo par tipo K, modelo MAX6675
S0  – Pino 22
SCK – Pino 23
CS  – Pino 21 através das entradas do Multiplexer
GND – negativo
VCC – positivo

Obs: para multiplexar os sensores de temperatura, utilizar a seguinte conexão:
- Todos os SCKs no pino 23.
- Todos os S0 no pino 22.
- CS de cada sensor numa porta do MUX.

MUX, Multiplexador 74HC4067 / HP4067 CMOS 16 Canais
GND – negativo
VCC – positivo
S0 - Pino 19
S1 - Pino 18
S2 - Pino 17
S3 - Pino 16
SIG - Pino 21 - chaveia esse pino com a saída do multiplexer
EN - negativo

Bibliotecas:
- MAX6675
https://blogmasterwalkershop.com.br/arquivos/libs/MAX6675-library-master.zip

*/
#include "Wire.h"
#include "SPI.h"
#include "max6675.h"

#define MUX_S0 19
#define MUX_S1 18
#define MUX_S2 17
#define MUX_S3 16
#define MUX_SIG 21

float temp1;
float temp2;
float temp3;
float temp4;
float temp5;
float temp6;
float temp7;
float temp8;
float temp9;
float temp10;
float temp11;
float temp12;
float temp13;
float temp14;
float temp15;
float temp16;

// Tempo par
int SO = 22;
int sck = 23;
MAX6675 module(sck, MUX_SIG, SO);

void mux(int circuit){

  if (circuit == 1) {
  digitalWrite(MUX_S0, LOW);
  digitalWrite(MUX_S1, LOW);
  digitalWrite(MUX_S2, LOW);
  digitalWrite(MUX_S3, LOW);
  }

  if (circuit == 2) {
  digitalWrite(MUX_S0, LOW);
  digitalWrite(MUX_S1, LOW);
  digitalWrite(MUX_S2, LOW);
  digitalWrite(MUX_S3, HIGH);
  }

  if (circuit == 3) {
  digitalWrite(MUX_S0, LOW);
  digitalWrite(MUX_S1, LOW);
  digitalWrite(MUX_S2, HIGH);
  digitalWrite(MUX_S3, LOW);
  }

  if (circuit == 4) {
  digitalWrite(MUX_S0, LOW);
  digitalWrite(MUX_S1, LOW);
  digitalWrite(MUX_S2, HIGH);
  digitalWrite(MUX_S3, HIGH);
  }

  if (circuit == 5) {
  digitalWrite(MUX_S0, LOW);
  digitalWrite(MUX_S1, HIGH);
  digitalWrite(MUX_S2, LOW);
  digitalWrite(MUX_S3, LOW);
  }

  if (circuit == 6) {
  digitalWrite(MUX_S0, LOW);
  digitalWrite(MUX_S1, HIGH);
  digitalWrite(MUX_S2, LOW);
  digitalWrite(MUX_S3, HIGH);
  }

  if (circuit == 7) {
  digitalWrite(MUX_S0, LOW);
  digitalWrite(MUX_S1, HIGH);
  digitalWrite(MUX_S2, HIGH);
  digitalWrite(MUX_S3, LOW);
  }

  if (circuit == 8) {
  digitalWrite(MUX_S0, LOW);
  digitalWrite(MUX_S1, HIGH);
  digitalWrite(MUX_S2, HIGH);
  digitalWrite(MUX_S3, HIGH);
  }

  if (circuit == 9) {
  digitalWrite(MUX_S0, HIGH);
  digitalWrite(MUX_S1, LOW);
  digitalWrite(MUX_S2, LOW);
  digitalWrite(MUX_S3, LOW);
  }

  if (circuit == 10) {
  digitalWrite(MUX_S0, HIGH);
  digitalWrite(MUX_S1, LOW);
  digitalWrite(MUX_S2, LOW);
  digitalWrite(MUX_S3, HIGH);
  }

  if (circuit == 11) {
  digitalWrite(MUX_S0, HIGH);
  digitalWrite(MUX_S1, LOW);
  digitalWrite(MUX_S2, HIGH);
  digitalWrite(MUX_S3, LOW);
  }

  if (circuit == 12) {
  digitalWrite(MUX_S0, HIGH);
  digitalWrite(MUX_S1, LOW);
  digitalWrite(MUX_S2, HIGH);
  digitalWrite(MUX_S3, HIGH);
  }

  if (circuit == 13) {
  digitalWrite(MUX_S0, HIGH);
  digitalWrite(MUX_S1, HIGH);
  digitalWrite(MUX_S2, LOW);
  digitalWrite(MUX_S3, LOW);
  }

  if (circuit == 14) {
  digitalWrite(MUX_S0, HIGH);
  digitalWrite(MUX_S1, HIGH);
  digitalWrite(MUX_S2, LOW);
  digitalWrite(MUX_S3, HIGH);
  }

  if (circuit == 15) {
  digitalWrite(MUX_S0, HIGH);
  digitalWrite(MUX_S1, HIGH);
  digitalWrite(MUX_S2, HIGH);
  digitalWrite(MUX_S3, LOW);
  }

  if (circuit == 16) {
  digitalWrite(MUX_S0, HIGH);
  digitalWrite(MUX_S1, HIGH);
  digitalWrite(MUX_S2, HIGH);
  digitalWrite(MUX_S3, HIGH);
  }
  
}

void getReadings(){
  // TEMPO PAR
  mux(1);
  delay(100);
  temp1 = module.readCelsius(); 
  mux(2);
  delay(100);
  temp2 = module.readCelsius(); 
  mux(3);
  delay(100);
  temp3 = module.readCelsius(); 
  mux(4);
  delay(100);
  temp4 = module.readCelsius(); 
  mux(5);
  delay(100);
  temp5 = module.readCelsius(); 
  mux(6);
  delay(100);
  temp6 = module.readCelsius(); 
  mux(7);
  delay(100);
  temp7 = module.readCelsius(); 
  mux(8);
  delay(100);
  temp8 = module.readCelsius(); 
  mux(9);
  delay(100);
  temp9 =  module.readCelsius(); 
  mux(10);
  delay(100);
  temp10 = module.readCelsius(); 
  mux(11);
  delay(100);
  temp11 = module.readCelsius(); 
  mux(12);
  delay(100);
  temp12 = module.readCelsius(); 
  mux(13);
  delay(100);
  temp13 = module.readCelsius(); 
  mux(14);
  delay(100);
  temp14 = module.readCelsius(); 
  mux(15);
  delay(100);
  temp15 = module.readCelsius(); 
  mux(16);
  delay(100);
  temp16 = module.readCelsius(); 
  
  Serial.print("Temp1: ");
  Serial.println(temp1);
  Serial.print("Temp2: ");
  Serial.println(temp2);
  Serial.print("Temp3: ");
  Serial.println(temp3);
  Serial.print("Temp4: ");
  Serial.println(temp4);
  Serial.print("Temp5: ");
  Serial.println(temp5);
  Serial.print("Temp6: ");
  Serial.println(temp6);
  Serial.print("Temp7: ");
  Serial.println(temp7);
  Serial.print("Temp8: ");
  Serial.println(temp8);
  Serial.print("Temp9: ");
  Serial.println(temp9);
  Serial.print("Temp10: ");
  Serial.println(temp10);
  Serial.print("Temp11: ");
  Serial.println(temp11);
  Serial.print("Temp12: ");
  Serial.println(temp12);
  Serial.print("Temp13: ");
  Serial.println(temp13);
  Serial.print("Temp14: ");
  Serial.println(temp14);
  Serial.print("Temp15: ");
  Serial.println(temp15);
  Serial.print("Temp16: ");
  Serial.println(temp16);
  
}

void setup() {
  Serial.begin(115200);

  // Multiplexer
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);
}

void loop(void) {
  getReadings();
  delay(3000);
}