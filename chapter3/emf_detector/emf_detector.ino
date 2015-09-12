// Required libraries
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Number of readings
#define NUMREADINGS 15

// Parameters for the EMF detector
int senseLimit = 15;
int probePin = 0;
int ledPin = 7;
int val = 0;
int threshold = 200;

// Averaging the measurements
int readings[NUMREADINGS];                
int index = 0;                            
int total = 0;                            
int average = 0;                          

// Time between readings
int updateTime = 40;

// Create LCD instance
LiquidCrystal_I2C lcd(0x27,20,4);

void setup()
{
  // Initialise LCD
  lcd.init();

  // Set LED as output
  pinMode(ledPin, OUTPUT);
  
  // Print a welcome message to the LCD
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("EMF Detector Started");
  delay(1000);
  lcd.clear();
}


void loop()
{
  // Read from the probe
  val = analogRead(probePin);

  // Check reading
  if(val >= 1){    

    // Constrain & map with sense limit value
    val = constrain(val, 1, senseLimit);
    val = map(val, 1, senseLimit, 1, 1023);

    // Averaging the reading
    total -= readings[index];              
    readings[index] = val; 
    total += readings[index];              
    index = (index + 1);                    

    if (index >= NUMREADINGS)              
      index = 0;                         

    average = total / NUMREADINGS; 

    // Print on LCD screen
    lcd.setCursor(0,1);
    lcd.print("   ");
    
    lcd.setCursor(0,0);
    lcd.print("EMF level: ");
    lcd.setCursor(0,1);
    lcd.print(average);

    // Light up LED if EMF activity detected
    if (average > threshold) {
      digitalWrite(ledPin, HIGH);  
    }
    else {
      digitalWrite(ledPin, LOW);  
    }

    // Wait until next reading
    delay(updateTime);
  }
}
