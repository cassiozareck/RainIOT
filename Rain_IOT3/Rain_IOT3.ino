// Bibliotecas-----------------------------------------------------------------------------------
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// Definições dos pinos -------------------------------------------------------------------------
#define pinSensorA A0 //Definição do pino analogico do sensor de chuva
#define pinSensorD 14 //Definição do pino digital do sensor de chuva 

// Variaveis Globais ----------------------------------------------------------------------------
int clima = 3; //Inicializada como 3, a variavel clima recebe valor 1 quando esta chovendo e 0 quando não esta chovendo

// Wi-Fi ----------------------------------------------------------------------------------------
const char*   SSID        = "rede";
const char*   PASSWORD    = "Senha";

// Broker SMTP-----------------------------------------------------------------------------------
const char* mqtt_server = "endereço do servidor mqtt";
const int mqtt_port     =  8883; //Porta do servidor
#define TOPIC_PUBLISH     "topico 1"  //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
#define TOPIC_PUBLISH2    "topico 2"
// Credenciais SMTP------------------------------------------------------------------------------
const char* mqtt_username = "usuario";
const char* mqtt_password = "senha";

// Certificado CA TLS ---------------------------------------------------------------------------
static const char* root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

WiFiClientSecure espClient;
PubSubClient client(espClient);

// Declaração de Funções--------------------------------------------------------------------------
void mantemConexoes();  //Mantem Conexões
void conectaWiFi();     //Faz conexão com WiFi
void logo();            //Logo Rain IOT
void sensorChuva();     //Sensor de Chuva



// Setup------------------------------------------------------------------------------------------
void setup() 
{
  Serial.begin(9600);
  conectaWiFi();
  delay(100);
  logo();
  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
}

// NUCLEO DO PROGRAMA, estará rodando constantemente----------------------------------------------
void loop() 
{
  sensorChuva();
  mantemConexoes();
}

// Funções abaixo:--------------------------------------------------------------------------------

void mantemConexoes() 
{
  conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita

  if (!client.connected()) 
  {
    conetaBroker();
  }
  client.loop();
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
      delay(500);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso! Rede atual: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void conetaBroker() //faz conexão com o Broker, e reconecta caso caia a conexão
{
  while (!client.connected()) 
  {
    Serial.print("Conectando-se ao broker MQTT...");
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) 
    {
      Serial.println("Conectado!");
      client.publish(TOPIC_PUBLISH2, "1");
    } 
    else 
    {
      Serial.print("Falha na conexão, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void sensorChuva()
{
  if ((analogRead(pinSensorA) < 3600 ) && (clima != 1 ))
  {
    clima = 1;
    if (client.publish(TOPIC_PUBLISH, "1", true))
    {
      Serial.println("Está Chovendo. Payload enviado.");
    }
    delay(3000);
  }
  else if ((analogRead(pinSensorA) > 3950 ) && (clima != 0 ))
  {
    clima = 0;
    if (client.publish(TOPIC_PUBLISH, "0", true))
    {
      Serial.println("Não está chovendo. Payload enviado.");
    }
    delay(3000);
  }
}

void logo()
{
  Serial.println("                                                     ");
  Serial.println("//  _____          _          _____   ____   _______ ");
  Serial.println("// |  __ \\        (_)        |_   _| / __ \\ |__   __|");
  Serial.println("// | |__) |  __ _  _  _ __     | |  | |  | |   | |   ");
  Serial.println("// |  _  /  / _` || || '_ \\    | |  | |  | |   | |   ");
  Serial.println("// | | \\ \\ | (_| || || | | |  _| |_ | |__| |   | |   ");
  Serial.println("// |_|  \\_\\ \\__,_||_||_| |_| |_____| \\____/    |_|   ");
  Serial.println("                                                         ");
}
