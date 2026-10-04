// =====================================================================
//  Task 3: Board A (sender with SHA-1 hash and tamper button)
//  No LCD is needed on this board.
// ---------------------------------------------------------------------
//  What this sketch does:
//    Every 2 seconds it sends a frame to Board B made of:
//        MESSAGE | HASH \n       e.g.  "Testing|0820b32b...319acdfd\n"
//    The hash is the SHA-1 "fingerprint" of the message. Board B works
//    the hash out again and checks that the two match.
//
//    While the button is held down, one bit of the message is flipped
//    before it is sent (but the hash is of the ORIGINAL message).
//    This pretends a bit got corrupted on the wire.
//
//  Wiring: the same as Task 2, plus
//    Push button between D2 and GND
// =====================================================================

#include <SoftwareSerial.h>

SoftwareSerial link(10, 11);   // (RX pin = D10, TX pin = D11)

const char* const messages[] = {"Testing", "Embedded", "Software"};
const uint8_t NUM_MESSAGES = 3;

const uint8_t BUTTON_PIN = 2;                // tamper button on D2
const unsigned long SEND_INTERVAL = 2000;    // 2000 ms = 2 seconds
const uint8_t MAX_MSG = 31;                  // longest message we can copy

unsigned long lastSend = 0;    // time (ms) of the last send
uint8_t msgIndex = 0;          // which message to send next


void setup()
{
    link.begin(9600);
    Serial.begin(9600);

    // INPUT_PULLUP: the pin reads HIGH normally, and LOW while the
    // button is pressed (the button connects D2 to GND).
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    Serial.println(F("Sender ready"));
}


void loop()
{
    if (millis() - lastSend >= SEND_INTERVAL)
    {
        lastSend += SEND_INTERVAL;

        // ----- 1. Work out the hash of the ORIGINAL message -----
        char hash[41];                         // 40 hex characters + '\0'
        sha1Hex(messages[msgIndex], hash);

        // ----- 2. Copy the message into a buffer we are allowed to change -----
        char msg[MAX_MSG + 1];                 // +1 for the '\0'
        strncpy(msg, messages[msgIndex], MAX_MSG);
        msg[MAX_MSG] = '\0';                   // make sure it is terminated

        // ----- 3. Button held? Flip one bit to fake a transmission error -----
        bool tamper = (digitalRead(BUTTON_PIN) == LOW);
        if (tamper)
        {
            msg[0] = msg[0] ^ 0x01;            // XOR flips bit 0: 'E' becomes 'D'
        }

        // ----- 4. Send the frame "MESSAGE|HASH\n" -----
        link.print(msg);
        link.print('|');                       // separator
        link.print(hash);
        link.print('\n');                      // end of frame

        // ----- 5. Show what was sent on the Serial Monitor -----
        if (tamper)
        {
            Serial.print(F("Sent (TAMPERED): "));
        }
        else
        {
            Serial.print(F("Sent: "));
        }
        Serial.print(msg);
        Serial.print('|');
        Serial.println(hash);

        // ----- 6. Next message -----
        msgIndex++;
        if (msgIndex == NUM_MESSAGES)
        {
            msgIndex = 0;
        }
    }
}


// =====================================================================
//  SHA-1 HASH FUNCTION
// ---------------------------------------------------------------------
//  You do NOT need to change anything below this line, or understand
//  every detail. All you need to know is how to call it:
//
//      char hash[41];                // 40 hex characters + '\0'
//      sha1Hex("Testing", hash);     // hash is now
//                                    // "0820b32b206b7352858e8903a838ed14319acdfd"
//
//  How SHA-1 works, in three steps:
//    1. Padding: add a 1 bit, then 0 bits, then the message length,
//       so the total is a whole number of 64-byte blocks.
//    2. Mixing: each 64-byte block goes through 80 rounds that scramble
//       five 32-bit numbers h[0]..h[4] (the function sha1Block).
//    3. Output: the final h[0]..h[4] (5 x 32 = 160 bits) is the hash,
//       written as 40 hexadecimal characters.
// =====================================================================

// Rotate a 32-bit number left by n bits (bits pushed out on the left
// come back in on the right)
uint32_t rotateLeft(uint32_t x, uint8_t n)
{
    return (x << n) | (x >> (32 - n));
}

// Step 2: mix one 64-byte block into the five state numbers h[0..4]
void sha1Block(const uint8_t* block, uint32_t* h)
{
    // Turn the 64 bytes into 16 32-bit words.
    // (Textbook SHA-1 uses 80 words here; keeping only 16 and reusing
    // them saves 256 bytes of RAM, which matters on a Nano.)
    uint32_t w[16];
    for (uint8_t i = 0; i < 16; i++)
    {
        w[i] = ((uint32_t)block[i * 4]     << 24) |
               ((uint32_t)block[i * 4 + 1] << 16) |
               ((uint32_t)block[i * 4 + 2] << 8)  |
                (uint32_t)block[i * 4 + 3];
    }

    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];

    for (uint8_t t = 0; t < 80; t++)       // 80 rounds of mixing
    {
        if (t >= 16)                       // make the next word from earlier ones
        {
            w[t & 15] = rotateLeft(w[(t + 13) & 15] ^ w[(t + 8) & 15] ^
                                   w[(t + 2) & 15]  ^ w[t & 15], 1);
        }

        uint32_t f, k;                     // each group of 20 rounds
        if (t < 20)      { f = (b & c) | (~b & d);          k = 0x5A827999UL; }
        else if (t < 40) { f = b ^ c ^ d;                   k = 0x6ED9EBA1UL; }
        else if (t < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDCUL; }
        else             { f = b ^ c ^ d;                   k = 0xCA62C1D6UL; }

        uint32_t temp = rotateLeft(a, 5) + f + e + k + w[t & 15];
        e = d;
        d = c;
        c = rotateLeft(b, 30);
        b = a;
        a = temp;
    }

    h[0] += a;  h[1] += b;  h[2] += c;  h[3] += d;  h[4] += e;
}

// Work out the SHA-1 of the text msg and write it into out
// as 40 lowercase hex characters plus '\0' (so out needs 41 chars)
void sha1Hex(const char* msg, char* out)
{
    // Starting values, fixed by the SHA-1 standard
    uint32_t h[5] = {0x67452301UL, 0xEFCDAB89UL, 0x98BADCFEUL,
                     0x10325476UL, 0xC3D2E1F0UL};

    uint16_t len = strlen(msg);            // message length in bytes
    uint16_t pos = 0;                      // how much we have processed
    uint8_t block[64];

    // Process every complete 64-byte block of the message
    while (len - pos >= 64)
    {
        memcpy(block, msg + pos, 64);
        sha1Block(block, h);
        pos += 64;
    }

    // Step 1: padding. Copy what is left, then add a single 1 bit (0x80).
    uint8_t rest = len - pos;
    memset(block, 0, 64);                  // fill with zeros
    memcpy(block, msg + pos, rest);
    block[rest] = 0x80;

    // The last 8 bytes must hold the length. If there is no room,
    // finish this block and use one more block of zeros.
    if (rest >= 56)
    {
        sha1Block(block, h);
        memset(block, 0, 64);
    }

    // Message length in BITS, most significant byte first
    uint32_t bits = (uint32_t)len * 8;
    block[60] = bits >> 24;
    block[61] = bits >> 16;
    block[62] = bits >> 8;
    block[63] = bits;
    sha1Block(block, h);

    // Step 3: write h[0..4] out as 40 hex characters
    const char hexDigits[] = "0123456789abcdef";
    for (uint8_t i = 0; i < 20; i++)
    {
        uint8_t value = h[i / 4] >> (24 - 8 * (i % 4));   // one byte of the hash
        out[i * 2]     = hexDigits[value >> 4];           // high 4 bits
        out[i * 2 + 1] = hexDigits[value & 0x0F];         // low 4 bits
    }
    out[40] = '\0';
}
