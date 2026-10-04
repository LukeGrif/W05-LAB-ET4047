# Week 05 Lab — UART Communication and Data Integrity (ET4047)

**Module:** ET4047 · **Author:** Luke Griffin
**University of Limerick — Department of Electronic & Computer Engineering**

In this lab you send data from an Arduino back to itself, then between two
Arduinos, and finally add a **SHA-1 hash** to every message so the receiver can
detect whether anything was corrupted on the way. An optional extension adds
ACK/NACK replies and automatic retries.

| # | Task | What you do |
|---|------|-------------|
| 1 | UART loopback on a single board | Wire D1 (TX) to D0 (RX) and check that everything sent comes back |
| 2 | Communication between two boards | Board A sends, Board B receives with a line-buffered receiver |
| 3 | Message integrity with SHA-1 | Append a hash to each message; a tamper button corrupts messages on purpose. Exercise 3d is specific to your display |
| Ext | Acknowledgements (ACK/NACK) | The receiver replies and the sender retries failed messages |

---

## Choose the code for your display

The code comes as one folder per display. Use **only** the folder for your LCD.
Both folders contain the same sketches; only the lines that set up the LCD are
different.

| Display | Folder | Library (Tools → Manage Libraries…) |
|---------|--------|-------------------------------------|
| 16×2 LCD with I2C backpack | [`Code/I2C_Backpack_LCD`](Code/I2C_Backpack_LCD) | LiquidCrystal I2C (by Frank de Brabander) |
| Adafruit RGB LCD shield | [`Code/Adafruit_RGB_LCD_Shield`](Code/Adafruit_RGB_LCD_Shield) | Adafruit RGB LCD Shield Library (by Adafruit) |

Every sketch is a **single `.ino` file**, with no extra tabs or `.h` files to
add. The SHA-1 function is at the bottom of each sketch that uses it. All the
code is fully commented.

| Task | Board A (no LCD) | Board B (LCD) |
|------|------------------|---------------|
| 1 | `Task1_Loopback` (one board, D1 → D0) | — |
| 2 | `Task2_Sender` | `Task2_Receiver` |
| 3 | `Task3_Sender` | `Task3_Receiver` |
| Ext | `Extension_Sender_ACK` | `Extension_Receiver_ACK` |
| 3d answer | — | `Solutions/Task3d_Receiver_Solution` |

The sender sketches are the same in both folders, because Board A has no LCD.

---

## Repository contents

| Path | Description |
|------|-------------|
| `W05_LAB_UART_Communication_and_Data_Integrity.pdf` | The lab document (no code listings: students open the sketches) |
| `W05 LAB UART Communication and Data Integrity.docx` | Editable Word version |
| `W05_UART_Lab_Code.zip` | All the sketches in one zip, ready to hand out |
| `Code/` | The same sketches, unzipped |
