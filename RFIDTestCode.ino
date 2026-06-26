// Pin definitions
const int switchPin = A3;   // Switch input
const int buzzerPin = D6;   // Buzzer output

// Threshold for analog reading (for switch pressed)
const int switchThreshold = 500; // Adjust depending on your switch

void setup() {
  pinMode(switchPin, INPUT);   // Set switch as input
  pinMode(buzzerPin, OUTPUT);  // Set buzzer as output
  digitalWrite(buzzerPin, LOW); // Ensure buzzer is off initially
}

void loop() {
  int switchValue = analogRead(switchPin); // Read switch value

  if (switchValue > switchThreshold) {
    // Switch pressed
    digitalWrite(buzzerPin, HIGH); // Turn on buzzer
  } else {
    // Switch released
    digitalWrite(buzzerPin, LOW);  // Turn off buzzer
  }

  delay(50); // Small debounce delay
}