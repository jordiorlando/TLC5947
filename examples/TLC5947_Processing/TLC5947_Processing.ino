#include <TLC5947.h>

TLC5947 TLC;

int nRandom = 5;


void setup()
{
  Serial.begin(57600);
  
  TLC.begin();
}

void loop()
{
  /*
  if ((millis() % 100) < 20)
  {
    TLC.set(nRandom, 0);
    nRandom = random(5, 21);
    TLC.set(nRandom, 4095);
    TLC.update();
  }
  */
  
  if (Serial.available() == 3)
  {
    uint16_t nRed = Serial.read();
    uint16_t nGreen = Serial.read();
    uint16_t nBlue = Serial.read();
    
    nRed = int(nRed);
    nGreen = int(nGreen);
    nBlue = int(nBlue);
    
    nRed = map(nRed, 0, 255, 0, 4095);
    nGreen = map(nGreen, 0, 255, 0, 4095);
    nBlue = map(nBlue, 0, 255, 0, 4095);
    
    TLC.set(1, nRed);
    TLC.set(2, nGreen);
    TLC.set(3, nBlue);
    
    TLC.update();
    
    delay(1);
  }
}
