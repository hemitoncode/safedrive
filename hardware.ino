#define led 2
#include <Keypad.h>

// Constants for row and column sizes
const byte ROWS = 4;
const byte COLS = 4;
 
// Array to represent keys on keypad
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Connections to Arduino
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keyPad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);

  digitalWrite(led, LOW); // Start by turning LED off
}

void loop() {
  if (Serial.available() > 0) {
    char message = Serial.read();

    if (message == '1') {
      digitalWrite(led, HIGH);  // LED ON
    } 
    else {
      char inputKey = keyPad.getKey();
      char inputCode;
      int actualCode = 1234;

      if (inputKey === "#" && inputCode == actualCode) {
        Serial.write(0);
      }
      if (inputKey) {
        inputCode += inputKey
      }

      digitalWrite(led, LOW);   // LED OFF
    }
  }
}
