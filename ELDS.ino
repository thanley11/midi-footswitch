/******************************
   EDLS-FS v1.0
   for Teensy LC (www.pjrc.com)
   by Notes and Volts
   www.notesandvolts.com
 ******************************/

/******************************
   ** Upload Settings **
   Board: "Teensy LC"
   USB Type: "MIDI"
   CPU Speed: "48 Mhz"
 ******************************/

#include <Bounce.h>

//MIDI Channel
#define MIDI_CHANNEL 1

//Switch CC Numbers
#define LOOP1 38
#define LOOP2 39
#define LOOP4 40
#define LOOP8 41

//Pin Numbers on Teensy
#define L1_PIN 11
#define L2_PIN 10
#define L4_PIN 2
#define L8_PIN 3

//Other
#define SWITCHES 4 // How many switches?
#define VELOCITY 127 // ON Velocity
#define BOUNCE_TIME 15 // Debounce Time (in milliseconds)
//********************

//Global variables
const int DIGITAL_SWITCHES[SWITCHES] = {L1_PIN, L2_PIN, L4_PIN, L8_PIN};
const int CC_NUMBER[SWITCHES] = {LOOP1, LOOP2, LOOP4, LOOP8};

//Set up 'Bounce' Objects
Bounce digital[] =   {
  Bounce(DIGITAL_SWITCHES[0], BOUNCE_TIME),
  Bounce(DIGITAL_SWITCHES[1], BOUNCE_TIME),
  Bounce(DIGITAL_SWITCHES[2], BOUNCE_TIME),
  Bounce(DIGITAL_SWITCHES[3], BOUNCE_TIME),
};

void SendSysExButton1(){
#ifdef DEBUG
    Serial.println(volume, DEC);
#else
    SerialOutput(0xF0);         // SysEx start
    SerialOutput(0x00);         // Manufacturer 0
    SerialOutput(0x01);         // Model 1
    SerialOutput(MIDI_CHANNEL);      // MIDI channel
    SerialOutput(0x42);         // Data
    SerialOutput(0x42);         // Data
    SerialOutput(volume);       // Data
    SerialOutput(0x42);         // Data
    SerialOutput(0x42);         // Data
    SerialOutput(0xF7);         // SysEx end
}

//Setup
void setup() {
  for (int i = 0; i < SWITCHES; i++) {
    pinMode(DIGITAL_SWITCHES[i], INPUT_PULLUP);
  }
}

//Main Loop
void loop() {
  readSwitches();
  while (usbMIDI.read()) {
  }
}

//Read Buttons
void readSwitches() {
  static byte input = 0;

  digital[input].update();

  if (digital[input].fallingEdge()) {
    switch(input){
      case 0:
        SendsysExButton1();
        break;
      case 1:
        SendsysExButton2();
        break;
      default:
        break;

      }
    usbMIDI.sendControlChange(CC_NUMBER[input], VELOCITY, MIDI_CHANNEL);
  }
  else if (digital[input].risingEdge()) {
    usbMIDI.sendControlChange(CC_NUMBER[input], 0, MIDI_CHANNEL);
  }

  input++;
  if (input >= SWITCHES) input = 0;
}
