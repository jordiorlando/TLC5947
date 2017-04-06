#include <TLC5947.h>

TLC5947 TLC;


void setup()
{
  TLC.begin();
}

void loop()
{
  for (int i = 24; i > 0; i--)
  {
    TLC.clear();
    TLC.set(i, 4095);
    TLC.update();
    
    delay(250);
    
    TLC.clear();
  }
}
