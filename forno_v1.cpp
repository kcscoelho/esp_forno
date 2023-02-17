/*
Programa para ler termo par K e pressão de fornos industriais
17-02-2023

Conexões:

Display OLED 0.9"
VCC – positivo
GND – negativo
SCL – Pino 22
SDA – Pino 21

Sensor de pressão BMP680
VCC – positivo
GND – negativo
SCL – Pino 22
SDA – Pino 21

Termo par MAX6675
CS  – Pino 5
SCK – Pino 18
S0  – Pino 23
GND – negativo
VCC – positivo


Bibliotecas:

- MAX6675
- Adafruit SSD1306
- Adafruit GFX
- Adafruit bme680

Obs: para instalar essas bibliotecas, abrir o Arduino IDE e seguir o caminho abaixo:
Tools > Manage Libraries
Então digitar o nome da biblioteca e clicar em Install.

Dica: Para acessar a tela do gerenciador de bibliotecas, com a interface do Arduino IDE aberta, utilize o atalho Ctrl + Shift + i


*/
#include <Wire.h>
#include <SPI.h>
#include <max6675.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

int SO = 23;
int CS = 5;
int sck = 18;
MAX6675 module(sck, CS, SO);

Adafruit_BME680 bme; // I2C
//Adafruit_BME680 bme(BME_CS); // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
//Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);


void setup() {   
// SERIAL  
  Serial.begin(115200);

// DISPLAY
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);  
if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { 
  Serial.println(F("SSD1306 allocation failed"));
  for(;;);
}
display.display();
delay(100);
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);

// PRESSAO
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}


void loop() {
  display.setCursor(0,0);
  display.clearDisplay();

// TEMPO PAR
  float temperature = module.readCelsius(); 
  /*
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(F("°C "));   
  */

  display.setTextSize(1);
  display.setCursor(0,10);
  display.print("Temperatura: ");
  display.setTextSize(2);
  display.setCursor(0,30);
  display.print(temperature);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");
  display.display();
  delay(3000);
  display.clearDisplay();

// PRESSAO
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  /*
  Serial.print("Temperature = "); Serial.print(bme.temperature); Serial.println(" *C");
  display.print("Temperature: "); display.print(bme.temperature); display.println(" *C");

  Serial.print("Pressure = "); Serial.print(bme.pressure / 100.0); Serial.println(" hPa");
  display.print("Pressure: "); display.print(bme.pressure / 100); display.println(" hPa");

  Serial.print("Humidity = "); Serial.print(bme.humidity); Serial.println(" %");
  display.print("Humidity: "); display.print(bme.humidity); display.println(" %");

  Serial.print("Gas = "); Serial.print(bme.gas_resistance / 1000.0); Serial.println(" KOhms");
  display.print("Gas: "); display.print(bme.gas_resistance / 1000.0); display.println(" KOhms");
  */
  
  display.print("Pressão: "); display.print(bme.pressure / 100); display.println(" hPa");
  
  display.display();
  delay(3000);
  display.clearDisplay();

}