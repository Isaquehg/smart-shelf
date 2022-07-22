#define IN2    16 //D0
#define IN1    5  //D1

int cortina = 0; //aberta

void setup()
{
  pinMode(IN1, OUTPUT);                  // definições das portas IN1 e IN2 como portas de saidas
  pinMode(IN2, OUTPUT);
  digitalWrite(IN1, 0);               // desligando-os
  digitalWrite(IN2, 0);
}

void subir(int tempo) {
  digitalWrite(IN2, 1);               // acionamento IN2(D0)
  digitalWrite(IN1, 0);                // desativa IN1(D1)
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
