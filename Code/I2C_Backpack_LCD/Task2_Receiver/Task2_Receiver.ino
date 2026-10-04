// =====================================================================
//  Task 2: Board B (receiver)
//  Display: 16x2 LCD with I2C backpack
// ---------------------------------------------------------------------
//  What this sketch does:
//    Characters from Board A arrive one at a time. They are saved in
//    a buffer (a "line buffer") until a newline '\n' arrives. Then
//    the whole message is shown on the LCD and the Serial Monitor.
//
//  Wiring:
//    A D11 (TX) -> B D10 (RX)
//    A D10 (RX) <- B D11 (TX)
//    A GND      -> B GND
//    LCD:    GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
//
//  Library (Tools -> Manage Libraries...):
//    "LiquidCrystal I2C" by Frank de Brabander
// =====================================================================

#include <SoftwareSerial.h>    // lets us make a serial port on any two pins
#include <Wire.h>                  // I2C bus: the LCD is controlled over SDA and SCL
#include <LiquidCrystal_I2C.h>     // driver for the I2C backpack LCD

SoftwareSerial link(10, 11);   // (RX pin = D10, TX pin = D11)

// The LCD object: I2C address 0x20, 16 columns, 2 rows.
// The lab backpacks are at address 0x20. If your screen stays blank,
// your backpack may use another address: try 0x27 or 0x3F.
LiquidCrystal_I2C lcd(0x20, 16, 2);

// ----- The line buffer -----
const uint8_t MAX_LEN = 32;    // longest message we will store
char buffer[MAX_LEN + 1];      // +1 to leave room for the '\0' at the end
uint8_t len = 0;               // how many characters are stored so far


// Show one complete message on the Serial Monitor and the LCD
void showMessage(const char* msg)
{
    Serial.print(F("Received: "));
    Serial.println(msg);

    lcd.clear();                       // wipe the screen
    lcd.print(F("Received:"));         // top row
    lcd.setCursor(0, 1);               // column 0, row 1 (bottom row)
    lcd.print(msg);                    // the message
}


void setup()
{
    link.begin(9600);          // the link from Board A, 9600 baud
    Serial.begin(9600);        // USB Serial Monitor

    lcd.init();                        // start the LCD
    lcd.backlight();                   // switch the backlight on
    lcd.print(F("Waiting..."));
}


void loop()
{
    // Deal with every character that is waiting
    while (link.available() > 0)
    {
        char c = link.read();          // take one character

        if (c == '\r')                 // ignore carriage returns
        {
            continue;
        }

        if (c == '\n')                 // newline = the message is complete
        {
            buffer[len] = '\0';        // add the '\0' so it is a proper C string
            showMessage(buffer);       // show it
            len = 0;                   // empty the buffer for the next message
        }
        else if (len < MAX_LEN)        // is there still room in the buffer?
        {
            buffer[len] = c;           // store the character
            len++;                     // and move along one place
        }
        // If the buffer is full, extra characters are simply dropped.
        // Writing past the end of the array would corrupt other variables.
    }
}
