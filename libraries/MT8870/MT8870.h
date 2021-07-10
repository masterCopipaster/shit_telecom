#pragma once
//
//    FILE: MT8870.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.2
//    DATE: 2019-02-11
// PURPOSE: Arduino library for MT8870 DTMF decoder (breakout)
//     URL: https://github.com/RobTillaart/MT8870
//          https://www.tinytronics.nl/shop/nl/sensoren/geluid/mt8870-dtmf-module
//

#include "Arduino.h"

#define MT8870_LIB_VERSION "0.1.2"

class MT8870
{
public:
  // STQ - pin to see if new data arrived
  // Q1 - Q4 data pins.
  void    begin(uint8_t STQ, uint8_t Q1, uint8_t Q2, uint8_t Q3, uint8_t Q4);

  bool    available();
  char    read();
  uint8_t readRaw();
  uint8_t lastRaw();

private:
  uint8_t _val = 255;
  uint8_t _STQ;
  uint8_t _q[4];
};

class dtmf_receiver : public MT8870
{
	inline static uint32_t dtmf_ir_time;
	inline static bool dtmf_ir_pending;
	uint32_t dtmf_handle_delay = 20;
	char ch;
	bool symbol_pending = false;
	
	
	public:
	
	dtmf_receiver()
	{
		dtmf_ir_pending = false;
		dtmf_ir_time = 0;
	}
	
	void attachInt(int i)
	{
		attachInterrupt(i, DTMF_IR_handler, RISING);
	}
	
	static void DTMF_IR_handler()
	{
	  dtmf_ir_time = millis();
	  dtmf_ir_pending = true;
	}

	void handle()
	{
	  if (available() && dtmf_ir_pending && (millis() - dtmf_ir_time >= dtmf_handle_delay))
	  {
		ch = read();
		dtmf_ir_pending = false;
		symbol_pending = true;
	  }
	}
	
	void set_read_delay(uint32_t d)
	{
		dtmf_handle_delay = d;
	}
	
	bool is_symbol_pending()
	{
		return symbol_pending;
	}
	
	char read_pending_symbol()
	{
		symbol_pending = false;
		return ch;
	}
	
	
};


// -- END OF FILE --
