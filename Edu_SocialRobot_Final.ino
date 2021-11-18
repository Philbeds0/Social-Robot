//VCC --> 5V
//GND --> GND
//VCC --> 5V
//VCC --> 5V
//VCC --> 5V


#include <CapacitiveSensor.h> //Library to use capacitive sensors (in this particular case we use copper plates)
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h" //Library to use the MP3 sd card reader
#include <FastLED.h> //To control multiple leds at once

//Declare serial ports dfplayermini
int serial1 = 10;// RX
int serial2 = 11;// TX


SoftwareSerial mySoftwareSerial(serial1, serial2); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

CapacitiveSensor head_sensor = CapacitiveSensor(4, 2);
CapacitiveSensor back_sensor = CapacitiveSensor(32, 37);
CapacitiveSensor belly_sensor = CapacitiveSensor(51, 50);


//########################Starting cheeks firmware


#define NUM_STRIPS 2

// How many leds in your strip?
#define NUM_LEDS 2


CRGB leds[NUM_STRIPS][NUM_LEDS];


//########################Ending cheeks firmware


int sensorMin = 1023;        // minimum sensor value
int sensorMax = 0;           // maximum sensor value
int sensorValue = 0;
byte head_random = 0;
byte back_random = 0;
byte belly_random = 0;
byte lasthead_random = 0;
byte lastback_random = 0;
byte lastbelly_random = 0;
int playerState = 0; //check if dfplayer mini is playing something


void setup()
{
  randomSeed(analogRead(0));
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, 8>(leds[0], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, 27>(leds[1], 1);

  pinMode(serial1, INPUT);
  pinMode(serial2, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(31, INPUT);

  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  Serial.println(F("Initializing DFPlayer ..."));

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
  }
  else
  {
    Serial.println(F("DFPlayer Mini online."));
  }
  Serial.println(myDFPlayer.readFileCounts());

  myDFPlayer.volume(30);  //Set volume value. From 0 to 30



  // calibrate during the first five seconds
  while (millis() < 5000) {
    sensorValue = digitalRead(4);

    // record the maximum sensor value
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
      Serial.print("Min:");
      Serial.println(sensorValue);
    }

    // record the minimum sensor value
    if (sensorValue < sensorMin) {
      sensorMin = sensorValue;
      Serial.print("Max:");
      Serial.println(sensorValue);
    }
  }
  //Set cheeks to normal
  leds[0][0] = CRGB( 255, 105, 180);
  leds[0][1] = CRGB( 255, 105, 180);
  FastLED.show();


}

void loop()
{

  long head_value = head_sensor.capacitiveSensor(100);
  long back_value = back_sensor.capacitiveSensor(100);
  long belly_value = belly_sensor.capacitiveSensor(100);

  //head_value = map(sensorValue, sensorMin, sensorMax, 50, 255);


  Serial.print("testa:");
  Serial.print(head_value);
  Serial.print("     schiena:");
  Serial.print(back_value);
  Serial.print("    pancia:");
  Serial.println(belly_value);
  delay(10);

  if (head_value > 90 && head_value < 120) {


    head_audio_random();

    while (playerState != 512) {  //while mp3 is playing
      parla();
      playerState = myDFPlayer.readState(); //check if it is still playing
      Serial.println("im in a loop head");
    }
    playerState = myDFPlayer.readState(); //check if it is still playing
    stop_talking();
    sensor_reset();
    Serial.println(playerState);




  }

  if (back_value > 90 && back_value < 120) {


    back_audio_random();

    while (playerState != 512) {  //while mp3 is playing
      parla();
      playerState = myDFPlayer.readState(); //check if it is still playing
      Serial.println("im in a loop back");
    }

    stop_talking();
    normal_cheeks();
    sensor_reset();
    return playerState;
  }

  if (belly_value > 90 && belly_value < 120) {
    belly_audio_random();

    while (playerState != 512) {  //while mp3 is playing
      parla();
      playerState = myDFPlayer.readState(); //check if it is still playing
      Serial.println("im in a loop belly");
    }

    stop_talking();
    sensor_reset();
    return playerState;
  }

  else {
    delay(10);
  }

}

void normal_cheeks() {
  leds[0][0] = CRGB( 255, 105, 180);
  leds[0][1] = CRGB( 255, 105, 180);
  FastLED.show();
}
void parla() {

  leds[1][0] = CRGB( 255, 105, 180);

  FastLED.show();
}
void stop_talking() {
  leds[1][0] = CRGB::Black;
  FastLED.show();
}

void head_audio_random() {

  head_random = random(1, 4);

  Serial.println(head_random);
  while (head_random == lasthead_random) {
    head_random = random(1, 3);

    Serial.println(head_random);
  }

  myDFPlayer.play(head_random);
  lasthead_random = head_random;


}
void back_audio_random() {

  back_random = random(4, 6);
  while (back_random == lastback_random) {
    back_random = random(4, 6);
  }
  myDFPlayer.play(back_random);
  lastback_random = back_random;

}
void belly_audio_random() {

  belly_random = random(7, 9);
  while (belly_random == lastbelly_random) {
    belly_random = random(7, 9);
  }
  myDFPlayer.play(belly_random);
  lastbelly_random = belly_random;

}
void sensor_reset() {
  delay(50); //just to give time to remove the hand, calibration is more precise if you do this
  belly_sensor.reset_CS_AutoCal(); //Stops readings
  back_sensor.reset_CS_AutoCal(); //Stops readings
  head_sensor.reset_CS_AutoCal(); //Stops readings

}
