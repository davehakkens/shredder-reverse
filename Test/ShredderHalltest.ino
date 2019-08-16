
int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue;

void setup() {
  // declare the ledPin as an OUTPUT:

  Serial.begin(9600);
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);

}
