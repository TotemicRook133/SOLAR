// https://www.tinkercad.com/things/1TiQPmiXWNR-frantic-migelo
int socPin = A0; // SOC input from potentiometer
int motorPin1 = 9; // L293D Input 1
int motorPin2 = 10; // L293D Input 2
int solarPin = A1; // Solar panel brightness (simulated by potentiometer)
int ledPin = 13; // LED to simulate generator status
int enA = 8;

void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(solarPin, INPUT);
  pinMode(enA, OUTPUT);
  
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  // Start serial communication to print the solar output
  Serial.begin(9600);
}

void loop() {
  int socValue = analogRead(socPin); // Read battery SOC from potentiometer
  int solarBrightness = analogRead(solarPin); // Read solar panel brightness
  float correctedSolar = 0.0;
  if (solarBrightness <=884){
  	correctedSolar = (solarBrightness/884.0)*25.0;
  }
  else if (solarBrightness <=963){
  	correctedSolar = 25.0 + ((solarBrightness-884.0)/(963-884.0))*25.0;
  }
  else if (solarBrightness <=998){
  	correctedSolar = 50.0 +((solarBrightness-963.0)/(998.0-963.0))*25.0;
  }
  else if (solarBrightness <=1023){
  	correctedSolar = 75.0 +((solarBrightness-998)/(1023-998.0))*25.0;
  }
  // Map SOC to percentage (0-100)
  int socPercent = map(socValue, 0, 1023, 0, 100);
  float solarPower = map(solarBrightness, 0, 1023, 0, 100);
  // Simulate lower SOC when solar brightness is low (nighttime)
  if (correctedSolar < 30) {
    socPercent -= 10; // Discharge battery faster at night
  }
  if (socPercent < 0){
    socPercent = 0;
  }
  // Calculate solar power produced (simulating in watts)
  // Map solar brightness (0-1023) to a range of solar power (e.g., 0-100W)

  
  // Print the solar power produced to the Serial Monitor
  Serial.print("Solar Power Produced: ");
  Serial.print(correctedSolar);
  Serial.print(" W");
  Serial.print("\n");
  Serial.print("Battery Percentage: ");
  Serial.print(socPercent);
  Serial.print("%");
   Serial.print("\n");
  
  // Turn on motor (generator) if SOC is low
  if (socPercent < 30) {
    analogWrite(enA, 255);
    digitalWrite(motorPin1, HIGH); // Turn motor on (diesel generator)
    digitalWrite(motorPin2, LOW); // Motor direction
    digitalWrite(ledPin, HIGH); // LED ON (generator running)
  } else {
    digitalWrite(motorPin1, LOW); // Turn motor off
    digitalWrite(motorPin2, LOW); // Motor OFF
    digitalWrite(ledPin, LOW); // LED OFF (generator off)
  }
  
  delay(1000); // Delay to stabilize readings
}
