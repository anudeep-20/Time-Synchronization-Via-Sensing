#define PWMpin 12 // PWM pin to which Raspberry Pi is connected

// Function to be trigerred on hardware interrupt
void IRAM_ATTR ISR() {
    Serial.println(esp_timer_get_time());
}

void setup() {
  Serial.begin(9600);
  
	pinMode(PWMpin, INPUT_PULLUP);
	attachInterrupt(PWMpin, ISR, FALLING); // Linking PWM falling edge and Interrupt function
}

void loop() {
  while(true);
}
