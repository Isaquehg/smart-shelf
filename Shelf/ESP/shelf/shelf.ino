#include <Servo.h>

//inicializando Servo
Servo servo;

void setup(){
  Serial.begin(9600);//monitor
  pinMode(1, INPUT);//trigger
  pinMode(2, INPUT);//echo
  servo.attach(3);//servo motor
  pinMode(7, OUTPUT);//led
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
}
