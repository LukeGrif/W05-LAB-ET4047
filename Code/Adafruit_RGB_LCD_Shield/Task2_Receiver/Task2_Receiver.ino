// =====================================================================
//  Task 2: Board B (receiver)
//  Display: Adafruit RGB LCD shield
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
//    LCD:    plug the shield onto the breakout board. No wires needed:
//            it uses the I2C pins SDA (A4) and SCL (A5) through the header.
//
//  Library (Tools -> Manage Libraries...):
//    "Adafruit RGB LCD Shield Library" by Adafruit
// =====================================================================

#include <SoftwareSerial.h>    // lets us make a serial port on any two pins
#include <Wire.h>                  // I2C bus: the shield is controlled over SDA and SCL
#include <Adafruit_RGBLCDShield.h> // driver for the Adafruit RGB LCD shield

SoftwareSerial link(10, 11);   // (RX pin = D10, TX pin = D11)

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

    lcd.begin(16, 2);                  // start the LCD: 16 columns, 2 rows
    lcd.setBacklight(WHITE);           // switch the backlight on (white)
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
