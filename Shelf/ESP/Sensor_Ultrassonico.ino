#include <Servo.h>

//inicializando Servo
Servo servo;

void setup(){
  Serial.begin(9600);//monitor
  servo.attach(9);//servo motor
  pinMode(1, INPUT);//trigger
  pinMode(2, INPUT);//echo
  pinMode(7, OUTPUT);//led
}
void loop(){
  int som = 0;
  int led1 = 7;
  int led2 = 8;

  //ultrassonico
  long duracao = 0;//duracao de ida e volta
  float dist = 0.0;//distancia
  digitalWrite(10, LOW);//inicializa o trigger
  delayMicroseconds(5);
  digitalWrite(10, HIGH);//envio do sinal
  delayMicroseconds(5);
  digitalWrite(10, LOW);

  duracao = pulseIn(6, HIGH);//inicia a contagem
  dist = duracao * 0.034 / 2.0;//distancia em cm

  //servo
  int angle = analogRead(A0);
  //mapeando valor de 0 a 360
  angle = map(angle, 0, 1023, 360);
  servo.write(angle);
  delay(15);

  //
  if(dist <= 320 && dist > 2){
    delay(100 );
    Serial.println("Movimentacao");
    digitalWrite(led1, HIGH);
    Serial.print("Distancia = ");
    Serial.println(dist);
  }
}
