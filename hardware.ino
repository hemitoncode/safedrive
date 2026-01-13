#include <Keypad.h>


const byte ROWS = 4;
const byte COLS = 3;

// Keypad setup
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

const int led = 2;
const int buzzer = 12; 

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Persistent input code as Arduino String
String inputCode = "";

String validChars = "123456890";

void setup() {
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Only read serial if available
  if (Serial.available() > 0) {
    char data = Serial.read();
    if (data == '1') {
      digitalWrite(led, HIGH);
      tone(buzzer, 1000);
    }
  }

  char key = keypad.getKey();

  if (key == '#' && inputCode == "1234") {
      digitalWrite(led, LOW);
      noTone(buzzer);
      inputCode = "";  // always reset after submit
  } 
  else if (key == '*') {
    inputCode = "";
  } 
  else if (key) {
    inputCode += key;  // append number keys only
  }
}
