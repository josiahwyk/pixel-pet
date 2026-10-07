# Pixel Pet: Step 0 (inventory and plan)

Date: 2026-10-02. Source: photos IMG_0126–0131 in ~/Documents/Physical AI, plus the earlier kit inventory.

## Kits confirmed from photos
- **Keyestudio ESP32 Learning Kit Complete Edition (KS5011)**: ESP32 mainboard (ESP32-WROOM-32), LCD_128X32_DOT module, button modules x4, passive + active buzzer, RGB module, tilt switch, photoresistors, 6xAA battery holder, breadboard power module, and more.
- **Keyestudio 48-in-1 Sensor Kit (KS0522)**. Tutorial: https://docs.keyestudio.com/projects/KS0522/en/latest/
- **Keyestudio IO Shield for ESP-32**: G/V/S pin rows, ON/OFF switch, DC jack.
- **Standalone:** 0.96" SSD1306 I2C OLED, 128x64 (confirmed on hand 2026-10-03).
- **Standalone:** 125pc assorted tact switch kit (25 types; SMD + DIP; 2x4, 3x6, 4x4, 6x6 mm). For the MVP, use the kit's button module instead. Keep the 6x6 DIP switches for the gift enclosure later (wire with `INPUT_PULLUP`, use diagonal legs). Skip the SMD ones (they need soldering).

## Shortlist for the pet
| Role | Module | Kit | Why |
|---|---|---|---|
| Screen (main) | 0.96" SSD1306 OLED 128x64 | standalone | Best resolution you have. Libraries are already installed. |
| Screen (backup) | LCD_128X32_DOT | KS5011 | Only half the height. Driver chip unconfirmed. |
| Sound | Passive buzzer module | KS0522 | Can play tunes (chirp, purr, sad tone). |
| Mood glow (optional) | RGB LED module | KS0522 | One colour per mood. |
| Input: pet | Capacitive touch module | KS0522 | Feels like stroking. Gives a simple on/off signal. |
| Input: feed | Digital push button module | KS0522 | Clear "feed" action. |
| Input: voice | Analog sound sensor (mic) | KS0522 | Detects how loud it is, not words. |
| Input: shake (optional) | Digital tilt sensor | KS0522 | Shake to wake. |

Not chosen: 8x8 dot matrix and 0802 LCD (too coarse for a pet).

## ESP32-WROOM-32 pin rules (chip-level, apply to any WROOM-32 board)
- Never use GPIO 6–11 (wired to flash memory).
- Avoid GPIO 0, 2, 12, 15 (boot pins) and 1, 3 (USB serial).
- GPIO 34, 35, 36, 39 are input-only, with no internal pull-up.
- Analog reads on GPIO 0, 2, 4, 12–15, 25–27 stop working once Wi-Fi is on, so the mic must use 32–39.
- Default I2C pins: SDA 21, SCL 22.
- Logic is 3.3 V. Power the modules from 3V3, not 5V, unless told otherwise.

## Board confirmed (IMG_0133, 2026-10-03)
- **Keyestudio Control Board for ESP-32 (KS0413)**: ESP32-WROOM-32, 38 pins (19 per side), **micro-USB**, EN and Boot buttons.
- **Keyestudio IO Shield for ESP-32 (KS0413-compatible, 19x2 sockets)**. The back-side labels match the 38-pin layout, and every GPIO below is broken out.
- Still unconfirmed: whether the shield's "V" pins carry 3.3 V or 5 V (check the top side for a jumper or label).

## Pin plan (pins confirmed to exist on this board/shield)
| Module pin | ESP32 GPIO |
|---|---|
| OLED SDA | 21 |
| OLED SCL | 22 |
| OLED VCC / GND | 3V3 / GND |
| Touch S | 27 |
| Button S | 26 |
| Sound sensor A (analog) | 34 |
| Passive buzzer S | 25 |
| RGB R / G / B (optional) | 16 / 17 / 18 |
| Tilt S (optional) | 33 |

## Toolchain found on this Mac
- arduino-cli 1.5.1, ESP32 core 3.3.11 (3.x: the buzzer uses `ledcAttach`, not the old `ledcSetup`)
- Libraries: Adafruit SSD1306, Adafruit GFX, U8g2, ArduinoJson
- Claude can compile and upload with arduino-cli, so the IDE window isn't needed.

## Open questions
See the chat reply. Main ones: board photo and USB type, whether the OLED is still on hand, whether to use the IO shield.
