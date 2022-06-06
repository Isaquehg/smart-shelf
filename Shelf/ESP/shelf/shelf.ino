#include <Stepper.h>       // motor de passo NEMA17
#include <Ultrassonic.h>   // ultrassonico
#include <ESP8266WiFi.h>   // biblioteca do Node MCU
#include <PubSubClient.h>  // biblioteca comunicação mqtt

//nema pin setup
#define IN1 14
#define IN2 27
#define IN3 26
#define IN4 25

//informações da rede WIFI
const char* ssid = "CSI-Lab"; //SSID da rede WIFI
const char* password = "In@teLCS&I"; //senha da rede wifi

//informações do broker MQTT
const char* mqttServer = "192.168.66.32";   //servidor
const int mqttPort = 1883;                  //porta
const char* mqttTopicSub = "broker";        //tópico que sera assinado
const char* mqttUser = "csilab";            //usuário
const char* mqttPassword = "WhoAmI#2020";   //senha
const char *ID = "SMARTSHELF";  // Nome do dispositivo - MUDE PARA NÃO HAVER COLISÃO

WiFiClient espClient; // Cria o objeto espClient
PubSubClient client(espClient); //instancia o Cliente MQTT passando o objeto espClient

//prototipos da funcoes de conexoes com internet - MQTT
void conectar();
void conectarmqtt();

//inicializando motor de passo
Stepper motor(steps_per_rev, IN1, IN2, IN3, IN4);
const int steps_per_rev = 200; //Set to 200 for NEMA 17

//inicializando ultrassonico
Ultrassonic ultrassom(7, 8);// trigger & echo pins
float dist = 0.0;

void setup(){
  Serial.begin(115200);//monitor
  motor.setSpeed(60);//nema17 speed
  pinMode(1, INPUT);//trigger
  pinMode(2, INPUT);//echo
  pinMode(7, OUTPUT);//led

  //Funções MQTT
  conectar();
  conectarmqtt();
}

void callback(char* topic, byte* payload, unsigned int length){
  //armazena mensagem recebida em uma variavel inteira - pode mudar a conversão para qualquer outro tipo de variável, se necessário
  payload[length] = '\0';
  int MSG = atoi((char*)payload);
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
  client.subscribe(mqttTopicSub);

}

void loop(){
  int led1_pin = 2;//verificar quando sensor esta funcionando

  //obter quantidade de itens em estoque do DB
  int itens = 5;//prateleira cheia(obter valor do DB)

  //configuracao ultrassonico
  dist = ultrasson.Ranging(CM);//calcula da distancia(cm)
  Serial.print("Distancia: ");
  Serial.println(dist);

  //configuracao servo
  servo.write(0);//posicao inicial
  
  //MQTT  
  client.loop();
  delay(2000);
  Serial.println("publicou!");
  client.publish("node", "smart shelf");//publicacao no broker MQTT

  //mover atuador
  int giros = 0;
  digitalWrite(led1_pin, LOW);//nao esta movendo
  while(dist >= 50 && itens > 0){ //distancia sem o produto e com estoque
    digitalWrite(led1_pin, HIGH);
    motor.step(steps_per_rev);
    giros ++;
    delay(500);
  }
  digitalWrite(led1_pin, LOW);//nao esta movendo

  //retornar atuador
  while(giros > 0){
    motor.step(-steps_per_rev);
    giros --;
    delay(500);
  }
}
