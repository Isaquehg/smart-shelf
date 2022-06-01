#include <Servo.h>
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

//inicializando Servo
Servo servo;

void setup(){
  Serial.begin(115200);//monitor
  pinMode(1, INPUT);//trigger
  pinMode(2, INPUT);//echo
  servo.attach(3);//servo motor
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

void loop(){
  int led1 = 7;//verificar quando sensor funciona
  int angle = 0;//angulo para mover o produto(7.5cm)
  int angulo_movido = 0;//quantas movimentacoes foram feitas
  bool movido = false;//verificar se o atuador mexeu

  //obter quantidade de itens em estoque do DB
  int itens = 5;//prateleira cheia(obter valor do DB)

  //configuracao ultrassonico
  long duracao = 0;//duracao de ida e volta
  float dist = 0.0;//distancia
  digitalWrite(1, LOW);//inicializa o trigger
  delayMicroseconds(5);
  digitalWrite(1, HIGH);//envio do sinal
  delayMicroseconds(5);
  digitalWrite(1, LOW);//retorna o som

  duracao = pulseIn(2, HIGH);//inicia a contagem
  dist = duracao * 0.034 / 2.0;//distancia em cm

  //configuracao servo
  servo.write(angle);
  delay(15);
  angle = 45;//valor para mover 7.5cm
  
  //mover produtos
  delay(30);
  Serial.println(itens);
  while(dist >= 50 && itens > 0){ //distancia sem o produto
    delay(5000);
    itens -= 1;//remover uma unidade do estoque
    servo.write(angle);//mover atuador
    angulo_movido += angle;
    movido = true;
    digitalWrite(led1, HIGH);
    Serial.print("Distancia = ");
    Serial.println(dist);
  }
  Serial.print("Passou: ");
  Serial.println(itens);
  if(movido == true){ //se entrou no loop anterior
    delay(500);
    servo.write(-angulo_movido);//voltar o atuador para posicao inicial
  }


  //MQTT  
  client.loop();
  delay(2000);
  Serial.println("publicou!");
  client.publish("node", "oi");//publicacao no broker MQTT
}
