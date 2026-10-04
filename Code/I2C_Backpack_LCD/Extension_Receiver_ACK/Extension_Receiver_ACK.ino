// =====================================================================
//  Extension: Board B (receiver with SHA-1 check and ACK/NACK replies)
//  Display: 16x2 LCD with I2C backpack
// ---------------------------------------------------------------------
//  What this sketch does:
//    The Task 3 receiver, plus a reply to Board A after every frame:
//      "ACK\n"  if the hashes match
//      "NACK\n" if they do not (or the frame was bad / too long)
//    Replies go back on the return wire: B D11 (TX) -> A D10 (RX).
//
//  Wiring: the same as Task 3
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

const uint8_t MAX_LEN = 64;    // message + '|' + 40 hash characters fits
char buffer[MAX_LEN + 1];
uint8_t len = 0;
bool overflow = false;

unsigned int okCount = 0;
unsigned int failCount = 0;


// Tell Board A whether the frame arrived safely
void sendReply(bool ok)
{
    if (ok)
    {
        link.print(F("ACK\n"));
    }
    else
    {
        link.print(F("NACK\n"));
    }
}


// Check one complete frame "MESSAGE|HASH", then reply
void handleFrame(char* frame)
{
    // ----- 1. Split the frame at the '|' -----
    char* bar = strchr(frame, '|');
    if (bar == NULL)
    {
        Serial.println(F("Bad frame: no '|' separator"));
        sendReply(false);              // still reply, so A is not left waiting
        return;
    }
    *bar = '\0';
    char* msg = frame;
    char* rxHash = bar + 1;

    // ----- 2. Hash what arrived and compare -----
    char calcHash[41];
    sha1Hex(msg, calcHash);
    bool ok = (strcmp(calcHash, rxHash) == 0);
    if (ok)
    {
        okCount++;
    }
    else
    {
        failCount++;
    }

    // ----- 3. Reply straight away, so Board A is not kept waiting -----
    sendReply(ok);

    // ----- 4. Serial Monitor -----
    Serial.print(F("Message:  "));  Serial.println(msg);
    Serial.print(F("Received: "));  Serial.println(rxHash);
    Serial.print(F("Computed: "));  Serial.println(calcHash);
    if (ok)
    {
        Serial.println(F("Integrity: OK"));
    }
    else
    {
        Serial.println(F("Integrity: FAIL"));
    }
    Serial.println();

    // ----- 5. LCD -----
    lcd.clear();
    lcd.print(msg);
    lcd.setCursor(0, 1);
    if (ok)
    {
        lcd.print(F("OK   "));
    }
    else
    {
        lcd.print(F("FAIL "));
    }
    lcd.print(okCount);
    lcd.print('/');
    lcd.print(okCount + failCount);
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
        char c = link.read();

        if (c == '\r')
        {
            continue;
        }

        if (c == '\n')
        {
            buffer[len] = '\0';
            if (overflow)
            {
                Serial.println(F("Frame too long - discarded"));
                sendReply(false);      // still reply
            }
            else
            {
                handleFrame(buffer);
            }
            len = 0;
            overflow = false;
        }
        else if (len < MAX_LEN)
        {
            buffer[len] = c;
            len++;
        }
        else
        {
            overflow = true;
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
