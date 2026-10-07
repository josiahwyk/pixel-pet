# Stage 1: two cats on the screen

![preview](stage-1-preview.png)
Left: both cats idle. Middle: left cat blinking, tails swishing. Right: happy face (saved for Stage 2).

## Parts
- ESP32 board (KS0413) plugged into the IO shield
- 0.96" SSD1306 OLED
- 4 female-to-female jumper wires
- Micro-USB cable (the one from the kit, not a USB-C cable)

## Before wiring
Unplug USB. Seat the board in the shield so each label matches, e.g. board IO23 goes in shield socket IO23 and board 3V3 in shield 3V3.

## Wiring
Read the pin names printed on your OLED. The order is not always the same.

| OLED pin | Shield pin | Plain language |
|---|---|---|
| GND | G (black) on any row | ground |
| VCC | 3.3V (red) on any row | power, 3.3 V |
| SCL | S (blue) on the **IO22** row | clock line |
| SDA | S (blue) on the **IO21** row | data line |

## Upload (Claude does this with arduino-cli)
1. `firmware/i2c_scanner` first. It should print `Found device at 0x3C` (or 0x3D).
2. If it's 0x3D, change `OLED_ADDR` in `board_config.h`.
3. Then upload `firmware/pixel_pet`.

## Test checklist
- [ ] Scanner finds exactly one device
- [ ] Two cats appear with names above them
- [ ] Each cat bobs, swishes its tail and blinks at its own random times
- [ ] Unplug from the laptop, plug into a USB wall charger: the cats come back on their own

## If it doesn't work
| Symptom | Try |
|---|---|
| No serial port shows up | Use a different micro-USB cable (some are charge-only). Then check the shield's ON/OFF switch. |
| Upload fails "Failed to connect" | Hold the board's **Boot** button while the upload starts, then release. |
| Scanner: "No I2C devices found" | Check that SDA goes to IO21 and SCL to IO22 (not swapped), and that VCC is on 3.3V. |
| Screen stays black but scanner found it | Address mismatch: set `OLED_ADDR` to what the scanner printed. |
| Picture is shifted, with a noisy stripe on one side | The screen may use the SH1106 chip, not SSD1306. Tell Claude and switch to the U8g2 library. |
| Works on laptop, not on wall charger | Try another charger or cable. Very old 500 mA chargers can brown out. |

## Customise
- Names: `pet_config.h` (`PET1_NAME`, `PET2_NAME`, max 8 letters).
- Look: edit the `#` rows in `sprites_nebelung.h`.
