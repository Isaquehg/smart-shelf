#include <Stepper.h>       // motor de passo NEMA17
#include <Ultrasonic.h>    // ultrassonico
#include <LiquidCrystal.h> // display
#include <WiFi.h>   // biblioteca do Node MCU
#include <PubSubClient.h>  // biblioteca comunicação mqtt

//NEMA 17 pin setup
#define IN1 15
#define IN2 2
#define IN3 4
#define IN4 16

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 17, en = 5, d4 = 18, d5 = 19, d6 = 21, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//hc-sr04 pin setup
#define trigger 1
#define echo 22

//informações da rede WIFI
char* ssid = "CSI-Lab"; //SSID da rede WIFI
char* password = "In@teLCS&I"; //senha da rede wifi

//informações do broker MQTT
const char* mqttServer = "192.168.66.32";   //servidor
const int mqttPort = 1883;                  //porta
const char* mqttTopicSub = "broker";        //tópico que sera assinado ????????
const char* mqttUser = "csilab";            //usuário
const char* mqttPassword = "WhoAmI#2020";   //senha
const char *ID = "SMARTSHELF";  // Nome do dispositivo - MUDE PARA NÃO HAVER COLISÃO

WiFiClient espClient; // Cria o objeto espClient
PubSubClient client(espClient); //instancia o Cliente MQTT passando o objeto espClient

const char* topicbd = "SmartShelf/#";
const char* topic_itens = "SmarShelf/itens";
const char* topic_cliente = "SmartShelf/cliente";

//prototipos da funcoes de conexoes com internet - MQTT
void conectar();
void conectarmqtt();

//inicializando motor de passo
const int steps_per_rev = 200; //Set to 200 for NEMA 17
Stepper motor(steps_per_rev, IN1, IN2, IN3, IN4);

//inicializando ultrassonico
Ultrasonic ultrassom(trigger, echo);// trigger & echo pins
float dist = 0.0;

//quantidade de itens
int quantidade;

void setup(){
  Serial.begin(115200);//monitor
  motor.setSpeed(60);//nema17 speed
  pinMode(23, OUTPUT);//led1

  //Funções MQTT
  conectar();
  conectarmqtt();

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
}

void callback(char* topic, byte* message, unsigned int length){
  //armazena mensagem recebida em uma variavel inteira - pode mudar a conversão para qualquer outro tipo de variável, se necessário
  //payload[length] = '\0';
  //int MSG = atoi((char*)payload);

  char MSG[15];

  //disribuir mensagens
  if(String(topic) == "SmartShelf/itens"){
    n_itens(MSG);
  }
  else if(String(topic) == "SmartShelf/cliente"){
    //convertendo para string
    for (int i = 0; i < length; i++) {
      MSG += (char)message[i];
    }
    nome_cliente(MSG);
  }
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

//exibir nome cliente
void nome_cliente(char msg){
  lcd.clear();
  lcd.print(msg);
}

void loop(){
  int led1_pin = 2;//verificar quando sensor esta funcionando

  //MQTT refresh
  client.loop();
  delay(2000);

  //obter quantidade de itens em estoque do DB
  int itens = quantidade;

  //configuracao ultrassonico
  dist = ultrassom.Ranging(CM);//calcula da distancia(cm)
  Serial.print("Distancia: ");
  Serial.println(dist);

  //mover atuador
  int giros = 0;
  //numero de revolucoes para mover 1 produto
  int step_produto = steps_per_rev * 5;//MUDAR VALOR!!
  
  digitalWrite(led1_pin, LOW);//nao esta movendo
  while(dist >= 10 && itens > 0){ //distancia sem o produto e com estoque
    digitalWrite(led1_pin, HIGH);
    motor.step(step_produto);
    giros ++;
    delay(500);
    dist = ultrassom.Ranging(CM);
    itens --;
  }
  digitalWrite(led1_pin, LOW);//nao esta movendo

  //publicar nova quantidade de itens 
  char quantidade[10];
  dtostrf(itens, 1, 1, quantidade);
  client.publish("SmartShelf/itens", "quantidade");//publicacao no broker MQTT

  //retornar atuador
  while(giros > 0){
    motor.step(-step_produto);
    giros --;
    delay(500);
  }
}