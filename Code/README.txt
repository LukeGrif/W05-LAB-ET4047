W05 LAB - UART Communication and Data Integrity (ET4047)
=========================================================

1. Find out which display you have:

     16x2 LCD with I2C backpack  (separate screen, 4 wires)  -> I2C_Backpack_LCD
     Adafruit RGB LCD shield     (plugs on, has 5 buttons)    -> Adafruit_RGB_LCD_Shield

2. Use ONLY the folder for your display. Both folders contain the same
   sketches; only the lines that set up the LCD are different.

3. Install the library for your display (Tools -> Manage Libraries...):

     I2C backpack LCD:  "LiquidCrystal I2C" by Frank de Brabander
     RGB LCD shield:    "Adafruit RGB LCD Shield Library" by Adafruit

   SoftwareSerial and Wire are built in.

4. Open each sketch with File -> Open. Every sketch is a single .ino file
   (the SHA-1 code for Task 4 is at the bottom of the Task 4 sketches).

   Task1_Loopback    one board, jumper D1 -> D0
   Task2_Sender      Board A (no LCD)
   Task2_Receiver    Board B (LCD)
   Task3_Sender      Board A (no LCD), Caesar cipher
   Task3_Receiver    Board B (LCD), Caesar cipher
   Task4_Sender      Board A (no LCD), SHA-1 hash, button on D2
   Task4_Receiver    Board B (LCD), SHA-1 check

The sender sketches are identical in both folders, because Board A
has no LCD.
