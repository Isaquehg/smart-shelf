#include <Stepper.h>            // motor de passo NEMA17
#include <Ultrasonic.h>         // ultrassonico
#include <LiquidCrystal_I2C.h>  // display
#include <WiFi.h>               // biblioteca do Node MCU
#include <PubSubClient.h>       // biblioteca comunicação mqtt

//NEMA 17 pin setup
#define IN1 15
#define IN2 2
#define IN3 4
#define IN4 16
//inicializando motor de passo
const int steps_per_rev = 200; //Set to 200 for NEMA 17
Stepper motor(steps_per_rev, IN1, IN2, IN3, IN4);

// pinagem e inicialização LCD com I2C
#define endereco 0x27 // VERIFICAR!!!
#define colunas 16
#define linhas 2
LiquidCrystal_I2C lcd(endereco, colunas, linhas);

//HC-SR04 pin setup
#define trigger 1
#define echo 22
//inicializando ultrassonico
Ultrasonic ultrassom(trigger, echo);// trigger & echo pins
float dist = 0.0;

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
//topicos criados
const char* topicbd = "SmartShelf/#";
const char* topic_itens = "SmartShelf/itens";
const char* topic_cliente = "SmartShelf/cliente";

WiFiClient espClient; // Cria o objeto espClient
PubSubClient client(espClient); //instancia o Cliente MQTT passando o objeto espClient

//prototipos da funcoes de conexoes com internet - MQTT
void conectar();
void conectarmqtt();

//quantidade de itens
int quantidade;

void setup(){
  Serial.begin(115200);//monitor
  motor.setSpeed(60);//nema17 speed
  pinMode(2, OUTPUT);//led1

  //Funções MQTT
  conectar();
  conectarmqtt();

  //setup LCD
  lcd.init();
  lcd.backlight();
}

void callback(char* topic, byte* message, unsigned int length) {
  String messagestr;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messagestr += (char)message[i];
  }
  Serial.println();

  if (String(topic) == topic_cliente) {
    Serial.print("Changing display");
    lcd.clear();
    lcd.print(messagestr);
  }
  else if(String(topic) == topic_itens){
    Serial.print("Changing itens");
    quantidade = messagestr.toInt();
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

void loop(){
  //receber dados do DB ??????
  client.subscribe(topic_cliente);
  client.subscribe(topic_itens);

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
  char quantidade[11];
  dtostrf(itens, 1, 1, quantidade);
  client.publish("SmartShelf/itens", quantidade);//publicacao no broker MQTT

  //retornar atuador
  while(giros > 0){
    motor.step(-step_produto);
    giros --;
    delay(500);
  }
}