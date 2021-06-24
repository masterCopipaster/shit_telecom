#ifndef __HT9200
#define __HT9200

class ht9200
{
  int nCE;
  int CLK;
  int DATA;
  int delay_us = 10;

  public:

  ht9200(int dt, int cl, int ce)
  {
    nCE = ce;
    CLK = cl;
    DATA = dt;
  }
  
  void begin()
  {
    pinMode(nCE, OUTPUT);
    digitalWrite(nCE, 1);
    pinMode(CLK, OUTPUT);
    digitalWrite(CLK, 1);
    pinMode(DATA, OUTPUT);
  }

  void on()
  {
    digitalWrite(nCE, 0);
    digitalWrite(CLK, 1);
  }

  void write_raw_digit(uint8_t d)
  {
    for(int i = 0; i < 5; i++)
    {
      digitalWrite(DATA, (d >> i) & 1);
      delayMicroseconds(delay_us);
      digitalWrite(CLK, 0);
      delayMicroseconds(delay_us);
      digitalWrite(CLK, 1);
    }
  }
  
  void end()
  {
    digitalWrite(nCE, 1);
    digitalWrite(CLK, 1);
  }
};

#endif
