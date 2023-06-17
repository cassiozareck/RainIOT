#include <ESP_Mail_Client.h>
#include <ESP_Mail_Client_Version.h>
#include <ESP_Mail_Const.h>
#include <ESP_Mail_Error.h>
#include <ESP_Mail_FS.h>
#include <ESP_Mail_IMAP.h>
#include <ESP_Mail_Print.h>
#include <ESP_Mail_SMTP.h>
#include <HeapStat.h>



#include <WiFi.h> 

#define SMTP_HOST "smtp.aol.com" //endereço do servidor SMTP
#define SMTP_PORT esp_mail_smtp_port_587 //porta

//Credenciais de Login
#define AUTHOR_EMAIL "email"
#define AUTHOR_PASSWORD "senha"



#define pinSensorA A0 //Definição do pino analogico do sensor de chuva
#define pinSensorD 14 //Definição do pino digital do sensor de chuva 

//WiFi
const char* SSID = "Redmi8";         // SSID / nome da rede WiFi que deseja se conectar
const char* PASSWORD = "123f456f";    // Senha da rede WiFi que deseja se conectar
WiFiClient wifiClient;                        
 

//Declaração das Funções
void mantemConexoes();  //Garante conexao WiFi
void conectaWiFi();     //Faz conexão com WiFi
void sensorChuva();      //Recebe os dados do sensor de chuva e se comunica via MQTT

void setup() 
{     
  pinMode(pinSensorD, INPUT);      //Definimos o modo do pino digital como INPUT
  Serial.begin(9600);               //Habilitando monitor serial para ver o que esta acontecendo

  conectaWiFi();
}

//NUCLEO DO PROGRAMA, estará rodando constantemente
void loop() 
{
  mantemConexoes();
  delay(100);  //Atraso para evitar alto consumo de recursos do esp32, definir um tempo suficiente
  sensorChuva();
}

void sensorChuva()
{
  Serial.print("Digital: ");

  if (digitalRead(pinSensorD)) //Se o que for lido no pino digital for verdadeiro (1)
  {
    Serial.println("Sem chuva ");
  }
  else                        //Se não, se o que for lido no pino digital for falso (0)
  {
    Serial.println("Está Chovendo.");
  }

  Serial.print("  Analogico:");
  Serial.print(analogRead(pinSensorA)); //Printa o valor analogico que está sendo lido  
}


void mantemConexoes() 
{
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() 
{

  if (WiFi.status() == WL_CONNECTED) 
  {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}
