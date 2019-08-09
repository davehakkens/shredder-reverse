/*
  this sketch lets an ac induction motor switch direction
  at the push of a three-state button: forward, reverse and stop. And there's a big switch that switches off everything.
  There's a safety in the wiring that makes sure both directions cannot switch on both at once,
  but the arduino wouldn't do that either.
  This sketch also monitors the current through the motor with a Hall sensor. If the current rises above a set limit, |
  the machine will register a jam. It will turn back for a second and try again for a set amount of times. If it continues
  to be jammed it switches off.
  Oh and it also makes use of a display to tell the world how it is doing.
  It is possible to use the serial plotter to monitor current, since that is the only number that is dumped on serial.
*/
#include <LiquidCrystal_I2C.h>            //use the LCD Display (yes it is one of those i2c things)
#include <Wire.h>
// constants won't change. They're used here to set pin numbers:
const int shredButton = 7;     // the number of the pin that registers if you want the machine to shred
const int reverseButton = 6;    // the number of the pin that registers if you want the machine to reverse
const int motionPin = 4;     // the number of the pin that decides if the motor turns
const int directionPin = 3;     // the number of the pin that decides the driection of the motor
const int measurePin = A0;   // this pin has a hall sensor connected to it that measures the output current to the motor
int maxJams = 3;     //this int sets the max amount of jams in a set time
int minJamTime = 10000; //this int sets that time in milliseconds (ten seconds now)
int jamTime;        //this int will store time between problematic jams
int current;         // this in will store the measured current
int currentCap = 700;   // this is the value over which the hall sensor signal will register as a jam.
int jammed = 0;         // this int will store the amount of Jams within the minJamTime
int startTime;        // this int is needed to count the interval between jams.
int shredstate;      //
int reversestate;    //
LiquidCrystal_I2C lcd(0x38, 20, 2);           //set the address and dimensions of the LCD. Here the address is 0x38, but it depends on the chip. You can use and i2c-scanner to determine the address of your chip.
///So here comes what the machine will do on startup
void setup() {
  lcd.init();                                       //Setup the LCD
  lcd.setBacklight(HIGH);
  lcd.print("COWABUNGA!"); // wtf does this mean?
  // initialize the output pins:
  pinMode(motionPin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  // initialize input pins:
  pinMode(shredButton, INPUT);
  pinMode(reverseButton, INPUT);
  pinMode(measurePin, INPUT);
  Serial.begin(9600);  // use serial, just to check what the machine is doing
  measureCurrent();     //measure the current current
}
// here comes what the machine will do while running, which is basically shred, reverse or do nothing
void loop() {
  lcd.setCursor(0, 1);
  lcd.print(current);
  lcd.print(" out of ");
  lcd.print(currentCap);
  if (jammed <= maxJams) {                 //as long as the machine isn't jammimg too often in a set time limit
    if (current <= currentCap) {              //if the measured current is under the cap
      checkDirection();                        // check in what direction you are asked to turn and move
      Serial.println(current);                  // and you can read the current over serial
      measureCurrent();                          // measure the present current
    }
    else {                                      //but if the measured current is over the cap
      Serial.println(current);                  // tell the problematic current over serial
      halt();                                    // the machine stops
      countJams();                                  // count how often this happens within an unacceaptable timeframe
      delay(1000);                               // wait a bit to make the shredder halt
      reverse();                                 //then it reverses
      delay(2000);                                // for two seconds
      halt();                                     //then stops
      delay(1000);                                //for a second to let the motor come to a halt.
      current = 0; //reset current
    }
  }
  else {
    halt();                                         //stop
    alarm();                                        //and sound the alarm
    current = analogRead(measurePin);           //it measures a new current (so it can move forward again)
  }
}
void shred() {
  lcd.setCursor(0, 0);
  lcd.print("shredding       ");
  Serial.println("I am shredding now");   //tell the world you are shredding
  digitalWrite(motionPin, LOW);                          // set stuff in motion
  digitalWrite(directionPin, HIGH);                        // in the forward direction
}
void reverse() {
  lcd.setCursor(0, 0);
  lcd.print("reversing      ");
  Serial.println("I am going back");   //tell the world you are shredding
  digitalWrite(motionPin, LOW);                        // Set stuff in motion
  digitalWrite(directionPin, LOW);                          // in the reverse direction
}
void halt() {
  lcd.setCursor(0, 0);
  lcd.print("doing nothing");
  Serial.println("STOP");   //tell the world you are coming to a halt
  digitalWrite(motionPin, HIGH);                        // Switch it all off
  digitalWrite(directionPin, LOW);
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
      Serial.print("I jammed ");
      Serial.print(jammed);
      Serial.println(" times.");
    }
    else {                        //If it has been ages since the last jam
      jammed = 1;                 //start counting again (while shredding, the jammed counter supposedly is 1)
    }
  }
  lcd.setCursor(0, 0);
  lcd.print("I jammed ");
  lcd.print(jammed);
  lcd.print(" times now");
}
void alarm() {
  Serial.println("I am jammed");   //tell the world something is wrong
  Serial.print("I jammed ");
  Serial.print(jammed);
  Serial.println(" times.");
  while (jammed >= maxJams) {
    Serial.print("I jammed ");
    Serial.print(jammed);
    Serial.println(" times.");
  }
}
void checkDirection() {
  int shredState = digitalRead(shredButton);              //check which button is pressed
  int reverseState =  digitalRead(reverseButton);
  if (reverseState == HIGH) {                             //if you are set to reverse
    reverse();                                            //turn back
    Serial.println("direction = reverse");
  }
  else if (shredState == HIGH) {                          //if you are set to shred
    shred();                                              //shred
    Serial.println("direction = forward");
  }
  else {                                                  //if you are set to stop
    halt();                                               //don't move
    Serial.println("direction = stop");
  }
}
void measureCurrent() {
  delay (333);                                //wait a third of a second to prevent freaking out over a initation spike
  current = analogRead(measurePin);         //check the current
}
