#define eyeBlinkPin 7   // Pin where the eye blink sensor is connected
#define buzzerPin 8     // Pin for the buzzer

bool eyesClosed = false;
unsigned long eyesClosedStartTime = 0;
unsigned long eyesClosedDuration = 0;

void setup() {
  pinMode(eyeBlinkPin, INPUT);  // Set the eye blink sensor pin as input
  pinMode(buzzerPin, OUTPUT);  // Set the buzzer pin as output
  // Other setup code for motors and Bluetooth
  Serial.begin(9600);
}

void loop() {
  int eyeBlinkState = digitalRead(eyeBlinkPin);
  
  if (eyeBlinkState == HIGH) {
    // Eyes are open
    eyesClosed = false;
  } else {
    // Eyes are closed
    if (!eyesClosed) {
      eyesClosedStartTime = millis();  // Record the time when the eyes first close
      eyesClosed = true;
    }
    eyesClosedDuration = millis() - eyesClosedStartTime;

    if (eyesClosedDuration > 4000) {
      // Eyes have been closed for more than 4 seconds
      stopCar();  // Implement your car stopping function
      activateBuzzer();
    }
  }

  // Other code for handling Bluetooth commands and motor control
}

void stopCar() {
  // Implement your car stopping function
}

void activateBuzzer() {
  digitalWrite(buzzerPin, HIGH);  // Turn on the buzzer
  delay(1000);  // Buzzer sounds for 1 second
  digitalWrite(buzzerPin, LOW);   // Turn off the buzzer
}
