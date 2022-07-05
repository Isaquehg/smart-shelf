#include <Servo.h>

bool sensorFD; // Rightmost sensor    D0
bool sensorD; // Middle-right sensor  D1
bool sensorE; // Middle-left sensor   D2
bool sensorFE; // Leftmost sensor     D5

bool VsensorFD; // Old readings
bool VsensorD;
bool VsensorE;
bool VsensorFE;

bool carga; // Load sensor   SD2

bool x;

String direcao; // Direction the cart is moving to
short opcao; // Selected option received

long tempo;

Servo servo1;


void setup() {
  pinMode(D0, INPUT); // sensorFD
  pinMode(D1, INPUT); // sensorD
  pinMode(D2, INPUT); // sensorE
  pinMode(D5, INPUT); // sensorFE

  pinMode(9, INPUT); // carga
  pinMode(10, INPUT);

  pinMode(D4, OUTPUT); // Left motor
  pinMode(D6, OUTPUT);

  pinMode(D7, OUTPUT); // Right motor
  pinMode(D8, OUTPUT);

  Serial.begin(115200);

  servo1.attach(D3);
}


void loop() {
  lerSensores();
  servo1.write(0);
  opcao = 0; 
  x = 0;
  mover("parado");
  
  while(opcao == 0){  // Waits new item request
    evitarReset();

    if(digitalRead(10) == 1){
      opcao = 2;
    }
    Serial.print("opcao = ");
    Serial.println(opcao);

    Serial.println("esperando...");
  }
  
  lerSensores();

  while(sensorFE == HIGH || sensorFD == HIGH){
    evitarReset();
    lerSensores();
    mover("frente");    
  }
  delay(200);
  
  moverNaLinha();
  virarDireita();
  moverNaLinha();
  virarEsquerda();
  moverNaLinha();

  while(true){ // Algorithm to find the right item
    evitarReset();
        
    while(sensorFE == 0 && sensorFD == 1){
      evitarReset();
      lerSensores();
      
      if(opcao == 1){
        virarDireita();
        break;
      }else{
        mover("frente");
        x = 1;
      }
      delay(100);
    }

    if(opcao == 1){
      break;
    }

    if(x == 1){
      opcao--;
      x = 0;
    }

    moverNaLinha();
  }

  moverNaLinha();

  mover("tras");  // Slides the box into the cart
  delay(75);
  mover("parado");
  meiaVolta();
  servo1.write(90);
  delay(400);

  moverNaLinha();
  virarEsquerda();

  while(sensorFD == LOW){
    evitarReset();
    lerSensores();
    moverNaLinha();
    mover("frente");
    delay(50);
  }
  virarDireita();
  moverNaLinha();
  mover("frente");
  
  while(sensorFE == HIGH || sensorFD == HIGH){
    evitarReset();
    lerSensores();
  }
  
  moverNaLinha();
  
  mover("tras");
  delay(75);
  mover("parado");

  while(carga == 1){  // Waits until someone takes the box
    evitarReset();
    lerSensores();
    delay(1000);
  }

  delay(2000);

  meiaVolta();
  moverNaLinha();
  virarDireita();
  moverNaLinha();
  meiaVolta();
}


void virarDireita(){  // Makes the cart turn sharp right

  while(sensorFD == HIGH){
    lerSensores();
    mover("frente");
  }

  delay(80);

  mover("direita");

  while(true){
    evitarReset();
    lerSensores();

    if(sensorD != VsensorD && sensorD == LOW){
      mover("esquerda");
      delay(25);
      VsensorD = sensorD;
      break;
    }

    VsensorD = sensorD;
  }
}


void virarEsquerda(){  // Makes the cart turn sharp left
  while(sensorFE == HIGH){
    lerSensores();
    mover("frente");
  }

  delay(80);
  
  mover("esquerda");

  while(true){
    evitarReset();
    lerSensores();

    if(sensorE != VsensorE && sensorE == LOW){
      mover("direita");
      delay(25);
      VsensorE = sensorE;
      break;
    }

    VsensorE = sensorE;
  }
}

void meiaVolta(){   // Makes the cart turn around
  mover("esquerda");
  delay(500);
  while(sensorE == LOW){
    evitarReset();
    lerSensores();
  }
  mover("direita");
  delay(50);
  mover("parado");
}


void evitarReset(){  // The ESP8266 being used was defective and wouldn't work correctly without this function
  if ((millis() - tempo) > 1000) {
    delay(0);
    tempo = millis();
  }
}


void lerSensores(){  // Reads sensors
  sensorFD = digitalRead(D0);
  sensorD = digitalRead(D1);
  sensorE = digitalRead(D2);
  sensorFE = digitalRead(D5);

  carga = digitalRead(9);
}


void direcaoLinha(){  // Gets what direction is the line going
  if(sensorE == 0 && sensorD == 0){
    direcao = "frente";
  }else if(sensorE == 1 && sensorD == 0){
    direcao = "esquerda frente";
  }else if(sensorE == 0 && sensorD == 1){
    direcao = "direita frente";
  }else if(sensorE == 1 && sensorD == 1){
    direcao = "parado";
  }
}


void moverNaLinha(){  // Makes the cart follow the line until it hits a mark
  lerSensores();
  while(sensorFE == LOW && sensorFD == LOW){
    evitarReset();
    lerSensores();
    direcaoLinha();
    mover(direcao);
  }
  mover("frente");
  delay(80);
}


void mover(String direcao){  // recieves the direction and moves the cart towards it
  Serial.print("movendo para ");
  Serial.println(direcao);
  
  if(direcao == "frente"){
    digitalWrite(D4, HIGH);
    digitalWrite(D6, LOW);

    digitalWrite(D7, HIGH);
    digitalWrite(D8, LOW);
    
  }else if(direcao == "tras"){
    digitalWrite(D4, LOW);
    digitalWrite(D6, HIGH);

    digitalWrite(D7, LOW);
    digitalWrite(D8, HIGH);
  }else if(direcao == "esquerda frente"){
    digitalWrite(D4, LOW);
    digitalWrite(D6, LOW);

    digitalWrite(D7, HIGH);
    digitalWrite(D8, LOW);
  }else if(direcao == "esquerda tras"){
    digitalWrite(D4, LOW);
    digitalWrite(D6, LOW);

    digitalWrite(D7, LOW);
    digitalWrite(D8, HIGH);
  }else if(direcao == "esquerda"){
    digitalWrite(D4, LOW);
    digitalWrite(D6, HIGH);

    digitalWrite(D7, HIGH);
    digitalWrite(D8, LOW);
    
  }else if(direcao == "direita frente"){
    digitalWrite(D4, HIGH);
    digitalWrite(D6, LOW);

    digitalWrite(D7, LOW);
    digitalWrite(D8, LOW);
  }else if(direcao == "direita tras"){
    digitalWrite(D4, LOW);
    digitalWrite(D6, HIGH);

    digitalWrite(D7, LOW);
    digitalWrite(D8, LOW);
  }else if(direcao == "direita"){
    digitalWrite(D4, HIGH);
    digitalWrite(D6, LOW);

    digitalWrite(D7, LOW);
    digitalWrite(D8, HIGH);
    
  }else if(direcao == "parado"){
    digitalWrite(D4, LOW);
    digitalWrite(D6, LOW);

    digitalWrite(D7, LOW);
    digitalWrite(D8, LOW);
  }
}
