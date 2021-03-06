/*
 * GSMSim Library
 * 
 * This library written for SIMCOM modules. Tested on Sim800L. Library may worked on any SIMCOM and another GSM modules and GSM Shields. Some AT commands for only SIMCOM modules.
 *
 * Created 11.05.2017
 * By Erdem ARSLAN
 * Modified 06.05.2020
 *
 * Version: v.2.0.1
 *
 * Erdem ARSLAN
 * Science and Technology Teacher, an Arduino Lover =)
 * erdemsaid@gmail.com
 * https://www.erdemarslan.com/
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#ifndef __GSMSim_H__
#define __GSMSim_H__

#include <Arduino.h>

// DEFAULT PIN DEFINATIONS IN HERE
#define DEFAULT_RST_PIN 2
#define DEFAULT_LED_PIN 13
#define DEFAULT_LED_FLAG true


#define BUFFER_RESERVE_MEMORY	255
#define TIME_OUT_READ_SERIAL	5000


// PIN (PERSONAL IDENTIFICATION NUMBER) STATUS
#define PIN_READY 0
#define SIM_PIN 1
#define SIM_PUK 2
#define PH_SIM_PIN 3
#define PH_SIM_PUK 4
#define SIM_PIN2 5
#define SIM_PUK2 6
#define PIN_STATUS_UNKNOWN 7

// CALL STATUS
#define READY 0
#define UNKNOWN 2
#define RINGING 3
#define IN_CALL 4
#define NOT_READ 99

class GSMSim {
  protected :
    Stream& gsm;
    unsigned int _timeout;
    static String _buffer;
    void _readSerial();
    void _readSerial(uint32_t timeout);

  public :
    unsigned int RESET_PIN;
    unsigned int LED_PIN;
    bool	LED_FLAG;
    // S??n??f?? Ba??lat??c??...
    GSMSim(Stream& s) : gsm(s) {
      RESET_PIN = DEFAULT_RST_PIN;
      LED_PIN = DEFAULT_LED_PIN;
      LED_FLAG = DEFAULT_LED_FLAG;
    }

    GSMSim(Stream& s, unsigned int resetPin) : gsm(s) {
      RESET_PIN = resetPin;
      LED_PIN = DEFAULT_LED_PIN;
      LED_FLAG = DEFAULT_LED_FLAG;
    }

    GSMSim(Stream& s, unsigned int resetPin, unsigned int ledPin, bool ledFlag) : gsm(s) {
      RESET_PIN = resetPin;
      LED_PIN = ledPin;
      LED_FLAG = ledFlag;
    }

    // Init GSMSim
    void init();

    // Reset Module
    void reset();

    String sendATCommand(char* command);
    // Telefon fonksiyonunu ayarla
    bool setPhoneFunc(int level);
    // Sinyal kalitesi
    unsigned int signalQuality();
    // operat??re kay??tl?? m??
    bool isRegistered();
    // sim kart tak??l?? m???
    bool isSimInserted();
    // Pin durumu
    unsigned int pinStatus();
    // enter pin code
    bool enterPinCode(char* pinCode);
    // enable pin code
    bool enablePinCode(char* pinCode);
    // disable pin code
    bool disablePinCode(char* pinCode);
    // operat??r ismi
    String operatorName();
    // Sim kartta kay??tl?? operat??r ismi
    String operatorNameFromSim();
    // Telefon durumu
    unsigned int phoneStatus();
    // echo kapal??
    bool echoOff();
    // echo a????k??
    bool echoOn();
    // mod??l ??reticisi
    String moduleManufacturer();
    // mod??l modeli
    String moduleModel();
    // mod??l revizyon
    String moduleRevision();
    // mod??l imei
    String moduleIMEI();
    // mod??l imei de??i??tirme
    bool moduleIMEIChange(char* imeino);
    // mod??l sim no
    String moduleIMSI();
    // mod??l sim operat??r no
    String moduleICCID();
    // zil vol??m??
    unsigned int ringerVolume();
    // zil seviyesini ayarlar
    bool setRingerVolume(unsigned int level);
    // speaker d??zeyi
    unsigned int speakerVolume();
    // speaker d??zeyini ayarla
    bool setSpeakerVolume(unsigned int level);
    // debug modu - verbose mode
    String moduleDebug();
    // Ayarlar?? cihaz ??zerine kaydeder.
    bool saveSettingsToModule();

};

#endif
