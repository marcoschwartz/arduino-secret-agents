// Required libraries
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define NUMREADINGS 15

int senseLimit = 15; // raise this number to decrease sensitivity (up to 1023 max)
int probePin = 0; // analog 0
int val = 0; // reading from probePin

// variables for smoothing

int readings[NUMREADINGS];                // the readings from the analog input
int index = 0;                            // the index of the current reading
int total = 0;                            // the running total
int average = 0;                          // final average of the probe reading

int updateTime = 40;

// Create LCD instance
LiquidCrystal_I2C lcd(0x27,20,4);

void setup()
{
  // Initialise LCD
  lcd.init();

  Serial.begin(9600);
  
  // Print a message to the LCD
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("EMF Detector Started");
  delay(1000);
  lcd.clear();
}


void loop()
{
  val = analogRead(probePin);  // take a reading from the probe
  Serial.println(val);
  
  if(val >= 1){    
  val = constrain(val, 1, senseLimit);  // turn any reading higher than the senseLimit value into the senseLimit value
    val = map(val, 1, senseLimit, 1, 1023);  // remap the constrained value within a 1 to 1023 range

    total -= readings[index];               // subtract the last reading
    readings[index] = val; // read from the sensor
    total += readings[index];               // add the reading to the total
    index = (index + 1);                    // advance to the next index

    if (index >= NUMREADINGS)               // if we're at the end of the array...
      index = 0;                            // ...wrap around to the beginning

    average = total / NUMREADINGS;          // calculate the average

    lcd.setCursor(0,1);
    lcd.print("   ");
    
    lcd.setCursor(0,0);
    lcd.print("EMF level: ");
    lcd.setCursor(0,1);
    lcd.print(average);

    delay(updateTime);
  }
}
