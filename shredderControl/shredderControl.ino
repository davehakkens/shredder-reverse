
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

#define SHRED_ST  0
#define SHRED_FW  1
#define SHRED_REV 2

//#define DEBUG 1

// constants won't change. They're used here to set pin numbers:
const int shredButton = 7;     // the number of the pin that registers if you want the machine to shred
const int reverseButton = 6;    // the number of the pin that registers if you want the machine to reverse
const int motionPin = 4;     // the number of the pin that decides if the motor turns
const int directionPin = 3;     // the number of the pin that decides the driection of the motor
const int measurePin = A0;   // this pin has a hall sensor connected to it that measures the output current to the motor

int startSpan = 400;    // Time to ignore current spikes due to motor start //TODO: allow GUI configuration
int maxJams = 3;        //this int sets the max amount of jams in a set time
int minJamTime = 40000; //this int sets that time in milliseconds (forty seconds now)
int currentCap = 650;   // this is the value over which the hall sensor signal will register as a jam.
int minCurrent = 0; //this is the current value that the machine uses to recognise when it is spinning, but not shredding.

unsigned long jamTime=0;       //this int will store time between problematic jams
int current;                   // this in will store the measured current
int jammedCounter = 0;         // this int will store the amount of Jams within the minJamTime
unsigned long startTime;       // this int is needed to count the interval between jams.
int i;

boolean working = true;
boolean alarmed = false;
unsigned long lastStart = 0;
int shredDir = SHRED_ST;

LiquidCrystal_I2C lcd(0x3F, 16, 2);           //set the address and dimensions of the LCD. Here the address is 0x3F, but it depends on the chip. You can use and i2c-scanner to determine the address of your chip.

byte pBar[8] = {0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f};

///So here comes what the machine will do on startup
void setup() {
  // initialize the output pins:
  pinMode(motionPin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  // initialize input pins:
  pinMode(shredButton, INPUT);
  pinMode(reverseButton, INPUT);
  pinMode(measurePin, INPUT);
  
  Serial.begin(9600);  // use serial, just to check what the machine is doing
  //initialise the lcd
  lcd.init();
  lcd.createChar(0, pBar);
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 1);
  lcd.print("Shredder Pro  ");
//TODO: use for to make banner 
  lcd.setCursor(0, 0);
  lcd.print("Precious Plastic");
  delay(700);
  lcd.setCursor(0, 0);
  lcd.print("recious Plastic ");
  delay(350);
  lcd.setCursor(0, 0);
  lcd.print("ecious Plastic v");
  delay(350);
  lcd.setCursor(0, 0);
  lcd.print("cious Plastic v4");
  delay(2100);
  lcd.setCursor(0, 1);
  lcd.print("             ");  
}

// here comes what the machine will do while running, which is basically shred, reverse or do nothing
void loop() {
  if(alarmed)return;
  current = analogRead(measurePin);
  checkDirection();
  if (working && millis()>=lastStart+startSpan && current > currentCap){
    halt();                                    // the machine stops
    countJams();                               // count how often this happens within an unacceaptable timeframe
    if (jammedCounter >= maxJams) {
      alarm();                                 // If it has jammed too much in timeframe stop everything
    }else{
      //TODO: change reverse to state machine instead of delays
      //TODO: what to do on reversed jam ???
      delay(2000);                               // wait a bit to make the shredder halt
      reverse();                                 // then it reverses
      delay(2000);                               // for two seconds
      halt();                                    // then stops
      delay(3000);                               // for a second to let the motor come to a halt.
      shred();
    }
  }
  Serial.println(current);                     // You can read the current over serial
  printBar();          //displays current as a progress bar.
  //printValue();      //you can also display the measured value.
  //printCurrent();    //or display this as value in amps.
}

void shred() {
  lcd.setCursor(0, 0);
  lcd.print("Shredding       ");
#ifdef DEBUG
  Serial.println("Shredding");
#endif
  digitalWrite(motionPin, LOW);                          // set stuff in motion
  digitalWrite(directionPin, LOW);                        // in the forward direction
}

void reverse() {
  lcd.setCursor(0, 0);
  lcd.print("Reversing       ");
#ifdef DEBUG
  Serial.println("I am going back");
#endif
  digitalWrite(motionPin, LOW);                        // Set stuff in motion
  digitalWrite(directionPin, HIGH);                          // in the reverse direction
}

void halt() {
  lcd.setCursor(0, 0);
  lcd.print("PPv4    Shredder");
#ifdef DEBUG
  Serial.println("STOP");   //tell the world you are coming to a halt
#endif
  digitalWrite(motionPin, HIGH);                        // Switch it all off
}

void countJams() {
#ifdef DEBUG
  Serial.print(startTime);
  Serial.print(" vs ");
  Serial.println(millis());
#endif
  if(startTime==0){
    jamTime = minJamTime;                 //If it is the first time dont mind elapsed time betwen jams
  }else{
    jamTime = millis() - startTime;       //check how much time is between jams and store this in jamTime
  }
  if (jamTime < (minJamTime)) {        //if that is inside unacceptable limits
    jammedCounter++;                         //count this as a jam
    startTime = millis();       //reset the start time
#ifdef DEBUG
    Serial.print("I jammed ");
    Serial.print(jammed);
    Serial.println(" times.");
#endif
  }else {                        //If it has been ages since the last jam
    startTime = millis();
    jammedCounter = 1;                 //start counting again
  }
  lcd.setCursor(0, 0);
  lcd.print("I jammed ");
  lcd.print(jammedCounter);
  lcd.print(" times now  ");
}


void alarm() {
#ifdef DEBUG
  Serial.println("I am jammed");
#endif
  alarmed = true;
  lcd.setCursor(0, 0);
  lcd.print("Stopped after ");
  lcd.print(jammedCounter);
  lcd.print(" jams ");
}

void checkDirection() {
  /*
   *  Code assumes a 3 states button with two pins (active high)
   *  1. Shred
   *  2. Reverse
   *  If none is pushed we assume it is off
   *  It's not possible to push both at the same time (in that case we assume shred forward)
   */ 
  int shredInput = digitalRead(shredButton);              //check which button is pressed
  int reverseInput =  digitalRead(reverseButton);
  if (shredInput == LOW && reverseInput==LOW){          //if you are set to stop
    if(working){
      halt();                                               //don't move
      working = false;
      shredDir = SHRED_ST;
    }
  }else{
    if(!working){
      working=true;
      lastStart=millis();
    }
    //TODO: check rotation shift and wait before changing
    if (shredInput == HIGH){                          //if you are set to shred
      if(shredDir!=SHRED_FW){
        shred();                                           //shred
        shredDir = SHRED_FW;
      }
    }else{                                               //if you are set to reverse
      if(shredDir!=SHRED_REV){
        reverse();                                         //turn back
        shredDir = SHRED_REV;
      }
    }
  }
}

void printBar() {       //displays the drawn current as a bar
  int pBari = map(current, 500, currentCap, 0, 17);  // turn the current current value into a percentage of the currentcap, considering the scale starts a 500.
  for (i = 0; i < pBari; i++) {
    lcd.setCursor(i, 1);
    lcd.write(byte(0));
  }
}

void printValue() {           //displays the drawn current as a value
  lcd.setCursor(0, 1);
  if(current<1000)lcd.print(" ");
  lcd.print(current);
  lcd.print(" out of ");
  lcd.print(currentCap);
  lcd.print(" ");
}

void printCurrent() {
  lcd.setCursor(0, 1);
  lcd.print((0, 1671 * current) - 90, 622);
  lcd.print(" amps     ");
}
