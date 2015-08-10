// Microphone test

void setup() {
  
  // Start Serial
  Serial.begin(115200);
}

void loop() {
  
  // Read the input on analog pin 5:
  int sensorValue = analogRead(A5);
  
  // Print out the value you read:
  Serial.println(sensorValue);
  delay(1);        // delay in between reads for stability
}
