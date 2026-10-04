// =====================================================================
//  Task 1: UART loopback on a single board
//  Display: Adafruit RGB LCD shield
// ---------------------------------------------------------------------
//  What this sketch does:
//    - Once every second it sends a word out of the TX pin (D1).
//    - A jumper wire carries it straight back into the RX pin (D0).
//    - Every character that comes back in is shown on the LCD.
//
//  Wiring:
//    Jumper: D1 (TX) -> D0 (RX)
//    LCD:    plug the shield onto the breakout board. No wires needed:
//            it uses the I2C pins SDA (A4) and SCL (A5) through the header.
//
//  Library (Tools -> Manage Libraries...):
//    "Adafruit RGB LCD Shield Library" by Adafruit
// =====================================================================

#include <Wire.h>                  // I2C bus: the shield is controlled over SDA and SCL
#include <Adafruit_RGBLCDShield.h> // driver for the Adafruit RGB LCD shield

// ---------------------------------------------------------------------
// Which serial port is connected to pins D0 (RX) and D1 (TX)?
//   Nano Every:   Serial1. (Serial is the separate USB port.)
//   Classic Nano: Serial.  It is shared with USB, so REMOVE the
//                 jumper while uploading, then put it back.
// The #if below runs when you compile and picks the right one, so
// the rest of the sketch just uses the name LINK.
// ---------------------------------------------------------------------
#if defined(ARDUINO_AVR_NANO_EVERY) || defined(HAVE_HWSERIAL1)
  #define LINK Serial1      // Nano Every
#else
  #define LINK Serial       // classic Nano
#endif

// The LCD object. The shield is always at I2C address 0x20,
// so no address is needed.
Adafruit_RGBLCDShield lcd;

// Backlight colours for lcd.setBacklight()
#define RED    0x1
#define GREEN  0x2
#define YELLOW 0x3
#define BLUE   0x4
#define VIOLET 0x5
#define TEAL   0x6
#define WHITE  0x7

const uint8_t LCD_COLS = 16;           // characters per row
const uint8_t LCD_ROWS = 2;            // number of rows

// The words we send, one per second, round and round
const char* const messages[] = {"Hello", "Test", "Loop"};
const uint8_t NUM_MESSAGES = 3;

const unsigned long SEND_INTERVAL = 1000;   // 1000 ms = 1 second

unsigned long lastSend = 0;            // time (ms) of the last send
uint8_t msgIndex = 0;                  // which message to send next

uint8_t col = 0;                       // where the next character goes:
uint8_t row = 0;                       //   column 0-15, row 0-1


// Print one character on the LCD.
// After 16 characters it moves down to the next row.
// When both rows are full it clears the screen and starts again.
void lcdPutChar(char c)
{
    if (col == LCD_COLS)               // this row is full...
    {
        col = 0;                       // ...go back to the first column
        row++;                         // ...of the next row

        if (row == LCD_ROWS)           // both rows full?
        {
            row = 0;                   // start again at the top
            lcd.clear();
        }
        lcd.setCursor(col, row);       // move the LCD cursor there
    }

    lcd.write(c);                      // show the character
    col++;                             // next character goes one to the right
}


void setup()
{
    LINK.begin(9600);                  // start the UART at 9600 baud

    lcd.begin(16, 2);                  // start the LCD: 16 columns, 2 rows
    lcd.setBacklight(WHITE);           // switch the backlight on (white)
    lcd.print(F("UART Loopback")); // F() keeps the text in flash, saving RAM
    delay(1000);                       // show the title for 1 second
    lcd.clear();
}


void loop()
{
    // ----- 1. Send the next message once every second -----
    // millis() is the number of ms since the board started.
    // We do NOT use delay(), so loop() keeps running and never
    // misses a character arriving on RX.
    if (millis() - lastSend >= SEND_INTERVAL)
    {
        lastSend += SEND_INTERVAL;     // next send exactly 1 s later

        LINK.print(messages[msgIndex]);    // out of TX (D1)

        msgIndex++;                    // move to the next message...
        if (msgIndex == NUM_MESSAGES)  // ...and after the last one,
        {
            msgIndex = 0;              // go back to the first
        }
    }

    // ----- 2. Show every character that has come back in on RX -----
    // available() = how many characters are waiting to be read
    while (LINK.available() > 0)
    {
        char c = LINK.read();          // take one character

        if (c == '\r' || c == '\n')    // skip line endings
        {
            continue;
        }

        lcdPutChar(c);                 // put it on the LCD
    }
}
