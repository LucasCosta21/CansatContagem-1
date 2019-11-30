/* 
 *  CONTAGEM-1: Nanossatélite para o ensino de ciência
 *  Versão Final
 *  Autores: Lucas Assunção Costa, Jhonatan Antônio Goulart, Júlia Antunes Miranda
 *  Código usado no projeto de conclusão de curso dos componentes acima no CEFET-MG Contagem 
 */

#include <VirtualWire.h>
#include "dht.h" //biblioteca dht11
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h> 

dht DHT;//objeto dht (sensor humidade e temperatura)
Adafruit_BMP280 sensor_bmp; // I2C
TinyGPSPlus gps; //objeto gps
SoftwareSerial gpsSerial(4, 5); //portas seriais do gps

void setup()
{
  Serial.begin(9600);

  if (!sensor_bmp.begin(0x76)) {
    Serial.println(F("não foi possível iniciar o BMP280!"));
  }               

  //Define o pino 8 do Arduino como o pino de dados do transmissor
  vw_set_tx_pin(8);
  vw_setup(2000);   // Bits per sec
  gpsSerial.begin(9600);
  delay(2000); //INTERVALO DE 2 SEGUNDO ANTES DE INICIAR
}
 
void loop()
{
  bool recebido = false; // Booleano auxiliador
  char data[60]; // String de dados que serão enviadas via Rádio Frequência
  DHT.read11(A2); //LÊ AS INFORMAÇÕES DO SENSOR DHT11

  //concatena temperatura
  concatena(DHT.temperature,data,2,0);

  //concatena umidade
  concatena(DHT.humidity,data,2,0);

  if (sensor_bmp.begin(0x76)) {
    //concatena pressao
    concatena(sensor_bmp.readPressure(),data,2,1);
  }else{
    strcat(data, ";");
  }

  if(!recebido){
    if(sensor_bmp.begin(0x76)){ 
            concatena(sensor_bmp.readAltitude(),data,3,1);
          }else{strcat(data, ";");}
          
          strcat(data, ";");
          strcat(data, ";");
  }

  send(data);
  strcpy(data, "");
} 

void concatena(float n, char dados[60],int casasInt, int casasDec){
  char aux[30];

  dtostrf(n,casasInt,casasDec,aux);  
  strcat(dados, aux);
  strcat(dados, ";");
}

void concatInfo(bool recebido, char data[60]){
  if (gps.location.isValid())
      {
        recebido = true;
        
          //concatena altitude
          concatena(gps.altitude.meters(),data,3,1);
          
          //concatena latitude
          concatena(gps.location.lat(),data,3,6);
          
          //concatena longitude
          concatena(gps.location.lng(),data,3,6);

          Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
      }
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
  }
  else
  {
    Serial.println("Location: Not Available");
  }
  
  Serial.print("Date: ");
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.print("Time: ");
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.println(gps.time.centisecond());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.println();
  Serial.println();
  delay(1000);
}
 
void send (char *message)
{
  vw_send((uint8_t *)message, strlen(message));
  vw_wait_tx(); // Aguarda o envio de dados
}