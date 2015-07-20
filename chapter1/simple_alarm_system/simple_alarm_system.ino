// Code for the simple alarm system

// Pins
const int alarm_pin = 5;
const int led_pin = 6;
const int motion_pin = 7;
const int button_pin = 12;

// Alarm
boolean alarm_mode = false;

// Variables for the flashing LED
int ledState = LOW;
long previousMillis = 0; 
long interval = 100;  // Interval at which to blink (milliseconds)

void setup()
{
  // Set pins to output
  pinMode(led_pin,OUTPUT);
  pinMode(alarm_pin,OUTPUT);

  // Set button pin to input
  pinMode(button_pin, INPUT);
  
  // Wait before starting the alarm
  delay(5000);
}

void loop()
{
  // Motion detected ?
  if (digitalRead(motion_pin)) {
    alarm_mode = true; 
  }

  // If alarm mode is on, flash the LED and make the alarm ring
  if (alarm_mode){
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;   
      if (ledState == LOW)
        ledState = HIGH;
      else
        ledState = LOW;
    // Switch the LED
    digitalWrite(led_pin, ledState);
    }
    tone(alarm_pin,1000);
  }

  // If alarm is off
  if (alarm_mode == false) {
  
    // No tone & LED off
    noTone(alarm_pin);  
    digitalWrite(led_pin, LOW);
  }

  // If button is pressed, set alarm off
  int button_state = digitalRead(button_pin);
  if (button_state) {alarm_mode = false;}
}
