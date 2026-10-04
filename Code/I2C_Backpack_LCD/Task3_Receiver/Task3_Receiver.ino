// =====================================================================
//  Task 3: Board B (receiver with a Caesar cipher)
//  Display: 16x2 LCD with I2C backpack
// ---------------------------------------------------------------------
//  What this sketch does:
//    Receives scrambled messages from Board A (one per line), then
//    decrypts them by moving every letter SHIFT places BACK.
//      LCD top row:     what arrived on the wire   e.g. "Khoor"
//      LCD bottom row:  the decrypted message      e.g. "Hello"
//
//  Wiring: the same as Task 2
//    A D11 (TX) -> B D10 (RX)
//    A D10 (RX) <- B D11 (TX)
//    A GND      -> B GND
//    LCD:    GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
//
//  Library (Tools -> Manage Libraries...):
//    "LiquidCrystal I2C" by Frank de Brabander
// =====================================================================

#include <SoftwareSerial.h>
#include <Wire.h>                  // I2C bus: the LCD is controlled over SDA and SCL
#include <LiquidCrystal_I2C.h>     // driver for the I2C backpack LCD

SoftwareSerial link(10, 11);   // (RX pin = D10, TX pin = D11)

// The LCD object: I2C address 0x20, 16 columns, 2 rows.
// The lab backpacks are at address 0x20. If your screen stays blank,
// your backpack may use another address: try 0x27 or 0x3F.
LiquidCrystal_I2C lcd(0x20, 16, 2);

// The secret key. It must be the SAME number as on Board A.
const int SHIFT = 3;

// ----- The line buffer (the same as Task 2) -----
const uint8_t MAX_LEN = 32;    // longest message we will store
char buffer[MAX_LEN + 1];      // +1 for the '\0' at the end
uint8_t len = 0;               // characters stored so far


// Move one letter 'shift' places along the alphabet (wrapping Z -> A).
// Anything that is not a letter is not changed.
// (This is exactly the same function as on Board A.)
char shiftChar(char c, int shift)
{
    if (c >= 'A' && c <= 'Z')                  // capital letter
    {
        return 'A' + (c - 'A' + shift) % 26;
    }
    if (c >= 'a' && c <= 'z')                  // small letter
    {
        return 'a' + (c - 'a' + shift) % 26;
    }
    return c;                                  // not a letter: leave it
}


// Decrypt one complete message and show it
void showMessage(const char* secret)
{
    // Decrypt into a second buffer, one character at a time.
    // Moving BACK 3 places is the same as moving FORWARD 23
    // (26 - 3), and moving forward is what shiftChar() does.
    char plain[MAX_LEN + 1];
    uint8_t i = 0;
    while (secret[i] != '\0')
    {
        plain[i] = shiftChar(secret[i], 26 - SHIFT);
        i++;
    }
    plain[i] = '\0';                   // end the decrypted string

    // Serial Monitor
    Serial.print(F("Received: "));
    Serial.print(secret);
    Serial.print(F("   Decrypted: "));
    Serial.println(plain);

    // LCD: scrambled text on top, real message underneath
    lcd.clear();
    lcd.print(secret);
    lcd.setCursor(0, 1);               // column 0, row 1 (bottom row)
    lcd.print(plain);
}


void setup()
{
    link.begin(9600);
    Serial.begin(9600);

    lcd.init();                        // start the LCD
    lcd.backlight();                   // switch the backlight on
    lcd.print(F("Waiting..."));
}


void loop()
{
    while (link.available() > 0)
    {
        char c = link.read();          // take one character

        if (c == '\r')                 // ignore carriage returns
        {
            continue;
        }

        if (c == '\n')                 // newline = the message is complete
        {
            buffer[len] = '\0';        // make it a proper C string
            showMessage(buffer);       // decrypt and show it
            len = 0;                   // ready for the next message
        }
        else if (len < MAX_LEN)        // room left: store it
        {
            buffer[len] = c;
            len++;
        }
    }
}
