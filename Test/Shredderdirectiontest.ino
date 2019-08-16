/*
  "Wendeschaltung" through arduino
  this sketch lets an ac induction motor switch direction
  at the push of a button
  there's a three-state button: forward, reverse and stop. And there's a big switch that switches off everything.
  There's a safety in the wiring that makes sure both directions cannot switch on both at once,
  but the arduino wouldn't do that either.
*/

// constants won't change. They're used here to set pin numbers:
const int shredButton = 6;     // the number of the pin that registers if you want the machine to shred
const int reverseButton = 7;    // the number of the pin that registers if you want the machine to reverse
const int shredPin = 3;     // the number of the forward pin
const int reversePin = 4;     // the number of the reverse pin
const int measurePin = A0;   // this pin has a hall sensor connected to it that measures the output current to the motor


int maxJams = 4;     //this int sets the max amount of jams in a set time
int minJamTime = 10000; //this int sets that time in milliseconds (ten seconds now)

int jamTime;        //this int will store time between problematic jams
int current;         // this in will store the measured current
int currentCap = 700;   // this is the value over which the hall sensor signal will register as a jam.
int jammed = 0;         // this int will store the amount of Jams within the minJamTime
int startTime;        // this int is needed to count the interval between jams.


///So here comes what the machine will do on startup
void setup() {
  // initialize the output pins:
  pinMode(shredPin, OUTPUT);
  pinMode(reversePin, OUTPUT);
  // initialize input pins:
  pinMode(shredButton, INPUT);
  pinMode(reverseButton, INPUT);
  pinMode(measurePin, INPUT);

  Serial.begin(9600);  // use serial, just to check what the machine is doing
  current = analogRead(measurePin);   //measure the initial current
}


// here comes what the machine will do while running, which is basically shred, reverse or do nothing
void loop() {
  delay(1000);
  shred();
  delay(10000);
  halt();
  delay(1000);
  reverse();
  delay(10000);
  halt();
}


void shred() {
  Serial.println("I am shredding now");   //tell the world you are shredding
  digitalWrite(reversePin, HIGH);                        // Make sure you are definitely not turning the other way
  digitalWrite(shredPin, LOW);                         // and switch on the motor in the shred direction
}


void reverse() {
  Serial.println("I am going back");   //tell the world you are shredding
  digitalWrite(shredPin, HIGH);                        // Make sure you are definitely not turning the other way
  digitalWrite(reversePin, LOW);                          // and switch on the motor in the shred direction
}

void halt() {
  Serial.println("STOP");   //tell the world you are coming to a halt
  digitalWrite(shredPin, HIGH);                        // Switch it all off
  digitalWrite(reversePin, HIGH);
}

void countJams() {
  if (jammed == 0) {             // if this is the first time jamming occurs
    startTime = millis();       //start counting the time
    jammed++;                     // and count the jams
  }
  else {                        //if this is not the first time jamming occurs
    jamTime = millis() - startTime;       //check how much time is between jams and store this in jamTime
    if (jamTime << minJamTime) {        //if that is inside unacceptable limits
      jammed++;                         //count this as a jam
      startTime = millis();       //reset the start time
    }
    else {                        //If it has been ages since the last jam
      jammed = 1;                 //start counting again (while shredding, the jammed counter supposedly is 1)
    }
  }
}

void alarm() {
  Serial.println("I am jammed");   //tell the world something is wrong
}
