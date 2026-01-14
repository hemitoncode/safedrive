#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

char data = 0;

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

const int led = 2;
const int fan = 11;
const int buzzer = 12;

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 20, 4);

String inputCode = "";
int randomCode;

// ---------- LCD helpers ----------
void clearLine(byte row) {
  lcd.setCursor(0, row);
  lcd.print("                    "); // 20 spaces for 20x4 would be better; keep 20 here
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
  showPrompt();  // restore prompt after message
}

// ---------- Code generation ----------
void generateNewCode() {
  randomCode = random(1000, 10000); // 1000–9999
  showRandomCode();
}

// ---------- Alarm control ----------
void alarmOn() {
  digitalWrite(fan, HIGH);
  digitalWrite(led, HIGH);
  tone(buzzer, 1000);
  printLine(2, "ALERT!");
}

void alarmOff() {
  digitalWrite(fan, LOW);
  digitalWrite(led, LOW);
  noTone(buzzer);
}

// ---------- Input handling ----------
void resetInput() {
  inputCode = "";
  showEnteredCode();
}

void handleSubmit() {
  if (data == 0) {
    flashMessageRow2("System not active", 500);
    return;
  }
  else if (inputCode.toInt() == randomCode) {
    alarmOff();
    flashMessageRow2("Unlocked", 500);
  } 
  else {
    flashMessageRow2("Wrong code", 500);
  }

  resetInput();
  generateNewCode();
}

void handleDigit(char key) {
  if (inputCode.length() < 4) { // ensure it is only 4 digits
    inputCode += key;
    showEnteredCode();
  }
}

// ---------- Setup / loop ----------
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(led, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);

  randomSeed(analogRead(A0));

  generateNewCode();
  showPrompt();
  showEnteredCode();
}

void loop() {
  if (Serial.available() > 0) {
    data = Serial.read();
    if (data == '1') alarmOn();
  }

  char key = keypad.getKey();
  if (!key) return;

  if (key == '*') {
    resetInput();
    return;
  }

  if (key == '#') {
    handleSubmit();
    return;
  }

  if (key >= '0' && key <= '9') {
    handleDigit(key);
  }
}
