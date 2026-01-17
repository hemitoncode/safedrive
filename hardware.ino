#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

const byte ROWS = 4;
const byte COLS = 3;

const char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

const int led = 13;
const int fan = 10;
const int buzzer = 12;

const byte rowPins[ROWS] = { 9, 8, 7, 6 };
const byte colPins[COLS] = { 5, 4, 3 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 20, 4);

String inputCode = "";
int randomCode = 0;

// System state
bool alarmActive = false;


// ---------- LCD helpers ----------
void clearLine(byte row) {
  lcd.setCursor(0, row);
  lcd.print("                    ");  // 20 spaces
}

void printLine(byte row, const char* msg) {
  clearLine(row);
  lcd.setCursor(0, row);
  lcd.print(msg);
}

void showEnteredCode() {
  clearLine(3);
  lcd.setCursor(0, 3);
  if (inputCode.length() == 0) lcd.print("0000");
  else lcd.print(inputCode);
}

void showRandomCode() {
  clearLine(1);
  lcd.setCursor(0, 1);
  lcd.print("Code: ");
  lcd.print(randomCode);
}

void showPrompt() {
  printLine(2, "Enter code:");
}

void flashMessageRow2(const char* msg, unsigned long ms) {
  printLine(2, msg);
  delay(ms);
  showPrompt();  // restore UI
}

// ---------- Code generation ----------
void generateNewCode() {
  randomCode = random(1000, 10000);
  showRandomCode();
}

void alarmOn() {
  alarmActive = true;
  analogWrite(fan, 255);
  digitalWrite(led, HIGH);
  tone(buzzer, 1000);
  flashMessageRow2("Alarm!", 1000);
}

void alarmOff() {
  alarmActive = false;
  analogWrite(fan, 0);
  digitalWrite(led, LOW);
  noTone(buzzer);
}

// ---------- Input handling ----------
void resetInput() {
  inputCode = "";
  showEnteredCode();
}

void handleSubmit() {
  if (!alarmActive) {
    flashMessageRow2("System not active", 500);
    resetInput();
    return;
  }

  if (inputCode.length() != 4) {
    flashMessageRow2("Enter 4 digits", 500);
    resetInput();
    return;
  }

  if (inputCode.toInt() == randomCode) {
    alarmOff();
    flashMessageRow2("Unlocked", 500);
    generateNewCode();
  } else {
    flashMessageRow2("Wrong code", 500);
  }

  resetInput();
}

void handleDigit(char key) {
  if (inputCode.length() < 4) {
    inputCode += key;
    showEnteredCode();
  }
}

// ---------- Serial handling ----------
void handleSerial() {
  while (Serial.available() > 0) {
    char data = Serial.read();

    if (data == '1') {
      if (!alarmActive) alarmOn();
    }
  }
}

void handleKeypad() {
  char key = keypad.getKey();
  if (!key) return;

  if (key == '*') {
    resetInput();
  }

  else if (key == '#') {
    handleSubmit();
  }

  else {
    handleDigit(key);
  }
}

// ---------- Setup / loop ----------
void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(led, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);

  generateNewCode();
  showPrompt();
  showEnteredCode();
}

void loop() {
  handleSerial();
  handleKeypad();
}
