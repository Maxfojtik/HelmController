#include <FastLED.h>
#include <Servo.h>
#include "pitches.h"

#define DATA_PIN 16
#define BUZZER_PIN 8


// How many leds in your strip?
#define NUM_LEDS 1

#define HALL_PIN 29
#define HALL_MIN_THRESH 700


#define COIL_TIME 20
#define MEL_LENGTH 10

unsigned long buzzerMillis = 0;

int startMel[MEL_LENGTH] = {NOTE_C5, 1, NOTE_G4, 1, 0};
int s1Mel[MEL_LENGTH] = {NOTE_C5, 1, NOTE_G4, 1, 0};
int nullMel[MEL_LENGTH] = {0};
                              //s0,     s1,    s2,    s3
int stateMel[][4]           = {*startMel, *s1Mel, *nullMel, *nullMel};
boolean numberLoops[]       = {1,        9999,  0,       0};
Servo coil;
Servo motor;

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() { 
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(HALL_PIN, INPUT);
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
  coil.attach(7);
  motor.attach(6);
  coil.write(90);
  motor.write(90);
  noTone(BUZZER_PIN);
}
byte hue = 0;
int state = 0;
unsigned long coilTimer = 0;
void loop() {
  if(millis() - buzzerMillis >= 1000) {
    buzzerMillis = millis();
    buzz();
  }
  if(state == 0)//off
  {
    leds[0] = CRGB(32,32,32);
    FastLED.show();
    coil.write(90);
    motor.write(90);
    coilTimer = millis();
  }
  else if(state == 1)//search
  {
    leds[0] = CRGB(0,32,32);
    coil.write(90);
    motor.write(0);
    if(analogRead(HALL_PIN) > HALL_MIN_THRESH)
    {
      state = 2;
    }
    coilTimer = millis();
  }
  else if(state == 2 && (millis()-coilTimer)<COIL_TIME)//hard in
  {
    leds[0] = CRGB(0,32,0);
    FastLED.show();
    coil.write(0);
    motor.write(90);
    if(millis()-coilTimer>=COIL_TIME)
    {
      state = 3;
    }
  }
  else if(state == 2)//hard in
  {
    state = 3;
  }
  else if(state == 3)//stay on
  {
    leds[0] = CRGB(0,0,32);
    FastLED.show();
    coil.write(82);
    motor.write(90);
  }
  if(millis()%15000>14500)
  {
    state = 1;
  }
  Serial.print(millis());
  Serial.print("\t");
  Serial.print(state);
  Serial.print("\t");
  Serial.println(analogRead(HALL_PIN));
}

void copyMel(int *current, int *newMel, int len)
{
  for(int i = 0; i < len; i++)
  {
    current[i] = newMel[i];
  }
}

int melNote = 0;
int currentMel[MEL_LENGTH];
int currentMelState = -1;
int loops = 0;
void buzz() {
  if(currentMelState != state)
  {
    currentMelState = state;
    melNote = 0;
    loops = 0;
    copyMel(currentMel, stateMel[state], MEL_LENGTH);
  }
  int loopCount = numberLoops[currentMelState];
  int hz = currentMel[melNote];
  if(hz==1 || hz==0)
  {
    noTone(BUZZER_PIN);
  }
  else
  {
    tone(BUZZER_PIN, hz);
  }
  if(currentMel[melNote]>0)
  {
    melNote++;
  }
  else
  {
    if(loops<loopCount)
    {
      melNote = 0;
      loops++;
    }
  }

  //todo: make hard in melody
}
