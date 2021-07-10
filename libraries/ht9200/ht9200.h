#ifndef __HT9200
#define __HT9200

#include <string.h>

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

enum
{
	OFF = 0,
	PRE_QIET,
	PLAYING
};

class dtmf_caller : public ht9200
{
	private:
	int state = OFF;
	unsigned long delay_start = 0;
	unsigned int pre_quiet = 10;
	unsigned int play_time = 40;
	char* playing_char = 0;
	char* playing_buf = 0;
	public:
	dtmf_caller(int dt, int cl, int ce) : ht9200(dt, cl, ce){}
	
	char char_to_raw(char c)
	{
		char* tabl = "D1234567890*#ABC";
		char* ptr = strchr(tabl, c);
		return ptr ? ptr - tabl : -1;
	}
	
	void start_sequence(char* s)
	{
		playing_char = s;
		state = PRE_QIET;
		delay_start = millis();
	}
	
	void handle()
	{
		switch(state)
		{
			case OFF:
				break;
			case PRE_QIET:
				if(millis() - delay_start > pre_quiet)
				{
					state = PLAYING;
					write_raw_digit(char_to_raw(*playing_char));
					delay_start = millis();
				}
				break;
			case PLAYING:
				if(millis() - delay_start > play_time)
				{
					playing_char ++;
					if(*playing_char)
					{
						state = PRE_QIET;
						write_raw_digit(31);
						delay_start = millis();
					}
					else
					{
						state = OFF;
						write_raw_digit(31);
					}
				}
				break;
		}
	}
};

#endif
