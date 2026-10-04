// =====================================================================
//  Task 3: Board A (sender with a Caesar cipher)
//  No LCD is needed on this board.
// ---------------------------------------------------------------------
//  What this sketch does:
//    Every 2 seconds it takes the next message, encrypts it with a
//    Caesar cipher (every letter moved SHIFT places along the alphabet)
//    and sends the scrambled text to Board B, followed by a newline.
//
//        "Hello"  --shift 3-->  "Khoor"
//
//  Wiring: the same as Task 2
//    A D11 (TX) -> B D10 (RX)
//    A D10 (RX) <- B D11 (TX)
//    A GND      -> B GND
// =====================================================================

#include <SoftwareSerial.h>

SoftwareSerial link(10, 11);   // (RX pin = D10, TX pin = D11)

// The secret key: how many places each letter is moved.
// Board B must use the SAME number to decrypt.
const int SHIFT = 3;

const char* const messages[] = {"Hello", "Embedded", "Software"};
const uint8_t NUM_MESSAGES = 3;

const unsigned long SEND_INTERVAL = 2000;   // 2000 ms = 2 seconds

unsigned long lastSend = 0;    // time (ms) of the last send
uint8_t msgIndex = 0;          // which message to send next


// Move one letter 'shift' places along the alphabet.
// % 26 makes the end of the alphabet wrap round to the start,
// so with a shift of 3:  X -> A,  Y -> B,  Z -> C.
// Anything that is not a letter (space, digit, ...) is not changed.
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


void setup()
{
    link.begin(9600);          // the link to Board B
    Serial.begin(9600);        // USB Serial Monitor
    Serial.println(F("Sender ready"));
}


void loop()
{
    if (millis() - lastSend >= SEND_INTERVAL)
    {
        lastSend += SEND_INTERVAL;

        const char* msg = messages[msgIndex];

        Serial.print(F("Plain: "));
        Serial.print(msg);
        Serial.print(F("   Sent: "));

        // Encrypt and send the message one character at a time
        for (uint8_t i = 0; msg[i] != '\0'; i++)     // '\0' marks the end
        {
            char secret = shiftChar(msg[i], SHIFT);  // encrypt this letter
            link.print(secret);                      // send it to Board B
            Serial.print(secret);                    // and show it here
        }

        link.print('\n');      // newline = end of message
        Serial.println();

        // Next message (back to the first after the last one)
        msgIndex++;
        if (msgIndex == NUM_MESSAGES)
        {
            msgIndex = 0;
        }
    }
}
