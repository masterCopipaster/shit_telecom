#include "MT8870.h"
#include "dtmf_command_protocol.h"
#include "rms.h"

#include <GSMSimCall.h>
#include <GSMSimSMS.h>
#include <SoftwareSerial.h>

MT8870 DTMF;
DTMF_command_protocol prot;

SoftwareSerial SIM800(8, 9);   // RX, TX

GSMSimCall call(SIM800, 12); // GSMSimCall inherit from GSMSim. You can use GSMSim methods with it.
GSMSimSMS sms(SIM800, 12);

//#define LEVELS_DEBUG

#define PTT_PIN 11
#define RMS_FAST_ALPHA 0.95
#define RMS_NORMAL_ALPHA 0.95
#define RMS_SLOW_ALPHA 0.9995

#define RMS_CONSTR 50
#define RMS_NOISE_CONSTR 50

rms_dc_removal<float, 2> rms0;
rms_dc_removal<float, 2> rms0_noise;

float snr_voice_active = 2.0;
float snr_voice_off = 1.0;
bool voice_active = false;
bool vox_active = true;

uint32_t dtmf_ir_time = 0;
bool dtmf_ir_pending = false;
uint32_t dtmf_handle_delay = 20;

void ptt_release()
{
  pinMode(PTT_PIN, INPUT_PULLUP);
  digitalWrite(13, 0);
}

void ptt_press()
{
  digitalWrite(PTT_PIN, 0);
  pinMode(PTT_PIN, OUTPUT);
  digitalWrite(PTT_PIN, 0);
  digitalWrite(13, 1);
}

void noise_rms_calibrate()
{
  for (int i = 0; i < 10000; i++)
  {
    int val = analogRead(0);
    rms0_noise.update(val);
  }
}

#include "handlers.h"

void DTMF_IR_handler()
{
  dtmf_ir_time = millis();
  dtmf_ir_pending = true;
}

void DTMF_handle()
{
  if (DTMF.available() && dtmf_ir_pending && (millis() - dtmf_ir_time >= dtmf_handle_delay))
  {
    char ch = DTMF.read();

    Serial.print(ch);
    Serial.println();

    prot.update(ch);
    dtmf_ir_pending = false;
  }
}


void setup()
{
  ptt_release();
  Serial.begin(115200);
  DTMF.begin(2, 7, 6, 5, 4);
  attachInterrupt(0, DTMF_IR_handler, RISING);

  rms0.set_casc_alpha(RMS_FAST_ALPHA, 0);
  rms0.set_casc_alpha(RMS_NORMAL_ALPHA, 1);
  rms0_noise.set_casc_alpha(RMS_NORMAL_ALPHA, 0);
  rms0_noise.set_casc_alpha(RMS_SLOW_ALPHA, 1);
  rms0.set_constr(RMS_CONSTR);
  rms0_noise.set_constr(RMS_NOISE_CONSTR);
  //pinMode(10, OUTPUT);

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
  
  
  prot.start_seq_handler = command_start_handler;
  prot.command_end_handler = command_end_handler;
  prot.pause_handler = pause_handler;

  SIM800.begin(9600);

  while(!SIM800) {
    ; // wait for module for connect.
  }

  // Init module...
  call.init(); // use for init module. Use it if you dont have any valid reason.

  Serial.print("Set Phone Function... ");
  Serial.println(call.setPhoneFunc(1));
  delay(100);

  Serial.print("Init Call... ");
  Serial.println(call.initCall()); // Its optional but highly recommended. Some function work with this function.
  delay(100);

  Serial.print("Setting Auto Answer... ");
  Serial.print(call.sendATCommand("ATS0=2"));
  delay(100);

  Serial.print("Enabling Speech Enhancement... ");
  Serial.print(call.sendATCommand("AT+SPE=1")); 
  delay(100);

  Serial.print("Init SMS... ");
  Serial.println(sms.initSMS()); // Its optional but highly recommended. Some function work with this function.
  delay(1000);

  Serial.print("List Unread SMS... ");
  Serial.println(sms.list(true)); // Its optional but highly recommended. Some function work with this function.
  delay(1000);

  /*
  Serial.print("SMS to any number... ");
  Serial.println(sms.send("89854864105", "SOS")); // only use ascii chars please
  */ 
  Serial.print("Calibrating... ");
  noise_rms_calibrate();
  
  tone(10, 300, 1000);

}

void vox_cycle()
{
  int val = analogRead(0);
  if(rms0.getval()/snr_voice_active > rms0_noise.getval() && !voice_active)
  {
    ptt_press();
    voice_active = true;
  }
  if(rms0.getval()/snr_voice_off < rms0_noise.getval() && voice_active)
  {
    ptt_release();
    voice_active = false;
  }
  
  rms0.update(val);
  rms0_noise.update(val);
}

void loop()
{
  DTMF_handle();
  prot.handle();
  // do something else
  if(vox_active)
    vox_cycle();

  
  #ifdef LEVELS_DEBUG
  Serial.print(rms0.getval());
  Serial.print(" ");
  Serial.println(rms0_noise.getval());
  #else
  delayMicroseconds(1500);
  #endif
}
