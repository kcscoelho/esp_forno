#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "FS.h"
#include "SPIFFS.h"
#include <EasyNTPClient.h>
#include <WiFiUdp.h>

#define FORMAT_SPIFFS_IF_FAILED false
#define DATABASE "/database.csv"

#define I2C_SDA 20
#define I2C_SCL 21

Adafruit_BMP280 bmp;
float temperature;
float pressure;

const char* ssid = "redewireless";
const char* password = "senha";
/*
IPAddress local_IP(x, x, x, x);
IPAddress gateway(x, x, x, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);
*/
WiFiUDP ntpUDP;
EasyNTPClient ntpClient(ntpUDP, "pool.ntp.br", (-3*60*60)); // GMT -3:00

AsyncWebServer server(80);
AsyncEventSource events("/events");

void getBMP280Readings(){
  temperature = bmp.readTemperature();
  pressure = bmp.readPressure()/100; // pa in hpa
}

String processor(const String& var){
  getBMP280Readings();
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(temperature);
  }
  else if(var == "PRESSURE"){
    return String(pressure);
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
    .card.temperature { color: #0e7c7b; }
    .card.pressure { color: #3fca6b; }
    .card.download { color: #3498db; }
  </style>
</head>
<body>
  <div class="topnav">
    <h3>Monitor de Fornos</h3>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card temperature">
        <h4><i class="fas fa-thermometer-half"></i> Temperatura</h4><p><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
      </div>
      <div class="card pressure">
        <h4><i class="fas fa-angle-double-down"></i> Pressão</h4><p><span class="reading"><span id="pres">%PRESSURE%</span> hPa</span></p>
      </div>
      <div class="card download">
        <h4>Baixar dados coletados</h4><p><span class="reading"><span id="download"><a href="/download" download><i class="fas fa-download"></i></a></span></p></span> 
      </div>
    </div>
  </div>
</body>
</html>)rawliteral";

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

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
  Wire.begin(I2C_SDA, I2C_SCL);
  bmp.begin(0x76);

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  WiFi.mode(WIFI_AP_STA);

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

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
    request->send_P(200, "text/html", index_html, processor);
  });

  // Route to download database.csv file
  server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, DATABASE, "text/csv");
  });

  // Route to delete database.csv file
  server.on("/delete-database-confirm-confirm-confirm", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, DATABASE, "text/csv");
    deleteFile(SPIFFS, DATABASE);
  });

  AsyncElegantOTA.begin(&server);
  server.begin();
}

void loop(void) {
// Rotina para salvar dados no arquivo
  delay(1000);
  getBMP280Readings();
  String dados = String(ntpClient.getUnixTime());
  dados += String(",");
  dados += String(temperature, 2);
  dados += String(",");
  dados += String(pressure, 2);
  dados += String("\r\n");
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