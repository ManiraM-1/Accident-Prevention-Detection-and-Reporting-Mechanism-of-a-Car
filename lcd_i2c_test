#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  
}


void loop()
{
  lcd.print("Hello, world!");
  delay(1000);
lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("This is");
  lcd.setCursor(3,1);
  lcd.print("Maniram");
  lcd.setCursor(3,2);
  delay(2000);
lcd.clear();
  lcd.print("Good Night");
  delay(1000);
lcd.clear();  

}
