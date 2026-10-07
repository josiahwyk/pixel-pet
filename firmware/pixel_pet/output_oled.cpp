#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "board_config.h"
#include "pet_config.h"
#include "output_oled.h"

static Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

bool outBegin() {
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) return false;
  oled.clearDisplay();
  oled.display();
  return true;
}

void outClear() {
  oled.clearDisplay();
}

void outShow() {
  oled.display();
}

// One sprite pixel = one small square on screen
static void block(int x, int y) {
  oled.fillRect(x, y, PET_SCALE, PET_SCALE, SSD1306_WHITE);
}

// Draw any grid of '#' and '.' at screen position x, y
static void drawRows(const char* const* rows, int w, int h, int x, int y) {
  for (int r = 0; r < h; r++) {
    for (int c = 0; c < w; c++) {
      if (rows[r][c] == '#') block(x + c * PET_SCALE, y + r * PET_SCALE);
    }
  }
}

void outDrawCat(int x, int y, bool tailOut, Face face, const char* name) {
  drawRows(tailOut ? CAT_TAIL_OUT : CAT_TAIL_IN, SPRITE_W, SPRITE_H, x, y);

  const char* const* faceRows = FACE_OPEN_ROWS;
  if (face == FACE_BLINK) faceRows = FACE_BLINK_ROWS;
  if (face == FACE_HAPPY) faceRows = FACE_HAPPY_ROWS;
  drawRows(faceRows, FACE_W, FACE_H,
           x + FACE_X * PET_SCALE, y + FACE_Y * PET_SCALE);

  // Name centred above the cat (each letter is 6 pixels wide)
  int nameX = x + (SPRITE_W * PET_SCALE) / 2 - strlen(name) * 3;
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(nameX, 0);
  oled.print(name);
}
