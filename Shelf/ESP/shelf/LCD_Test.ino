#include <LiquidCrystal_I2C.h>  // display

// pinagem e inicialização LCD com I2C
//UTILIZAR PORTAS 22(SCL) E 21(SDA)
#define endereco 0x27 // VERIFICAR!!!
#define colunas 16
#define linhas 2
LiquidCrystal_I2C lcd(endereco, colunas, linhas);

void setup(){
  Serial.begin(115200);//monitor
  
  //setup LCD
  lcd.init();
  lcd.backlight();
}
void loop(){
    lcd.print("Hey There!");
}