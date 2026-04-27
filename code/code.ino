#include <LiquidCrystal.h>

// -----------------------------
// LCD pin configuration
// -----------------------------
const int rs = 11;  // Register Select pin
const int en = 12;  // Enable pin
const int d4 = 2;   // Data pin 4
const int d5 = 3;   // Data pin 5
const int d6 = 4;   // Data pin 6
const int d7 = 5;   // Data pin 7

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// -----------------------------
// Timing variables for both cars
// -----------------------------
unsigned long lapStartTime1 = 0;
unsigned long lapStartTime2 = 0;

unsigned long lapTime1 = 0;
unsigned long lapTime2 = 0;

// Stores the best lap time recorded so far
unsigned long bestTime = 999999999;

// Flags to ignore the very first lap (invalid timing)
bool first1 = true;
bool first2 = true;

// -----------------------------
// Custom LCD character (crown)
// Indicates the current fastest car
// -----------------------------
byte crown[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b10101,
  0b11111,
  0b11111,
  0b00000,
  0b00000
};

void setup() {
  Serial.begin(19200);   // Communication with Carrera Control Unit
  lcd.begin(16, 2);      // Initialize 16x2 LCD

  lcd.print("Starting...");
  lcd.createChar(0, crown);  // Store custom crown character

  delay(1000);
  lcd.clear();

  // -----------------------------
  // Static LCD layout
  // -----------------------------
  lcd.setCursor(0, 0);
  lcd.print("Car1:");

  lcd.setCursor(0, 1);
  lcd.print("Car2:");

  // Placeholder for lap times
  lcd.setCursor(6, 0);
  lcd.print("--.--");

  lcd.setCursor(6, 1);
  lcd.print("--.--");

  // Best lap display
  lcd.setCursor(12, 0);
  lcd.print("BEST");

  lcd.setCursor(12, 1);
  lcd.print("-.--");
}

void loop() {

  // Request data from the Control Unit
  Serial.print("\"?");

  // Check if data is available
  if (Serial.available()) {

    // Read incoming data until '$' delimiter
    String incomingData = Serial.readStringUntil('$');

    // Ensure the received data is long enough
    if (incomingData.length() > 10) {

      // Check if sensor group 1 was triggered
      if (incomingData.charAt(10) == '1') {

        // -----------------------------
        // Car 1 detected
        // -----------------------------
        if (incomingData.charAt(1) == '1') {

          lapTime1 = millis() - lapStartTime1;
          lapStartTime1 = millis();

          // Update display for Car 1
          lcd.setCursor(6, 0);
          lcd.print("     ");  // Clear previous value
          lcd.setCursor(6, 0);
          lcd.print(lapTime1 / 1000.0);

          besttime();
          first1 = false;
        }

        // -----------------------------
        // Car 2 detected
        // -----------------------------
        if (incomingData.charAt(1) == '2') {

          lapTime2 = millis() - lapStartTime2;
          lapStartTime2 = millis();

          lcd.setCursor(6, 1);
          lcd.print("     ");
          lcd.setCursor(6, 1);
          lcd.print(lapTime2 / 1000.0);

          besttime();
          first2 = false;
        }
      }
    }
  }
}

// -----------------------------
// Updates the best lap time
// and moves the crown indicator
// -----------------------------
void besttime() {

  // New best lap from Car 1
  if (lapTime1 < bestTime && !first1) {

    bestTime = lapTime1;

    // Move crown to Car 1
    lcd.setCursor(5, 1);
    lcd.print(" ");           // Remove previous crown
    lcd.setCursor(5, 0);
    lcd.write(byte(0));       // Draw crown

    // Update best time display
    lcd.setCursor(12, 1);
    lcd.print("    ");
    lcd.setCursor(12, 1);
    lcd.print(bestTime / 1000.0);
  }

  // New best lap from Car 2
  if (lapTime2 < bestTime && !first2) {

    bestTime = lapTime2;

    lcd.setCursor(5, 0);
    lcd.print(" ");
    lcd.setCursor(5, 1);
    lcd.write(byte(0));

    lcd.setCursor(12, 1);
    lcd.print("    ");
    lcd.setCursor(12, 1);
    lcd.print(bestTime / 1000.0);
  }
}