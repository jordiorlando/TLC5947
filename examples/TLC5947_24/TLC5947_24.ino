#include <TLC5947.h>

TLC5947 TLC;


void setup()
{
  Serial.begin(57600);
  
  TLC.begin();
}

void loop()
{
  for (int i = 1; i < 23; i++)
  {
    TLC.set(i + 3, 1024);
    TLC.set(i + 2, 128);
    TLC.set(i + 1, 32);
    TLC.set(i, 8);
    
    TLC.update();
    delay(100);
    
    TLC.clear();
  }
  
  /*
  int nRandom = random(1, 25);
  TLC.set(nRandom, random(0, 256));
  TLC.update();
  
  delay(1000);
  
  TLC.clear();
  */
}
