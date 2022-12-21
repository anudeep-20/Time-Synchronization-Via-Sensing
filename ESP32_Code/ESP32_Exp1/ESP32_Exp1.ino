#define PWMpin 12 // PWM pin to which Raspberry Pi is connected

// Function to be trigerred after a FALLING egde is detected
void IRAM_ATTR ISR() {
    Serial.println(esp_timer_get_time());
}

void setup() {
  Serial.begin(9600);
  
	pinMode(PWMpin, INPUT_PULLUP);
	attachInterrupt(PWMpin, ISR, FALLING);
}

void loop() {
  while(true);
}
