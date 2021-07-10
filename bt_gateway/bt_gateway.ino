#include <MT8870.h>
#include <dtmf_command_protocol.h>
#include <rms.h>
#include <ht9200.h>

#include <GSMSimCall.h>
#include <GSMSimSMS.h>
#include <SoftwareSerial.h>

dtmf_receiver DTMF;
DTMF_command_protocol prot;

SoftwareSerial SIM800(8, 9);   // RX, TX

GSMSimCall call(SIM800, 12); // GSMSimCall inherit from GSMSim. You can use GSMSim methods with it.
GSMSimSMS sms(SIM800, 12);

dtmf_caller dtmf_out(A1, A2, 10);

//#define LEVELS_DEBUG

#define PTT_PIN 11
#define RMS_FAST_ALPHA 0.99
#define RMS_SLOW_ALPHA 0.999

bool vox_active = true;
rms_switch<1> ptt_switch;

#define RMS_CONSTR 30

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
  for (int i = 0; i < 10000; i++)
  {
    int val = analogRead(0);
    ptt_switch.update(val);
  }
}

#include "handlers.h"

void setup()
{
  pinMode(PTT_PIN, OUTPUT);
  ptt_release();
  Serial.begin(115200);
  DTMF.begin(2, 4, 5, 6, 7);
  DTMF.attachInt(0);

  dtmf_out.begin();

  ptt_switch.set_main_rms_alpha(RMS_FAST_ALPHA);
  ptt_switch.set_noise_rms_alpha(RMS_SLOW_ALPHA);
  ptt_switch.set_main_rms_constr(RMS_CONSTR);
  ptt_switch.set_noise_rms_constr(RMS_CONSTR);

  prot.command_keys[0] = "1";
  prot.command_handlers[0] = command1_handler;
  
  prot.command_keys[1] = "2";
  prot.command_handlers[1] = command2_handler;

  prot.command_keys[2] = "3";
  prot.command_handlers[2] = command3_handler;

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
  
  
  prot.start_seq_handler = command_start_handler;
  prot.command_end_handler = command_end_handler;
  prot.pause_handler = pause_handler;

  SIM800.begin(9600);

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
