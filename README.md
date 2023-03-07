# ESP Forno

Programa para ler 16 termo pares tipo K e 2 sensores de pressão em fornos industriais.<br>
17-02-2023 - 06-03-2023

#### Descrição:
O programa efetua a leitura dos sensores, exibe os valores em página web e os grava em arquivo csv.<br>
O arquivo csv pode ser baixado pela página web. Também pode ser zerado para começar um arquivo novo.<br>
Atualização de firmware via OTA (ElegantOTA).

##### Comandos web
- /download: baixa o arquivo database.csv com os dados coletados pelos sensores.
- /update: permite atualizar o firmware via navegador.
- /delete-database-confirm-confirm-confirm: exclui o arquivo de dados atual e inicia um novo arquivo de coletas. Atenção, baixe o arquivo antes de excluí-lo.

#### Componentes utilizados:
- ESP WROOM32
- Termo par tipo K modelo MAX6675
- Sensor de pressão modelo MPS20N0040D HX710B
- Multiplexador 74HC4067 / HP4067 CMOS 16 Canais

#### Pinou ESP WROOM32:
https://lastminuteengineers.com/esp32-pinout-reference/

#### Pinos de IO ESP WROOM32:
16,17,18,19,20,21,22,23,24,28,29,30,31,32,33,34i,35i

#### Esquema de ligação:
<img src="https://github.com/kcscoelho/esp_forno/blob/main/Esquema.PNG">

#### Conexões:
##### Sensor de temperatura termo par tipo K, modelo MAX6675
- CS : Pino 21 através das entradas do Multiplexer
- SCK: Pino 23
- S0 : Pino 22
- GND: negativo
- VCC: positivo

Obs: para multiplexar os sensores de temperatura, utilizar a seguinte conexão:
- Todos os SCKs no pino 23.
- Todos os S0 no pino 22.
- CS de cada sensor numa porta do MUX.

##### MUX, Multiplexador 74HC4067 / HP4067 CMOS 16 Canais
- GND: negativo
- VCC: positivo
- S0: Pino 19
- S1: Pino 18
- S2: Pino 17
- S3: Pino 16
- SIG: Pino 21 - chaveia esse pino com a saída do multiplexer
- EN: negativo

##### Sensor de pressão modelo MPS20N0040D / HX710B

Sensor 1 - Entrada
- SCK: Pino 32
- OUT: Pino 34
- GND: negativo
- VCC: positivo

Sensor 2 - Saída
- SCK: Pino 33
- OUT: Pino 35
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

Obs: para instalar essas bibliotecas, abrir o Arduino IDE e seguir o caminho abaixo:<br>
Tools > Manage Libraries<br>
Então digitar o nome da biblioteca e clicar em Install.

#### Importante: 
Se não encontrar a biblioteca pela interface do arduino IDE, baixar diretamente do site do desenvolvedor e descompactar no diretório "libraries" do Arduino.

#### Referências:
Pressure Measurements with the MPS20N0040D Breakout Board with the HX710B/HX711 ADC 5V Supply Voltage
Ref.: https://makersportal.com/blog/2020/6/4/mps20n0040d-pressure-sensor-calibration-with-arduino

#### Testes e versionamento
- versão 1: inicial, um sensor de temperatura e LCD.
- versão 2: removido LCD, direcionado informações para serial.
- versão 3: inserido hora na página web e formato humano no arquivo de dados ao invés de unixtime.
- versão 4: adequado para sensores MAX6675 e HX710B.
