#include <IRremote.h>
#include <AFMotor.h>

// Define IR receiver pin
const int IR_RECEIVER_PIN = 7;

// Create an instance of the IRremote library
IRrecv irrecv(IR_RECEIVER_PIN);

// Create instances for motor control
AF_DCMotor motor1(1); // Motor 1
AF_DCMotor motor2(2); // Motor 2
AF_DCMotor motor3(3); // Motor 3
AF_DCMotor motor4(4); // Motor 4

// Define IR remote button codes
#define FORWARD 0xF50A7F80
#define BACKWARD 0xE41B7F80
#define LEFT 0xE01F7F80
#define RIGHT 0xF30C7F80
#define STOP 0xF20D7F80

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the IR receiver
}

void loop() {
  if (irrecv.decode()) {
    unsigned long key = irrecv.decodedIRData.decodedRawData;

    // Map IR remote button codes to motor actions
    if (key == FORWARD) {
      moveForward();
    } else if (key == BACKWARD) {
      moveBackward();
    } else if (key == LEFT) {
      turnLeft();
    } else if (key == RIGHT) {
      turnRight();
    } else if (key == STOP) {
      stopMotors();
    }

    irrecv.resume(); // Receive the next value
  }
}

void moveForward() {
  motor1.setSpeed(255);
  motor2.setSpeed(255);
  motor3.setSpeed(255);
  motor4.setSpeed(255);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void moveBackward() {
  motor1.setSpeed(255);
  motor2.setSpeed(255);
  motor3.setSpeed(255);
  motor4.setSpeed(255);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

void turnLeft() {
  motor1.setSpeed(255);
  motor2.setSpeed(255);
  motor3.setSpeed(255);
  motor4.setSpeed(255);
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(FORWARD);
}

void turnRight() {
  motor1.setSpeed(255);
  motor2.setSpeed(255);
  motor3.setSpeed(255);
  motor4.setSpeed(255);
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(BACKWARD);
}

void stopMotors() {
  motor1.setSpeed(0);
  motor2.setSpeed(0);
  motor3.setSpeed(0);
  motor4.setSpeed(0);
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}
