/*
  LED Gumball Machine - Press a button, get tasty LEDs
  Nathan Seidle
  SparkFun Electronics
  March 6th, 2019

  We use a continuous servo in a gumball machine to vend LEDs on demand.

  Connections:
  5: Momentary button
  6: LED inside button (PWM)
  9: PWM line to continuous servo
*/

#include <Servo.h> //Needed for Servo

Servo gumballServo;

const int GUMBALL_SERVO = 9;
const int BUTTON = 5;
const int BUTTON_LED = 6;

int brightness = 0;
int fadeAmount = 5;

unsigned long minTimeBetweenPresses = 15 * 1000; //Make users wait this amount of ms between dispenses
bool firstDispense = false; //Goes true once button is first pressed. Avoids first minTime timeout.
unsigned long lastDispenseTime = 0; //Tracks last millis() we dispensed LEDs

void setup() {
  //In case of brown out reset, the servo may still be moving
  //Tell servo to stop and then shut down PWM
  gumballServo.attach(GUMBALL_SERVO); //Be sure the servo won't move
  gumballServo.write(95); //Stop movement on continous servo
  gumballServo.detach(); //Be sure the servo won't move

  Serial.begin(9600);
  Serial.println("Great LED dispensor 3000 begins!");

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUTTON_LED, OUTPUT);
}

void loop() {
  fadeUp();
  fadeDown();

  if (buttonPressed() == true)
  {
    analogWrite(BUTTON_LED, 255); //Turn on button LED

    //Dispense LEDs
    dispense();

    while (buttonPressed() == true) delay(10);

    analogWrite(BUTTON_LED, 0); //Turn off button LED
  }
}

void fadeUp()
{
  if (buttonPressed() == true) return;
  while (brightness <= 255)
  {
    analogWrite(BUTTON_LED, brightness);

    brightness += fadeAmount;

    if (buttonPressed() == true) return;
    delay(30);
  }
  brightness = 255;
}

void fadeDown()
{
  if (buttonPressed() == true) return;
  while (brightness > 0)
  {
    analogWrite(BUTTON_LED, brightness);

    brightness -= fadeAmount;

    if (buttonPressed() == true) return;
    delay(30);
  }
  brightness = 0;
}

bool buttonPressed()
{
  if (digitalRead(BUTTON) == LOW)  return true;
  return false;
}

void dispense()
{
  if (firstDispense == false)
  {
    Serial.println("First dispense");
    firstDispense = true;
  }

  //Check if we are allowed to dispense
  else if (millis() - lastDispenseTime < minTimeBetweenPresses)
  {
    Serial.println("Uh-huh Mr. Eager. Wait your turn.");
    while (digitalRead(BUTTON) == LOW) delay(10);
    return; //No soup for you!
  }

  Serial.println("Dispensing yummy LEDs");

  gumballServo.attach(GUMBALL_SERVO); //Control gumball machine from pin 9

  gumballServo.write(200); //Run CCW at fast speed
  delay(1000);
  gumballServo.write(95); //Stop movement on continous servo
  delay(1000);
  gumballServo.detach(); //Be sure the servo won't move

  lastDispenseTime = millis();
}
