#include <Arduino.h>
#include "A4988.h"
#include <SoftwareSerial.h>

int Step = 2;
int Dire = 5;  

int Step2 = 3;
int Dire2 = 6;

int Step3 = 4;
int Dire3 = 7;

int buttonPin = 33; 

int ledPin1 = 41;
int ledPin2 = 40;
int ledPin3 = 42;

//Motor Specs
const int spr = 200;  //Steps per revolution
int RPM = 30;
int Microsteps = 1;  
int stepSize = 10;

int hallSensorPin  = 26;
int hallSensorPin2 = 27;
int hallSensorPin3 = 28;

int homed  = 0;
int homed2 = 0;
int homed3 = 0;

// This is the deck look-up tabel. 
// The index in the array is the card's number for selection perpouses. 
// The first element is the card's name. 
// The scond element is the number of steps from 'home' to turn the motor to get to that card.
// check the commented out calibration code at the bottom of the main loop.
// To calibrate, have that code output the number of steps into the console, and push the button until the desired card comes up, then update the step number in the look up tabel.
// Clunky? yes. Functional? also yes!
String deckFull[80][2] = { 
{"Start","0"},
{"The Fool","8"},
{"The Magician","20"},
{"The High Priestess","28"},
{"The Empress","40"},
{"The Emperor","48"},
{"The Hierophant","60"},
{"The Lovers","68"},
{"The Chariot","80"},
{"Strength","88"},
{"The Hermit","100"},
{"Wheel of Fortune","108"},
{"Justice","120"},
{"The Hanged Man","128"},
{"Death","140"},
{"Temperance","148"},
{"The Devil","160"},
{"The Tower","168"},
{"The Star","180"},
{"The Moon","188"},
{"The Sun","200"},
{"Judgment","208"},
{"The World","220"},

{"Wild Card","228"},

{"Ace of Wands","240"},
{"Two of Wands","248"},
{"Three of Wands","260"},
{"Four of Wands","270"},
{"Five of Wands","278"},
{"Six of Wands","288"},
{"Seven of Wands","300"},
{"Eight of Wands","308"},
{"Nine of Wands","318"},
{"Ten of Wands","328"},
{"Page of Wands","340"},
{"Knight of Wands","348"},
{"Queen of Wands","360"},
{"King of Wands","368"},

{"Ace of Cups","380"},
{"Two of Cups","388"},
{"Three of Cups","400"},
{"Four of Cups","408"},
{"Five of Cups","420"},
{"Six of Cups","428"},
{"Seven of Cups","438"},
{"Eight of Cups","448"},
{"Nine of Cups","458"},
{"Ten of Cups","468"},
{"Page of Cups","480"},
{"Knight of Cups","488"},
{"Queen of Cups","498"},
{"King of Cups","508"},

{"Ace of Pentacles","518"},
{"Two of Pentacles","528"},
{"Three of Pentacles","540"},
{"Four of Pentacles","548"},
{"Five of Pentacles","560"},
{"Six of Pentacles","568"},
{"Seven of Pentacles","580"},
{"Eight of Pentacles","592"},
{"Nine of Pentacles","600"},
{"Ten of Pentacles","610"},
{"Page of Pentacles","620"},
{"Knight of Pentacles","632"},
{"Queen of Pentacles","640"},
{"King of Pentacles","650"},

{"Ace of Swords","660"},
{"Two of Swords","670"},
{"Three of Swords","680"},
{"Four of Swords","690"},
{"Five of Swords","698"},
{"Six of Swords","710"},
{"Seven of Swords","720"},
{"Eight of Swords","728"},
{"Nine of Swords","740"},
{"Ten of Swords","750"},
{"Page of Swords","760"},
{"Knight of Swords","768"},
{"Queen of Swords","780"},
{"King of Swords","788"}
};

int wildCard   = 0;
int critFail   = 0;

int spread[3]   = {0,0,0};
int inverse[3]  = {0,0,0};
String spreadString = "";

int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
int counter = 0;
int steps = 0;

//Providing parameters for motor control
// A4988 stepper(spr, Dire, Step, MS1, MS2, MS3);
A4988 stepper(spr, Dire, Step);
A4988 stepper2(spr, Dire2, Step2);
A4988 stepper3(spr, Dire3, Step3);

void findHome1(int SensorPin) {
  if (homed == 0) {
    stepper.move(200);
    while (digitalRead(SensorPin) == HIGH) {
      stepper.move(2);
    }
    // Cycle to start possition.
    stepper.move(stepSize * 14);
    stepper.stop();
    homed = 1;
  }
}

void findHome2(int SensorPin) {
  if (homed2 == 0) {
    stepper2.move(200);
    while (digitalRead(SensorPin) == HIGH) {
      stepper2.move(2);
    }
    // Cycle to start possition.
    stepper2.move((stepSize * 14));
    stepper2.stop();
    homed2 = 1;
  }
}

void findHome3(int SensorPin) {
  if (homed3 == 0) {
    stepper3.move(200);
    while (digitalRead(SensorPin) == HIGH) {
      stepper3.move(2);
    }
    // Cycle to start possition.
    stepper3.move((stepSize * 14));
    stepper3.stop();
    homed3 = 1;
  }
}

void showCard(int cardNum) {
  if (homed == 1) {
    homed = 0;
    stepper.move(deckFull[cardNum][1].toInt());
    // stepper.stop();
  }
}

void showCard2(int cardNum) {
  if (homed2 == 1) {
    homed2 = 0;
    stepper2.move((deckFull[cardNum][1].toInt()));
    stepper2.stop();
  }
}

void showCard3(int cardNum) {
  if (homed3 == 1) {
    homed3 = 0;
    stepper3.move((deckFull[cardNum][1].toInt()));
    stepper3.stop();
  }
}


void drawCard( int cardPosition) {
  int result;
  result = random(1, 79);
  do
    result = random(1, 79);
  while (result == spread[0] || result == spread[1] || result == spread[2] || result == 23);

  spread[cardPosition] = result;
}

void isCardInverted( int cardPosition) {
  int inverted;
  // 1 in 5 chance of card inverted.
  inverted = random(1, 5);
  if (inverted == 3) {
    inverse[cardPosition] = 1;
  } else {
    inverse[cardPosition] = 0;
  }

}

void resetSpread() {
  spread[0]   = 0;
  spread[1]   = 0;
  spread[2]   = 0;

  inverse[0]  = 0;
  inverse[1]  = 0;
  inverse[2]  = 0;

  wildCard   = 1;
  critFail   = 0;
}

void critRoll() {
  int result;
  result = random(1, 20);

  if (result == 20) {
    wildCard   = 1;
  }

  if (result == 1) {
    critFail   = 1;
  }

}


void setup() {
  Serial1.begin(9600); // For sending to Display
  Serial.begin(19200); // For debugging

  pinMode(Step, OUTPUT);    //Step pin as output
  pinMode(Dire, OUTPUT);    //Direcction pin as output
  pinMode(Step2, OUTPUT);    //Step pin as output
  pinMode(Dire2, OUTPUT);    //Direcction pin as output
  pinMode(Step3, OUTPUT);    //Step pin as output
  pinMode(Dire3, OUTPUT);    //Direcction pin as output

  digitalWrite(Step, LOW);  // Currently no stepper motor movement
  digitalWrite(Dire, LOW);
  digitalWrite(Step2, LOW);  // Currently no stepper motor movement
  digitalWrite(Dire2, LOW);
  digitalWrite(Step3, LOW);  // Currently no stepper motor movement
  digitalWrite(Dire3, LOW);

  pinMode(hallSensorPin, INPUT);
  pinMode(hallSensorPin2, INPUT);
  pinMode(hallSensorPin3, INPUT);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  randomSeed(analogRead(0));

  // Set target motor RPM to and microstepping setting
  stepper.begin(RPM);
  stepper2.begin(RPM);
  stepper3.begin(RPM);

  digitalWrite(ledPin1, HIGH);
  delay(500);
  digitalWrite(ledPin2, HIGH);
  delay(500);
  digitalWrite(ledPin3, HIGH);
  delay(500);
  digitalWrite(ledPin1, LOW);
  delay(500);
  digitalWrite(ledPin2, LOW);
  delay(500);
  digitalWrite(ledPin3, LOW);
  delay(500);

  findHome1(hallSensorPin);
  findHome2(hallSensorPin2);
  findHome3(hallSensorPin3);

  Serial1.println("RESET"); // Send reset command to display.
}


//--------------------------------------------------------------
// MAIN LOOP
//--------------------------------------------------------------
void loop() {
  buttonState = digitalRead(buttonPin);
   if (buttonState != lastButtonState) {
      if (buttonState == HIGH && homed == 1 ){
        

          // DRAW CARDS
          drawCard(0);
          drawCard(1);
          drawCard(2);
          isCardInverted(0);
          isCardInverted(1);
          isCardInverted(2);

          showCard(spread[0]);
          showCard2(spread[1]);
          showCard3(spread[2]);

          if (inverse[0] == 1){
            digitalWrite(ledPin1, HIGH);
          } else {
            digitalWrite(ledPin1, LOW);
          }

          if (inverse[1] == 1){
            digitalWrite(ledPin2, HIGH);
          } else {
            digitalWrite(ledPin2, LOW);
          }

          if (inverse[2] == 1){
            digitalWrite(ledPin3, HIGH);
          } else {
            digitalWrite(ledPin3, LOW);
          }

          // format the drawn cards and send the string to the digital display.
          String transmitString = String(spread[0]) + "," + String(inverse[0]) + "," + String(spread[1]) + "," + String(inverse[1]) + "," + String(spread[2]) + "," + String(inverse[2]);
          
          // Debug Serial
          // Serial.println(transmitString);

          // Send to  ESP32
          Serial1.println(transmitString);
        
        
      } else if (buttonState == HIGH && homed == 0) {
        // RESET
        digitalWrite(ledPin1, LOW);
        digitalWrite(ledPin2, LOW);
        digitalWrite(ledPin3, LOW);
        findHome1(hallSensorPin);
        findHome2(hallSensorPin2);
        findHome3(hallSensorPin3);
        resetSpread();
        Serial1.println("RESET"); // Send reset command to display.
        spreadString = "";
      }

      // CALIBRATION
      // uncomment this, and comment out everything else inside this IF statement.
      // now press the button and note the number of steps shown in the console.
      // When the desired card is shown, change the numer in the lookup array.
    
      // stepper.move(2); 
      // stepper2.move(-2); 
      // stepper3.move(-2); 
      // counter = counter + 1;
      // steps = steps +2;
      // Serial.print(counter);
      // Serial.print(" steps: ");
      // Serial.print(steps);
      // Serial.println();

   }
   lastButtonState = buttonState;

}