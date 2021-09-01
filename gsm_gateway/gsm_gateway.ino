#include <MT8870.h>
#include <dtmf_command_protocol.h>
#include <rms.h>
#include <ht9200.h>
#include <protocols.h>
#include <GSMSimCall.h>
#include <GSMSimSMS.h>
#include <SoftwareSerial.h>

dtmf_receiver DTMF;
DTMF_command_protocol <> prot;
sms_dtmf_protocol<> sms_prot;
SoftwareSerial SIM800(8, 9);   // RX, TX

GSMSimCall call(SIM800, 12); // GSMSimCall inherit from GSMSim. You can use GSMSim methods with it.
GSMSimSMS sms(SIM800, 12);
dtmf_caller dtmf_out(A3, A4, A2);

//#define LEVELS_DEBUG

#define PTT_PIN 11
#define RMS_FAST_ALPHA 0.99
#define RMS_SLOW_ALPHA 0.999
bool vox_active = true;
rms_switch<1> ptt_switch;

#define RMS_CONSTR 30

String buffer;

void ptt_release()
{
  digitalWrite(PTT_PIN, 0);
}

void ptt_press()
{
  digitalWrite(PTT_PIN, 1);
}

void noise_rms_calibrate()
{
  for (int i = 0; i < 1000; i++)
  {
    int val = analogRead(0);
    ptt_switch.update(val);
  }
}

void incoming_sms_handler()
{
    if(SIM800.available()) {
      buffer = SIM800.readString();
      //Serial.println(buffer);

      
      // This example for how you catch incoming calls.
      if(buffer.indexOf("+CMTI:") != -1) {
        //sms.sendATCommand("AT+CMGR=37");
        
        Serial.print("SMS Index No... ");
        int indexno = sms.indexFromSerial(buffer);
        Serial.println(indexno);

        Serial.print("Who send the message?...");
        Serial.println(sms.getSenderNo(indexno));

        Serial.print("Read the message... ");
        Serial.println(sms.readFromSerial(buffer));
      } else {
        Serial.println(buffer);
      }
  }
}

#include "handlers.h"

void setup()
{
  pinMode(PTT_PIN, OUTPUT);
  ptt_release();
  Serial.begin(115200);
  DTMF.begin(2, 7, 6, 5, 4);
  DTMF.attachInt(0);
  dtmf_out.begin();
  buffer.reserve(BUFFER_RESERVE_MEMORY);

  ptt_switch.set_main_rms_alpha(RMS_FAST_ALPHA);
  ptt_switch.set_noise_rms_alpha(RMS_SLOW_ALPHA);
  ptt_switch.set_main_rms_constr(RMS_CONSTR);
  ptt_switch.set_noise_rms_constr(RMS_CONSTR);

  prot.command_keys[0] = "1";
  prot.command_handlers[0] = command1_handler; //call
  
  prot.command_keys[1] = "2";
  prot.command_handlers[1] = command2_handler; //hangoff

  prot.command_keys[2] = "3";
  prot.command_handlers[2] = command3_handler; // accept call

  prot.command_keys[3] = "01";
  prot.command_handlers[3] = command_snr_on_handler;

  prot.command_keys[4] = "02";
  prot.command_handlers[4] = command_snr_off_handler;

  prot.command_keys[5] = "03";
  prot.command_handlers[5] = command_rssi_handler;

  prot.command_keys[6] = "04";
  prot.command_handlers[6] = command_is_registered_handler;
  
  prot.command_keys[7] = "05";
  prot.command_handlers[7] = command_set_rms_noise_constr;

  prot.command_keys[8] = "07";
  prot.command_handlers[8] = command_loudness_handler;
  
  prot.command_keys[9] = "08";
  prot.command_handlers[9] = command_vox_onoff_handler;

  prot.command_keys[10] = "0";
  prot.command_handlers[10] = command_0;

  prot.command_keys[11] = "4";
  prot.command_handlers[11] = command_sms_handler;

  
  
  prot.start_seq_handler = command_start_handler;
  prot.command_end_handler = command_end_handler;
  prot.pause_handler = pause_handler;

  SIM800.begin(9600);

  while(!SIM800) {
    ; // wait for module for connect.
  }

  // Init module...
  call.init(); // use for init module. Use it if you dont have any valid reason.
/*
  Serial.print("Resetting GSM module... ");
  Serial.print(call.sendATCommand("ATZ0")); 
  Serial.print(call.sendATCommand("AT&F")); 
  delay(100);
*/    
  Serial.print("Init SMS... ");
  Serial.println(sms.initSMS()); // Its optional but highly recommended. Some function work with this function.
  delay(100);
  
  Serial.print("Set Phone Function... ");
  Serial.println(call.setPhoneFunc(1));
  delay(100);

  Serial.print("Init Call... ");
  Serial.println(call.initCall()); // Its optional but highly recommended. Some function work with this function.
  delay(100);

  Serial.print("Setting Auto Answer... ");
  Serial.print(call.sendATCommand("ATS0=3"));
  delay(100);

  Serial.print("Enabling Speech Enhancement... ");
  Serial.print(call.sendATCommand("AT+SPE=1")); 
  delay(100);

  Serial.print("Location... ");
  Serial.print(call.sendATCommand("AT+CIPGSMLOC=1,1")); 
  delay(100);

  Serial.print("List SMS... ");
  Serial.println(sms.list(false)); // Its optional but highly recommended. Some function work with this function.
  delay(100);

  /*
  Serial.print("SMS to any number... ");
  Serial.println(sms.send("89854864105", "SOS")); // only use ascii chars please
  */ 
  Serial.println("Calibrating... ");
  noise_rms_calibrate();

  Serial.println("Beep");
  tone(10, 300, 1000);

}

void loop()
{
  DTMF.handle();
  dtmf_out.handle();
  if(DTMF.is_symbol_pending())
    prot.update(DTMF.read_pending_symbol());
  prot.handle();
  //incoming_sms_handler();
  // do something else
  
  int val = analogRead(0);
  if(vox_active)
  {
    ptt_switch.update(val);
    if(ptt_switch.is_switched_on())
      ptt_press();
    if(ptt_switch.is_switched_off())
      ptt_release();
  }
  #ifdef LEVELS_DEBUG
  ptt_switch.print_vals();
  #else
  delayMicroseconds(1500);
  #endif
}
