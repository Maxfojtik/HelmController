#include <FastLED.h>
#include <Servo.h>
#include "pitches.h"

// How many leds in your strip?
#define NUM_LEDS 1
#define DATA_PIN 16

#define HALL_PIN 29
#define HALL_MIN_THRESH 700


#define COIL_TIME 20

int buzzer = 8;
unsigned long buzzerMillis = 0;
int s1Mel[] = {NOTE_C5, NOTE_G4};


Servo coil;
Servo motor;

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() { 
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(HALL_PIN, INPUT);
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
  coil.attach(7);
  motor.attach(6);
  coil.write(90);
  motor.write(90);
  tone(buzzer, NOTE_D4, 1000);
  tone(buzzer, NOTE_D5, 1000);
  noTone(buzzer);
}
byte hue = 0;
int state = 0;
unsigned long coilTimer = 0;
void loop() {
  if(millis() - buzzerMillis >= 500) {
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

void buzz() {
  static int i = 0;
  static int[] currentMel;
  if(state == 1) {
    currentMel = s1Mel[];
  } else {
    currentMel = [0];
    noTone(buzzer);
  }

  tone(buzzer, currentMel[i]);
  if(i+=1 > sizeof(currentMel)) i=0;
}
