
#include <LiquidCrystal_I2C.h>


#include <Wire.h>


LiquidCrystal_I2C lcd(0x38, 20, 2);

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.setBacklight(HIGH);
  lcd.print("COWABUNGA!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
