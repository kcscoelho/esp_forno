# ESP Forno

Programa para ler 16 termo pares tipo K e 2 sensores de pressão em fornos industriais
17-02-2023 - 28-02-2023

#### Descrição:
O programa efetua a leitura dos sensores e exibe em página web e grava em arquivo csv os valores.
O arquivo csv pode ser baixado pela página web. Também pode ser zerado para começar um arquivo novo.
Atualização de firmware via OTA (ElegantOTA).

#### Componentes utilizados:
- ESP WROOM32 (pinout: https://www.electronicshub.org/esp32-pinout/)
- Termo par tipo K modelo MAX6675
- Sensor de pressão modelo MPS20N0040D HX710B
- Multiplexador 74HC4067 / HP4067 CMOS 16 Canais

#### Conexões:
##### Sensor de temperatura termo par tipo K, modelo MAX6675
- CS : Pino 5
- SCK: Pino 18
- S0 : Pino 23
- GND: negativo
- VCC: positivo

Obs: para multiplexar os sensores de temperatura, utilizar a seguinte conexão:
- Todos os SCKs no pino 18.
- Todos os S0 no pino 23.
- CS de cada sensor numa porta do MUX.

##### MUX, Multiplexador 74HC4067 / HP4067 CMOS 16 Canais
- GND: negativo
- VCC: positivo
- S0 : Pino 2
- S1 : Pino 3
- S2 : Pino 4
- S3 : Pino 8
- EN : Pino 5

##### Sensor de pressão modelo MPS20N0040D / HX710B

Sensor 1 - Entrada
- SCK: Pino 10
- OUT: Pino 11
- GND: negativo
- VCC: positivo

Sensor 2 - Saída
- SCK: Pino 12
- OUT: Pino 13
- GND: negativo
- VCC: positivo

#### Bibliotecas:
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

#### Importante: 
Se não encontrar a biblioteca pela interface do arduino IDE,
baixar diretamente do site do desenvolvedor e descompactar no diretório "libraries" do Arduino.

#### Referências:
Pressure Measurements with the MPS20N0040D Breakout Board with the HX710B/HX711 ADC 5V Supply Voltage
Ref.: https://makersportal.com/blog/2020/6/4/mps20n0040d-pressure-sensor-calibration-with-arduino

#### Testes e versionamento
- versão 1: inicial, um sensor de temperatura e LCD.
- versão 2: removido LCD, direcionado informações para serial.
- versão 3: inserido hora na página web e formato humano no arquivo de dados ao invés de unixtime.
- versão 4: adequado para sensores MAX6675 e HX710B.
