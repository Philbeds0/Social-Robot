//VCC --> 5V
//GND --> GND
//VCC --> 5V
//VCC --> 5V
//VCC --> 5V


#include <CapacitiveSensor.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <FastLED.h>

//Dichiaro porte seriali dfplayermini
int serial1 = 10;// RX
int serial2 = 11;// TX

//Associo la porta seriale creata ai 2 pin
SoftwareSerial mySoftwareSerial(serial1, serial2); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

CapacitiveSensor sensoretesta = CapacitiveSensor(4, 2);
CapacitiveSensor sensoreschiena = CapacitiveSensor(32, 37);
CapacitiveSensor sensorepancia = CapacitiveSensor(51, 50);


//########################Inizio codice guance


#define NUM_STRIPS 2

// How many leds in your strip?
#define NUM_LEDS 2


CRGB leds[NUM_STRIPS][NUM_LEDS];


//########################Fine codice guance


int sensorMin = 1023;        // minimum sensor value
int sensorMax = 0;           // maximum sensor value
int sensorValue = 0;
byte testarandom = 0;
byte schienarandom = 0;
byte panciarandom = 0;
byte lasttestarandom = 0;
byte lastschienarandom = 0;
byte lastpanciarandom = 0;
int playerState = 0; //Controlla se il dfplayer mini sta riproducendo una canzone


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
  //Setto guance normali
  leds[0][0] = CRGB( 255, 105, 180);
  leds[0][1] = CRGB( 255, 105, 180);
  FastLED.show();


}

void loop()
{

  long valoretesta = sensoretesta.capacitiveSensor(100);
  long valoreschiena = sensoreschiena.capacitiveSensor(100);
  long valorepancia = sensorepancia.capacitiveSensor(100);

  //valoretesta = map(sensorValue, sensorMin, sensorMax, 50, 255);


  Serial.print("testa:");
  Serial.print(valoretesta);
  Serial.print("     schiena:");
  Serial.print(valoreschiena);
  Serial.print("    pancia:");
  Serial.println(valorepancia);
  delay(10);

  if (valoretesta > 90 && valoretesta < 120) {


    testaaudiorandom();

    while (playerState != 512) {  //Finché la scheda sta riproducendo
      parla();
      playerState = myDFPlayer.readState(); //Controllo se sta riproducendo ancora
      Serial.println("Sonoinlooptesta");
    }
    playerState = myDFPlayer.readState(); //Controllo se sta riproducendo ancora
    bastaparla();
    resetsensore();
    Serial.println(playerState);




  }

  if (valoreschiena > 90 && valoreschiena < 120) {


    schienaaudiorandom();

    while (playerState != 512) {  //Finché la scheda sta riproducendo
      parla();
      playerState = myDFPlayer.readState(); //Controllo se sta riproducendo ancora
      Serial.println("Sonoinloopschiena");
    }

    bastaparla();
    guancenormali();
    resetsensore();
    return playerState;
  }

  if (valorepancia > 90 && valorepancia < 120) {
    panciaaudiorandom();

    while (playerState != 512) {  //Finché la scheda sta riproducendo
      parla();
      playerState = myDFPlayer.readState(); //Controllo se sta riproducendo ancora
      Serial.println("Sonoinlooppancia");
    }

    bastaparla();
    resetsensore();
    return playerState;
  }

  else {
    delay(10);
  }

}

void guancenormali() {
  leds[0][0] = CRGB( 255, 105, 180);
  leds[0][1] = CRGB( 255, 105, 180);
  FastLED.show();
}
void parla() {

  leds[1][0] = CRGB( 255, 105, 180);

  FastLED.show();
}
void bastaparla() {
  leds[1][0] = CRGB::Black;
  FastLED.show();
}

void testaaudiorandom() {

  testarandom = random(1, 4);

  Serial.println(testarandom);
  while (testarandom == lasttestarandom) {
    testarandom = random(1, 3);

    Serial.println(testarandom);
  }

  myDFPlayer.play(testarandom);
  lasttestarandom = testarandom;


}
void schienaaudiorandom() {

  schienarandom = random(4, 6);
  while (schienarandom == lastschienarandom) {
    schienarandom = random(4, 6);
  }
  myDFPlayer.play(schienarandom);
  lastschienarandom = schienarandom;

}
void panciaaudiorandom() {

  panciarandom = random(7, 9);
  while (panciarandom == lastpanciarandom) {
    panciarandom = random(7, 9);
  }
  myDFPlayer.play(panciarandom);
  lastpanciarandom = panciarandom;

}
void resetsensore() {
  delay(50); //per dare tempo alla mano di togliersi cosi l'autocalibrazione sarà più precisa
  sensorepancia.reset_CS_AutoCal(); //Stops readings
  sensoreschiena.reset_CS_AutoCal(); //Stops readings
  sensoretesta.reset_CS_AutoCal(); //Stops readings

}
