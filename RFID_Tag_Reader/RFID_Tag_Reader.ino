#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

// Pins for the RFID module
#define SS_PIN 10
#define RST_PIN 9

// MFRC522 library instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

// I2C address of the LCD (it may be 0x27 or 0x3F, check your display)
LiquidCrystal_I2C lcd(0x27, 16, 2); // For a 16x2 display

// Timeout before going back to the home screen (ms)
#define DISPLAY_TIMEOUT 5000
unsigned long lastReadMs = 0;
bool tagShown = false;

// Show the home screen
void showHomeScreen() {
  lcd.clear();
  lcd.print("RFID Reader");
  lcd.setCursor(0, 1);
  lcd.print("Present a tag...");
}

void setup() {
  Serial.begin(9600);
  SPI.begin();        // Start SPI communication
  mfrc522.PCD_Init(); // Initialise the MFRC522 module

  // Initialise the LCD
  lcd.init();
  lcd.backlight();
  showHomeScreen();
}

void loop() {
  // Go back to the home screen after the timeout
  if (tagShown && (millis() - lastReadMs >= DISPLAY_TIMEOUT)) {
    showHomeScreen();
    tagShown = false;
  }

  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("Card UID: ");
  String tagId = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    tagId += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println(tagId);

  // Show the UID on the LCD
  lcd.clear();
  lcd.print("Tag detected:");
  lcd.setCursor(0, 1);
  lcd.print(tagId);

  // Start the timeout that restores the home screen
  lastReadMs = millis();
  tagShown = true;

  // Pause reading for a moment
  delay(1000);

  // Stop communicating with the current card
  mfrc522.PICC_HaltA();

  // Stop encryption on the PCD
  mfrc522.PCD_StopCrypto1();
}
