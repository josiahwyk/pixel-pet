// All pin numbers live here. This file changes when you change boards.
// Board: Keyestudio KS0413 ESP32 (ESP32-WROOM-32) on the Keyestudio IO shield.
#pragma once

// OLED screen (I2C)
#define PIN_OLED_SDA 21
#define PIN_OLED_SCL 22
#define OLED_ADDR    0x3C   // found by the I2C scanner
#define OLED_WIDTH   128
#define OLED_HEIGHT  64

// Inputs
#define PIN_TOUCH    27     // capacitive touch module: left cat
#define PIN_BUTTON   26     // push button module: right cat
#define PIN_MIC      34     // analog sound sensor (must be 32-39, see docs)

// Sound
#define PIN_BUZZER   25     // passive buzzer module
