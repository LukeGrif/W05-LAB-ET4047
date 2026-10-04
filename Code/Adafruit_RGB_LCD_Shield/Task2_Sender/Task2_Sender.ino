// =====================================================================
//  Task 2: Board A (sender)
//  No LCD is needed on this board.
// ---------------------------------------------------------------------
//  What this sketch does:
//    Once every second it sends a numbered message to Board B,
//    for example "Hello 12", followed by a newline '\n'.
//    The newline tells Board B where the message ends.
//
//  Wiring (the TX/RX wires cross over):
//    A D11 (TX) -> B D10 (RX)    A's data travels to B
//    A D10 (RX) <- B D11 (TX)    return path (used in the extension)
//    A GND      -> B GND         common ground: essential!
// =====================================================================

#include <SoftwareSerial.h>    // lets us make a serial port on any two pins

// A second serial port on D10 (RX) and D11 (TX).
// It works just like Serial: begin(), print(), available(), read().
SoftwareSerial link(10, 11);   // (RX pin, TX pin)

// The messages we send, round and round
const char* const messages[] = {"Hello", "Test", "Loop"};
const uint8_t NUM_MESSAGES = 3;

const unsigned long SEND_INTERVAL = 1000;   // 1000 ms = 1 second

unsigned long lastSend = 0;    // time (ms) of the last send
uint8_t msgIndex = 0;          // which message to send next
unsigned int counter = 0;      // number added to each message


void setup()
{
    link.begin(9600);          // the link to Board B, 9600 baud
    Serial.begin(9600);        // USB Serial Monitor, for checking
    Serial.println(F("Sender ready"));
}


void loop()
{
    // Is it time to send the next message? (no delay() used)
    if (millis() - lastSend >= SEND_INTERVAL)
    {
        lastSend += SEND_INTERVAL;         // next send exactly 1 s later

        // ----- Send the frame to Board B, e.g. "Hello 12\n" -----
        link.print(messages[msgIndex]);    // the text
        link.print(' ');                   // a space
        link.print(counter);               // the counter
        link.print('\n');                  // newline = end of message

        // ----- Show the same thing on the Serial Monitor -----
        Serial.print(F("Sent: "));
        Serial.print(messages[msgIndex]);
        Serial.print(' ');
        Serial.println(counter);

        // ----- Get ready for the next message -----
        counter++;
        msgIndex++;
        if (msgIndex == NUM_MESSAGES)      // after the last message...
        {
            msgIndex = 0;                  // ...go back to the first
        }
    }
}
