#include <PubSubClient.h>
#include <WiFi.h> 


#define pinSensorA A0 //Definição do pino analogico do sensor de chuva
#define pinSensorD 14 //Definição do pino digital do sensor de chuva 

//WiFi
const char* SSID = "Redmi8";         // SSID / nome da rede WiFi que deseja se conectar
const char* PASSWORD = "123f456f";    // Senha da rede WiFi que deseja se conectar
WiFiClient wifiClient;                        
 
//MQTT Server
const char* BROKER_MQTT = "mqtt.eclipseprojects.io"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                      // Porta do Broker MQTT

#define ID_MQTT  "RainIOT01"            //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
#define TOPIC_PUBLISH "RainIOT_UNISC"   //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
PubSubClient MQTT(wifiClient);          // Instancia o Cliente MQTT passando o objeto espClient// ??esta passando wifiClient??

//Declaração das Funções
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void sensorChuva();      //Recebe os dados do sensor de chuva e se comunica via MQTT

void setup() 
{     
  pinMode(pinSensorD, INPUT);      //Definimos o modo do pino digital como INPUT
  Serial.begin(9600);               //Habilitando monitor serial para ver o que esta acontecendo

  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   
}

//NUCLEO DO PROGRAMA, estará rodando constantemente
void loop() 
{
  mantemConexoes();
  delay(100);  //Atraso para evitar alto consumo de recursos do esp32, definir um tempo suficiente
  MQTT.loop();
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
    MQTT.publish(TOPIC_PUBLISH, "0");  //Publica 0 para o servidor 
    Serial.println("Está Chovendo. Payload enviado.");
  }

  Serial.print("  Analogico:");
  Serial.print(analogRead(pinSensorA)); //Printa o valor analogico que está sendo lido  
}


void mantemConexoes() 
{
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
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

void conectaMQTT() 
{ 
    while (!MQTT.connected()) 
    {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
        } 
        else 
        {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}
