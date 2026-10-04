# Week 05 Lab — UART Communication and Data Integrity (ET4047)

**Module:** ET4047 · **Author:** Luke Griffin
**University of Limerick — Department of Electronic & Computer Engineering**

🌐 **Open the lab online:** https://lukegrif.github.io/W05-LAB-ET4047/

In this lab you send data from an Arduino back to itself, then between two
Arduinos, then scramble every message with a **Caesar cipher** so that only a
board with the right key can read it, and finally add a **SHA-1 hash** so the
receiver can tell whether a message was damaged on the way.

| # | Task | What you do |
|---|------|-------------|
| 1 | UART loopback on a single board | Wire D1 (TX) to D0 (RX) and check that everything sent comes back |
| 2 | Communication between two boards | Board A sends, Board B receives with a line-buffered receiver |
| 3 | Secret messages with a Caesar cipher | Board A encrypts each message (shift 3), Board B decrypts it |
| 4 | Checking messages with SHA-1 | Board A sends a hash with each message; a button damages messages on purpose and Board B detects it |

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
add. The SHA-1 code is at the bottom of the two Task 4 sketches. All the code is
fully commented.

| Task | Board A (no LCD) | Board B (LCD) |
|------|------------------|---------------|
| 1 | `Task1_Loopback` (one board, D1 → D0) | — |
| 2 | `Task2_Sender` | `Task2_Receiver` |
| 3 | `Task3_Sender` | `Task3_Receiver` |
| 4 | `Task4_Sender` (button on D2) | `Task4_Receiver` |

The sender sketches are the same in both folders, because Board A has no LCD.

---

## Repository contents

| Path | Description |
|------|-------------|
| `index.html` | GitHub Pages landing page for the lab |
| `viewer.html` | Renders the lab PDF in the browser |
| `assets/` | University logos used on the landing page |
| `W05_LAB_UART_Communication_and_Data_Integrity.pdf` | The lab document (no code listings: students open the sketches) |
| `W05 LAB UART Communication and Data Integrity.docx` | Editable Word version |
| `W05_UART_Lab_Code.zip` | All the sketches in one zip, ready to hand out |
| `Code/` | The same sketches, unzipped |
