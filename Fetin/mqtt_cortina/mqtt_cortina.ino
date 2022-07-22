//DECLARACAO DAS BIBLIOTECAS
#include <ESP8266WiFi.h>   // biblioteca do Node MCU
#include <PubSubClient.h>  // biblioteca comunicação mqtt
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//informações da rede WIFI
const char* ssid = "CSI-Lab"; //SSID da rede WIFI
const char* password =  "In@teLCS&I"; //senha da rede wifi

//informações do broker MQTT
const char* mqttServer = "192.168.40.22";   //servidor
const char* mqttUser = "csilab";              //usuário
const char* mqttPassword = "WhoAmI#2020";      //senha
const int mqttPort = 1883;                     //porta
const char* mqttTopicSub = "smarthouse/#";           //tópico que sera assinado
const char *ID = "Teste_Cortina";  // Name of our device, must be unique


#define IN2    16 //D0
#define IN1    5  //D1

int cortina = 0; //aberta


WiFiClient espClient; // Cria o objeto espClient
PubSubClient client(espClient); //instancia o Cliente MQTT passando o objeto espClient

void setup()
{
  pinMode(IN1, OUTPUT);                  // definições das portas IN1 e IN2 como portas de saidas
  pinMode(IN2, OUTPUT);
  digitalWrite(IN1, 0);               // desligando-os
  digitalWrite(IN2, 0);

  Serial.begin(115200); //Velocidade da Serial

  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(ssid);
  Serial.println("Aguarde");

  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(ssid, password); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(ssid);
  ArduinoOTA.setPassword((const char *)"CSI#SmartHouse");
  ArduinoOTA.setHostname("smarthome_cortina");

  ArduinoOTA.onStart([]() {
    Serial.println("Inicio...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("nFim!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progresso: %u%%r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro [%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Autenticacao Falhou");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Falha no Inicio");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Falha na Conexao");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Falha na Recepcao");
    else if (error == OTA_END_ERROR) Serial.println("Falha no Fim");
  });
  ArduinoOTA.begin();
  Serial.println("Pronto");
  Serial.print("Endereco IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  connect();

}

void subir(int tempo) {
  digitalWrite(IN2, 1);               // acionamento IN2
  digitalWrite(IN1, 0);                // desativa IN1
  Serial.println("Relé 2 acionado!");
  delay(tempo);
}

void descer(int tempo) {
  digitalWrite(IN2, 0);               // desativa IN2
  digitalWrite(IN1, 1);                // acionamento IN1
  Serial.println("Relé 1 acionado!");
  delay(tempo);
}

//FUNCAO DE RECEBIMENTO E PROCESSAMENTO DE MENSAGENS VINDAS DO BROKER
void callback(char* topic, byte* payload, unsigned int length)
{

  //armazena mensagem recebida em uma variavel inteira
  payload[length] = '\0';
  int MSG = atoi((char*)payload);
  String topico = topic;
  //MOSTRANDO RECEBIMENTO DA MENSAGEM NA SERIAL

  Serial.print("Mensagem no tópico: ");
  Serial.println(topico);
  Serial.print("Mensagem:");
  Serial.print(MSG);
  Serial.println();
  Serial.println("-----------------------");


  //PROCESSAMENTO DAS MENSAGENS
  //MOTOR
  if (topico == "smarthouse/cortina" && cortina != MSG) {
    switch (MSG) {
      case 3: //anti-horário
        if (cortina == 4)
          subir(3750);
        else
          subir(7500);
        break;

      case 2: //horário
        if (cortina == 4)
          descer(3450);
        else
          descer(6900);
        break;

      case 4:
        if (cortina == 3)   //descer
          descer(3450);
        else if (cortina == 2)
          subir (3750);
        break;
    }

    cortina = MSG;
    digitalWrite(IN1, 0);               // desativa IN1
    digitalWrite(IN2, 0);                // desativa IN2
    delay (200);
    Serial.println("Relés desligados!");
  }



  Serial.println();
  Serial.println("**********************************************************************");
  Serial.println();
}

void connect () //FUNCAO DE RECONEXAO COM O BROKER
{
  while (!client.connected()) //enquanto a conexão com o broker não for realizada
  {
    Serial.println("Conectando ao Broker MQTT...");

    if (client.connect(ID, mqttUser, mqttPassword )) {
      Serial.println("Conectado");
    }
    else
    {
      Serial.print("falha estado  ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  //subscreve no tópico
  client.subscribe(mqttTopicSub);
}



void loop()
{
  // Mantenha esse trecho no inicio do laço "loop" - verifica requisicoes OTA
  ArduinoOTA.handle();
  delay(10);
  client.loop();
  delay(10);

  if (!client.connected())
  {
    connect();

  }
}
