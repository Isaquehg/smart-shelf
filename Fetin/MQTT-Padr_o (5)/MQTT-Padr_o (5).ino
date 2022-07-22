//DECLARACAO DAS BIBLIOTECAS
#include <ESP8266WiFi.h>   // biblioteca do Node MCU
#include <PubSubClient.h>  // biblioteca comunicação mqtt

//informações da rede WIFI
const char* ssid = "CSI-Lab"; //SSID da rede WIFI
const char* password = "In@teLCS&I"; //senha da rede wifi

//informações do broker MQTT
const char* mqttServer = "192.168.66.32";   //servidor
const int mqttPort = 1883;                     //porta
const char* mqttTopicSub = "broker";           //tópico que sera assinado
const char* mqttUser = "csilab";              //usuário
const char* mqttPassword = "WhoAmI#2020";      //senha
const char *ID = "Treinamento";  // Nome do dispositivo - MUDE PARA NÃO HAVER COLISÃO

WiFiClient espClient; // Cria o objeto espClient
PubSubClient client(espClient); //instancia o Cliente MQTT passando o objeto espClient

//PROTOTIPOS DE FUNCOES DAS CONEXOES INTERNET - MQTT
void conectar();
void conectarmqtt();

void setup()
{
  Serial.begin(115200); //Velocidade da Serial


  //CHAMADAS DAS FUNCOES DAS CONEXOES INTERNET - MQTT
  conectar();
  conectarmqtt();
}

//FUNCAO DE RECEBIMENTO E PROCESSAMENTO DE MENSAGENS VINDAS DO BROKER
void callback(char* topic, byte* payload, unsigned int length)
{

  //armazena mensagem recebida em uma variavel inteira - pode mudar a conversão para qualquer outro tipo de variável, se necessário
  payload[length] = '\0';
  int MSG = atoi((char*)payload);

  //MOSTRANDO RECEBIMENTO DA MENSAGEM NA SERIAL

  Serial.print("Mensagem no tópico: ");
  Serial.println(mqttTopicSub);
  Serial.print("Mensagem:");
  Serial.print(MSG);
  Serial.println();
  Serial.println("-----------------------");



}

void conectar()//FUNCAO DA CONEXAO COM A INTERNET
{
  delay(10);
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
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
}

void conectarmqtt () //FUNCAO DE CONEXAO COM O BROKER
{
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
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

  client.loop();



  delay(2000);
  Serial.println("publicou!");
  client.publish("node", "oi");//publicacao no broker MQTT


}
