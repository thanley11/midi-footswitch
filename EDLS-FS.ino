#include <Bounce.h>

// MIDI Channel
#define MIDI_CHANNEL 1

// Switch CC Numbers (for L1 & L2)
#define LOOP1 38
#define LOOP2 39
#define LOOP4 40
#define LOOP8 41

// Program Change Range
#define MIN_PROGRAM 0  
#define MAX_PROGRAM 10  // Set max allowed program changes

// Pin Numbers on Teensy
#define L1_PIN 11
#define L2_PIN 10
#define L4_PIN 2  // Decrease Program Change
#define L8_PIN 3  // Increase Program Change

// Other
#define SWITCHES 4  // Total number of switches
#define VELOCITY 127 // CC ON value
#define BOUNCE_TIME 15  // Debounce Time (in milliseconds)

// Global variables
const int DIGITAL_SWITCHES[SWITCHES] = {L1_PIN, L2_PIN, L4_PIN, L8_PIN};
const int CC_NUMBER[SWITCHES] = {LOOP1, LOOP2, LOOP4, LOOP8};

// Track the current Program Change
int currentProgram = 0;  

// Set up 'Bounce' Objects
Bounce digital[] = {
  Bounce(DIGITAL_SWITCHES[0], BOUNCE_TIME),
  Bounce(DIGITAL_SWITCHES[1], BOUNCE_TIME),
  Bounce(DIGITAL_SWITCHES[2], BOUNCE_TIME),
  Bounce(DIGITAL_SWITCHES[3], BOUNCE_TIME),
};

// Setup
void setup() {
  for (int i = 0; i < SWITCHES; i++) {
    pinMode(DIGITAL_SWITCHES[i], INPUT_PULLUP);
  }
}

// Main Loop
void loop() {
  readSwitches();
  while (usbMIDI.read()) {
  }
}

// Read Buttons
void readSwitches() {
  static byte input = 0;

  digital[input].update();

  if (digital[input].fallingEdge()) {
    if (input < 2) {
      // Send Control Change (L1 & L2)
      usbMIDI.sendControlChange(CC_NUMBER[input], VELOCITY, MIDI_CHANNEL);
    } else {
      // Program Change Handling (Flip behavior)
      if (input == 2 && currentProgram < MAX_PROGRAM) {
        currentProgram++;  // L4 now increases program
      } 
      else if (input == 3 && currentProgram > MIN_PROGRAM) {
        currentProgram--;  // L8 now decreases program
      }
      
      usbMIDI.sendProgramChange(currentProgram, MIDI_CHANNEL);
    }
  }
  else if (digital[input].risingEdge()) {
    if (input < 2) {
      // Send CC OFF (value 0) for L1 & L2, but ignore for L4 & L8
      usbMIDI.sendControlChange(CC_NUMBER[input], 0, MIDI_CHANNEL);
    }
  }

  input++;
  if (input >= SWITCHES) input = 0;
}

