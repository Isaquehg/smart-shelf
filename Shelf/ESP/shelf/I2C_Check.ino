#include <Wire.h>

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  while (!Serial);
}

void loop()
{
  byte error, address;
  int I2CDevices;

  Serial.println("Scanning for I2C Devices…");

  I2CDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println(" !");

      I2CDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (I2CDevices == 0)
  {
    Serial.println("No I2C devices found\n");
  }
  else
  {
    Serial.println("****\n");
  }
  delay(5000);
}