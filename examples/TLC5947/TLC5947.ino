#include <TLC5947.h>

TLC5947 TLC;

uint8_t nSpeed = map(analogRead(A0), 0, 1023, 0, 250);

void setup()
{
  TLC.begin();
  
  fadeUp(1);
}

void loop()
{
  
  for (int i = 1; i < 4; i++)
  {
    if (i == 3)
      fadeUp(1);
    else
      fadeUp(i + 1);
    
    fadeDown(i);
  }
  
  /*
  fadeUp(3);
  fadeDown(1);
  fadeDown(3);
  fadeUp(1);
  */
}

void fadeUp(int nChannel)
{
  for (int i = 0; i < (4096 - nSpeed); i += nSpeed)
  {
    TLC.set(nChannel, i);
    TLC.update();
    
    nSpeed = map(analogRead(A0), 0, 1023, 0, 250);
    
    delay(1);
  }
}

void fadeDown(int nChannel)
{
  for (int i = 4095; i > (nSpeed - 1); i -= nSpeed)
  {
    TLC.set(nChannel, i);
    TLC.update();
    
    nSpeed = map(analogRead(A0), 0, 1023, 0, 250);
    
    delay(1);
  }
}
