#include <WiFi.h>               // biblioteca do Node MCU
#include <PubSubClient.h>       // biblioteca comunicação mqtt

//informações da rede WIFI
char* ssid = "CSI-Lab"; //SSID da rede WIFI
char* password = "In@teLCS&I"; //senha da rede wifi

//informações do broker MQTT
const char* mqttServer = "192.168.66.32";   //servidor
const int mqttPort = 1883;                  //porta
const char* mqttUser = "csilab";            //usuário
const char* mqttPassword = "WhoAmI#2020";   //senha
const char *ID = "SMARTSHELF";  // Nome do dispositivo - MUDE PARA NÃO HAVER COLISÃO
//topicos criados
const char* topic_bd = "SmartShelf/#";
const char* topic_itens = "SmartShelf/itens";
const char* topic_cliente = "SmartShelf/cliente";

WiFiClient espClient; // Cria o objeto espClient
PubSubClient client(espClient); //instancia o Cliente MQTT passando o objeto espClient

//prototipos da funcoes de conexoes com internet - MQTT
void conectar();
void conectarmqtt();

//quantidade de itens
//String quantidade;

void setup(){
  Serial.begin(115200);//monitor

  //Funções MQTT
  conectar();
  conectarmqtt();
}

void callback(char* topic, byte* message, unsigned int length) {
  String messagestr;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messagestr += (char)message[i];
  }
  Serial.println();
}

void conectar()//Conectar com internet
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

void conectarmqtt () //conectar com broker
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
  client.subscribe(topic_cliente);
  client.subscribe(topic_itens);
  client.subscribe(topic_bd);
}

void loop(){
  //MQTT refresh
  client.loop();
  delay(2000);

  client.publish("SmartShelf/itens", "20");//publicacao no broker MQTT
  client.publish("SmartShelf/cliente", "Larry");//publicacao no broker MQTT
  client.publish("SmartShelf/cliente", "Perry");//publicacao no broker MQTT
}