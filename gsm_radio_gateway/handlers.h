#ifndef _HANDLERS_H

#define _HANDLERS_H

void command1_handler(char* c)
{
  Serial.print("CALL: ");
  Serial.write(c, strlen(c));
  Serial.println();
  Serial.println(call.call(c));
  digitalWrite(13, 0);
}

void command2_handler(char* c)
{
  Serial.println("HANGOFF");
  call.hangoff();
  digitalWrite(13, 0);
}

void command3_handler(char* c)
{
  Serial.println("ANSWER");
  call.answer();
  digitalWrite(13, 0);
}

void command_loudness_handler(char* c)
{
  int lvl = atoi(c);
  Serial.println("SET LOUDNESS");
  Serial.print(call.setSpeakerVolume(lvl)); 
  Serial.print(call.setRingerVolume(lvl));
  digitalWrite(13, 0);
}

void command_snr_on_handler(char* c)
{
  Serial.println("SET SNR ON BORDER");
  noise_rms_calibrate();
  if(strlen(c))
    snr_voice_active = float(atoi(c))/100;
  digitalWrite(13, 0);
}

void command_start_handler()
{
  Serial.println("START!");
  digitalWrite(13, 1);
}

void command_end_handler()
{
  Serial.println("END!");
  digitalWrite(13, 0);
  tone(10, 1000, 600);
}

void pause_handler()
{
  tone(10, 500, 600);
}

void command_is_registered_handler(char* c)
{
  Serial.print("is Module Registered to Network?... ");
  bool reg = call.isRegistered();
  Serial.println(reg);
  if(reg)
    tone(10, 500, 600);
  digitalWrite(13, 0);
}

void command_rssi_handler(char* c)
{
  Serial.print("Signal Quality... ");
  int q = call.signalQuality();
  Serial.println(call.signalQuality());
  if (q <= 31)
    tone(10, 500, 1000 * (q / 6));
  else
    tone(10, 1000, 600);
  digitalWrite(13, 0);
}

void command_set_rms_noise_constr(char* c)
{
  Serial.println("SET NOISE RMS CONSTRICTION");
  rms0_noise.set_constr(float(atoi(c)));
  digitalWrite(13, 0);
}

void command_set_rms_constr(char* c)
{
  Serial.println("SET VOICE RMS CONSTRICTION");
  rms0.set_constr(float(atoi(c)));
  digitalWrite(13, 0);
}

void command_snr_off_handler(char* c)
{
  Serial.println("SET SNR OFF BORDER");
  noise_rms_calibrate();
  if(strlen(c))
    snr_voice_off = float(atoi(c))/100;
  digitalWrite(13, 0);
}

void command_vox_onoff_handler(char* c)
{
  Serial.println("VOX ONOFF");
  vox_active = !vox_active;
  digitalWrite(13, 0);
}
#endif 
