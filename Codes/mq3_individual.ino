//**This is the code for the MQ3 sensor when interfaced with arduino and the readings are shown in the LCD display connected with I2C Converter**//
//** Maniram Tatipamula**//

#include <Wire.h> //library that help to communicate with I2C
#include <LiquidCrystal_I2C.h> 
LiquidCrystal_I2C lcd(0x27, 16, 2);
int val = 0;
const int buzzerPin = 12;
void setup()
{
  pinMode(A0, INPUT); //declare A0 as input pin
  lcd.init(); // initialize the LCD
  lcd.backlight(); //turn on the backlight
  lcd.clear(); //clear the LCD display
  Serial.begin(9600); //start serial monitor
  pinMode(buzzerPin, OUTPUT);
}

void loop()
{
   val = analogRead(A0); //read the analog input from pin A0


 if ( val > 400 ) {
 tone(3,1000);      //activate buzzer at pin 3
      lcd.setCursor(0,0);  //set cursor LCD
      lcd.print("High Alcohol");
      lcd.setCursor(0,1);
      lcd.print("Content Detected");
      alcBuzz();
      //**This code belongs to Maniram Tatipamula**//
 }

   else{
    noTone(3);
     lcd.setCursor(0,0); //Set the cursor position
     lcd.print("MQ3 Value: "); // Start display on LCD
     lcd.setCursor(0,1);
      lcd.print(val);
      delay(100);
      lcd.clear(); //clear LCD screen 
      Serial.println(val); //print val on serial monitor
   }
}

void alcBuzz(){
  digitalWrite(buzzerPin, HIGH);
  delay(100);  // Buzzer sounds for 1 second
  digitalWrite(buzzerPin, LOW);
  digitalWrite(buzzerPin, HIGH);
  delay(200);  // Buzzer sounds for 1 second
  digitalWrite(buzzerPin, LOW);
  }
