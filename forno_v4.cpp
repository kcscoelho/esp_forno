/*
Programa para ler termo par K e pressão de fornos industriais
17-02-2023

Descrição:
O programa efetua a leitura dos sensores e exibe em página web e grava em arquivo csv os valores.
O arquivo csv pode ser baixado pela página web. Também pode ser zerado para começar um arquivo novo.
Atualização de firmware via OTA (ElegantOTA).

Componentes utilizados:
- ESP WROOM32 (pinout: https://www.electronicshub.org/esp32-pinout/)
- Termo par tipo K modelo MAX6675
- Sensor de pressão modelo MPS20N0040D HX710B
- Multiplexador 74HC4067 / HP4067 CMOS 16 Canais

Conexões:
Sensor de temperatura termo par tipo K, modelo MAX6675
CS  – Pino 5
SCK – Pino 18
S0  – Pino 23
GND – negativo
VCC – positivo

Obs: para multiplexar os sensores de temperatura, utilizar a seguinte conexão:
- Todos os SCKs no pino 18.
- Todos os S0 no pino 23.
- CS de cada sensor numa porta do MUX.

MUX, Multiplexador 74HC4067 / HP4067 CMOS 16 Canais
GND – negativo
VCC – positivo
S0 - Pino 2
S1 - Pino 3
S2 - Pino 4
S3 - Pino 8
EN - Pino 5

Sensor de pressão modelo MPS20N0040D / HX710B
Sensor 1 - Entrada
SCK – Pino 10
OUT – Pino 11
GND – negativo
VCC – positivo

Sensor 2 - Saída
SCK – Pino 12
OUT – Pino 13
GND – negativo
VCC – positivo

Bibliotecas:
- MAX6675
https://blogmasterwalkershop.com.br/arquivos/libs/MAX6675-library-master.zip

- Q2HX711
https://github.com/queuetue/Q2-HX711-Arduino-Library

- AsyncTCP
https://github.com/me-no-dev/AsyncTCP

- EasyNTPClient
https://github.com/aharshac/EasyNTPClient

- AsyncElegantOTA
https://github.com/ayushsharma82/AsyncElegantOTA

- ESPAsyncWebServer
https://github.com/me-no-dev/ESPAsyncWebServer


Obs: para instalar essas bibliotecas, abrir o Arduino IDE e seguir o caminho abaixo:
Tools > Manage Libraries
Então digitar o nome da biblioteca e clicar em Install.

Importante: 
Se não encontrar a biblioteca pela interface do arduino IDE,
baixar diretamente do site do desenvolvedor e descompactar no diretório "libraries" do Arduino.

Referências:
Pressure Measurements with the MPS20N0040D Breakout Board with the HX710B/HX711 ADC 5V Supply Voltage
Ref.: https://makersportal.com/blog/2020/6/4/mps20n0040d-pressure-sensor-calibration-with-arduino

Testes e versionamento
versão 1: inicial.
versão 2: removido LCD, direcionado informações para serial.
versão 3: inserido hora na página web e formato humano no arquivo de dados ao invés de unixtime.
versão 4: adequado para sensores MAX6675 e HX710B.

*/
#include "Wire.h"
#include "SPI.h"
#include "max6675.h"
#include "WiFi.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "AsyncElegantOTA.h"
#include "FS.h"
#include "SPIFFS.h"
#include "EasyNTPClient.h"
#include "WiFiUdp.h"
#include "time.h"
#include "Q2HX711.h"

#define FORMAT_SPIFFS_IF_FAILED false
#define DATABASE "/database.csv"

#define MUX_S0 2
#define MUX_S1 3
#define MUX_S2 4
#define MUX_S3 8

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
float pressure1;
float pressure2;
String data;

// Tempo par
int SO = 23;
int CS = 5;
int sck = 18;
MAX6675 module(sck, CS, SO);

// Pressão
const byte MPS_1_SCK_pin = 10; // clock data pin
const byte MPS_1_OUT_pin = 11; // OUT data pin
const byte MPS_2_SCK_pin = 12; // clock data pin
const byte MPS_2_OUT_pin = 13; // OUT data pin
int avg_size = 10; // # pts to average over

Q2HX711 MPS20N0040D_1(MPS_1_OUT_pin, MPS_1_SCK_pin); // start comm with the HX710B Sensor 1
Q2HX711 MPS20N0040D_2(MPS_2_OUT_pin, MPS_2_SCK_pin); // start comm with the HX710B Sensor 2

const char* ssid = "x";
const char* password = "x";
/*
IPAddress local_IP(x, x, x, 19);
IPAddress gateway(x, x, x, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);
*/
WiFiUDP ntpUDP;
EasyNTPClient ntpClient(ntpUDP, "pool.ntp.br", (-3*60*60)); // GMT -3:00

AsyncWebServer server(80);
AsyncEventSource events("/events");

String unixTimeToDateTime(unsigned long seconds){
    // Save the time in Human
    // readable format
    String ans = "";

    // Number of days in month
    // in normal year
    int daysOfMonth[] = { 31, 28, 31, 30, 31, 30,
                          31, 31, 30, 31, 30, 31 };

    long int currYear, daysTillNow, extraTime, extraDays,
        index, date, month, hours, minutes, secondss,
        flag = 0;

    // Calculate total days unix time T
    daysTillNow = seconds / (24 * 60 * 60);
    extraTime = seconds % (24 * 60 * 60);
    currYear = 1970;

    // Calculating current year
    while (true) {
        if (currYear % 400 == 0
            || (currYear % 4 == 0 && currYear % 100 != 0)) {
            if (daysTillNow < 366) {
                break;
            }
            daysTillNow -= 366;
        }
        else {
            if (daysTillNow < 365) {
                break;
            }
            daysTillNow -= 365;
        }
        currYear += 1;
    }
    // Updating extradays because it
    // will give days till previous day
    // and we have include current day
    extraDays = daysTillNow + 1;

    if (currYear % 400 == 0
        || (currYear % 4 == 0 && currYear % 100 != 0))
        flag = 1;

    // Calculating MONTH and DATE
    month = 0, index = 0;
    if (flag == 1) {
        while (true) {

            if (index == 1) {
                if (extraDays - 29 < 0)
                    break;
                month += 1;
                extraDays -= 29;
            }
            else {
                if (extraDays - daysOfMonth[index] < 0) {
                    break;
                }
                month += 1;
                extraDays -= daysOfMonth[index];
            }
            index += 1;
        }
    }
    else {
        while (true) {

            if (extraDays - daysOfMonth[index] < 0) {
                break;
            }
            month += 1;
            extraDays -= daysOfMonth[index];
            index += 1;
        }
    }

    // Current Month
    if (extraDays > 0) {
        month += 1;
        date = extraDays;
    }
    else {
        if (month == 2 && flag == 1)
            date = 29;
        else {
            date = daysOfMonth[month - 1];
        }
    }

    // Calculating HH:MM:YYYY
    hours = extraTime / 3600;
    minutes = (extraTime % 3600) / 60;
    secondss = (extraTime % 3600) % 60;

    //Ajuste de mês, dia, hora, minutos e segundos inferiores a 10, adicionando um 0. Ex.: 05 ao invés de 5. (5/5/2023 5:5:5 > 05/05/2023 05:05:05) by KCSC
    if (date < 10) {
      ans += "0";
      ans += String(date);
    } else {
      ans += String(date);
    }
    ans += "/";    
    if (month < 10) {
      ans += "0";
      ans += String(month);
    } else {
      ans += String(month);
    }
    ans += "/";
    ans += String(currYear);
    ans += " ";
    if (hours < 10) {
      ans += "0";
      ans += String(hours);
    } else {
      ans += String(hours);
    }
    ans += ":";
    if (minutes < 10) {
      ans += "0";
      ans += String(minutes);
    } else {
      ans += String(minutes);  
    }
    ans += ":";
    if (secondss < 10) {
      ans += "0";
      ans += String(secondss);
    } else {
      ans += String(secondss);  
    }
  return ans;
}

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
  temp1 = module.readCelsius(); 
  mux(2);
  temp2 = module.readCelsius(); 
  mux(3);
  temp3 = module.readCelsius(); 
  mux(4);
  temp4 = module.readCelsius(); 
  mux(5);
  temp5 = module.readCelsius(); 
  mux(6);
  temp6 = module.readCelsius(); 
  mux(7);
  temp7 = module.readCelsius(); 
  mux(8);
  temp8 = module.readCelsius(); 
  mux(9);
  temp9 =  module.readCelsius(); 
  mux(10);
  temp10 = module.readCelsius(); 
  mux(11);
  temp11 = module.readCelsius(); 
  mux(12);
  temp12 = module.readCelsius(); 
  mux(13);
  temp13 = module.readCelsius(); 
  mux(14);
  temp14 = module.readCelsius(); 
  mux(15);
  temp15 = module.readCelsius(); 
  mux(16);
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

String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMP1"){
    return String(temp1);
  }
  else if(var == "TEMP2"){
    return String(temp2);
  }
  else if(var == "TEMP3"){
    return String(temp3);
  }
  else if(var == "TEMP4"){
    return String(temp4);
  }
  else if(var == "TEMP5"){
    return String(temp5);
  }
  else if(var == "TEMP6"){
    return String(temp6);
  }
  else if(var == "TEMP7"){
    return String(temp7);
  }
  else if(var == "TEMP8"){
    return String(temp8);
  }
  else if(var == "TEMP9"){
    return String(temp9);
  }
  else if(var == "TEMP10"){
    return String(temp10);
  }
  else if(var == "TEMP11"){
    return String(temp11);
  }
  else if(var == "TEMP12"){
    return String(temp12);
  }
  else if(var == "TEMP13"){
    return String(temp13);
  }
  else if(var == "TEMP14"){
    return String(temp14);
  }
  else if(var == "TEMP15"){
    return String(temp15);
  }
  else if(var == "TEMP16"){
    return String(temp16);
  }
  else if(var == "PRESSURE1"){
    return String(pressure1);
  }
  else if(var == "PRESSURE2"){
    return String(pressure2);
  }
  else if(var == "DATA"){
    return String(data);
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html lang="pt">
<head>
  <title>Monitor de Fornos</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="10">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v6.3.0/css/all.css">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p {  font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #4B1D3F; color: white; font-size: 1.7rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); }
    .reading { font-size: 2.8rem; }
    .card.t1 { color: #0e7c7b; }
    .card.p1 { color: #3fca6b; }
    .card.d1 { color: #3498db; }
  </style>
</head>
<body>
  <div class="topnav">
    <h3>Monitor de Fornos</h3>
    %DATA%
  </div>
  <div class="content">
    <div class="cards">
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 1</h4><p><span class="reading"><span id="temp">%TEMP1%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 2</h4><p><span class="reading"><span id="temp">%TEMP2%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 3</h4><p><span class="reading"><span id="temp">%TEMP3%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 4</h4><p><span class="reading"><span id="temp">%TEMP4%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 5</h4><p><span class="reading"><span id="temp">%TEMP5%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 6</h4><p><span class="reading"><span id="temp">%TEMP6%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 7</h4><p><span class="reading"><span id="temp">%TEMP7%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 8</h4><p><span class="reading"><span id="temp">%TEMP8%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 9</h4><p><span class="reading"><span id="temp">%TEMP9%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 10</h4><p><span class="reading"><span id="temp">%TEMP10%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 11</h4><p><span class="reading"><span id="temp">%TEMP11%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 12</h4><p><span class="reading"><span id="temp">%TEMP12%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 13</h4><p><span class="reading"><span id="temp">%TEMP13%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 14</h4><p><span class="reading"><span id="temp">%TEMP14%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 15</h4><p><span class="reading"><span id="temp">%TEMP15%</span> &deg;C</span></p>
      </div>
      <div class="card t1">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura 16</h4><p><span class="reading"><span id="temp">%TEMP16%</span> &deg;C</span></p>
      </div>
      <div class="card p1">
        <h4><i class="fas fa-angle-double-down"></i> Pressão Entrada</h4><p><span class="reading"><span id="pres">%PRESSURE1%</span> hPa</span></p>
      </div>
      <div class="card p1">
        <h4><i class="fas fa-angle-double-down"></i> Pressão Saída</h4><p><span class="reading"><span id="pres">%PRESSURE2%</span> hPa</span></p>
      </div>
      <div class="card d1">
        <h4>Baixar dados coletados</h4><p><span class="reading"><span id="download"><a href="/download" download><i class="fas fa-download"></i></a></span></p></span> 
      </div>
    </div>
  </div>
</body>
</html>)rawliteral";

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void setup() {
  Serial.begin(115200);

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  WiFi.mode(WIFI_AP_STA);

//  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
//    Serial.println("STA Failed to configure");
//  }

  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    data = unixTimeToDateTime(ntpClient.getUnixTime());    
    request->send_P(200, "text/html", index_html, processor);
  });

  // Route to download database.csv file
  server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, DATABASE, "text/csv");
  });

  // Route to delete database.csv file
  server.on("/delete-database-confirm-confirm-confirm", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, DATABASE, "text/csv");
    delay(30000);
    deleteFile(SPIFFS, DATABASE);
  });

  // Multiplexer
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);

  AsyncElegantOTA.begin(&server);
  server.begin();
}

void loop(void) {

// Rotina para salvar dados no arquivo
  delay(1000);
  getReadings();
  String dados = unixTimeToDateTime(ntpClient.getUnixTime());
  dados += ",";
  dados += String(temp1, 2);
  dados += ",";
  dados += String(temp2, 2);
  dados += ",";
  dados += String(temp3, 2);
  dados += ",";
  dados += String(temp4, 2);
  dados += ",";
  dados += String(temp5, 2);
  dados += ",";
  dados += String(temp6, 2);
  dados += ",";
  dados += String(temp7, 2);
  dados += ",";
  dados += String(temp8, 2);
  dados += ",";
  dados += String(temp9, 2);
  dados += ",";
  dados += String(temp10, 2);
  dados += ",";
  dados += String(temp11, 2);
  dados += ",";
  dados += String(temp12, 2);
  dados += ",";
  dados += String(temp13, 2);
  dados += ",";
  dados += String(temp14, 2);
  dados += ",";
  dados += String(temp15, 2);
  dados += ",";
  dados += String(temp16, 2);
  dados += ",";
  dados += String(pressure1, 2);
  dados += ",";
  dados += String(pressure2, 2);
  dados += "\r\n";
  Serial.println("Dados: "+dados);
  //
  File file = SPIFFS.open(DATABASE, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
    }
    if(file.print(dados)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
  file.close();
  delay(300000);
// Fim RPSDNA  
}