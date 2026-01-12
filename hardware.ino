#define led 2

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    char message = Serial.read();

    if (message == '1') {
      digitalWrite(led, HIGH);  // LED ON
    } 
    else if (message == '0') {
      digitalWrite(led, LOW);   // LED OFF
    }
  }
}
