#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "board_config.h"
#include "pet_config.h"
#include "output_oled.h"

static Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

const int PET_SCALE = 2;   // each sprite pixel = 2x2 screen pixels
const int CAT_Y = 10;      // top of the cats, below the names
const int CAT_X[2] = { 8, 72 };
const char* const NAMES[2] = { PET1_NAME, PET2_NAME };
const bool HAS_PATCH[2] = { PET1_PATCH, PET2_PATCH };

// Animation state for one cat (only used for drawing)
struct Anim {
  int bob;                  // up/down offset in pixels (minus = up)
  bool tailOut;
  unsigned long nextBob;
  unsigned long nextTail;
  unsigned long nextBlink;
  unsigned long blinkUntil;
  Mood lastMood;            // to know when a mood started
  unsigned long moodStart;
};
static Anim anim[2];

bool outBegin() {
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) return false;
  oled.clearDisplay();
  oled.display();
  return true;
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

// Same, but at normal size (for the heart and bowl)
static void drawSmall(const char* const* rows, int w, int h, int x, int y) {
  for (int r = 0; r < h; r++) {
    for (int c = 0; c < w; c++) {
      if (rows[r][c] == '#') oled.drawPixel(x + c, y + r, SSD1306_WHITE);
    }
  }
}

// Small text next to the cat's right ear (for "z" and "!")
static void drawMark(int x, int y, const char* text) {
  oled.setCursor(x + 40, y + 2);
  oled.print(text);
}

// One behaviour per mood
static void drawCat(int i, Mood mood, unsigned long now) {
  Anim& a = anim[i];
  if (mood != a.lastMood) { a.lastMood = mood; a.moodStart = now; }
  const char* const* face = FACE_OPEN_ROWS;

  switch (mood) {
    case MOOD_IDLE:     // breathe, swish tail, blink
      if (now >= a.nextBob)   { a.bob = a.bob ? 0 : 1; a.nextBob = now + random(600, 1000); }
      if (now >= a.nextTail)  { a.tailOut = !a.tailOut; a.nextTail = now + random(300, 1500); }
      if (now >= a.nextBlink) { a.blinkUntil = now + 150; a.nextBlink = now + random(2000, 6000); }
      if (now < a.blinkUntil) face = FACE_BLINK_ROWS;
      break;

    case MOOD_HAPPY:    // after a pat: hop, wag fast, heart
      if (now >= a.nextBob)  { a.bob = a.bob ? 0 : -3; a.nextBob = now + 150; }
      if (now >= a.nextTail) { a.tailOut = !a.tailOut; a.nextTail = now + 150; }
      face = FACE_HAPPY_ROWS;
      break;

    case MOOD_EATING:   // after feeding: sit still and chew
      a.bob = 0;
      a.tailOut = false;
      face = (now / 200) % 2 ? FACE_CHEW_ROWS : FACE_BLINK_ROWS;
      break;

    case MOOD_SLEEPY:   // eyes shut, slow breathing, "z"
      if (now >= a.nextBob) { a.bob = a.bob ? 0 : 1; a.nextBob = now + 1500; }
      a.tailOut = false;
      face = FACE_BLINK_ROWS;
      break;

    case MOOD_HUNGRY:   // frown, still tail, flashing "!"
      a.bob = 0;
      a.tailOut = false;
      face = FACE_HUNGRY_ROWS;
      break;
  }

  int x = CAT_X[i];
  int y = CAT_Y + a.bob;
  drawRows(a.tailOut ? CAT_TAIL_OUT : CAT_TAIL_IN, SPRITE_W, SPRITE_H, x, y);
  drawRows(face, FACE_W, FACE_H, x + FACE_X * PET_SCALE, y + FACE_Y * PET_SCALE);
  if (HAS_PATCH[i]) drawRows(PATCH_ROWS, PATCH_W, PATCH_H, x + PATCH_X * PET_SCALE, y + PATCH_Y * PET_SCALE);

  if (mood == MOOD_SLEEPY) drawMark(x, y, (now / 1000) % 2 ? "z" : "Z");
  if (mood == MOOD_HUNGRY && (now / 500) % 2) drawMark(x, y, "!");
  if (mood == MOOD_HAPPY) drawSmall(HEART_ROWS, HEART_W, HEART_H, x + 40, y + 2);

  // Food bowl under the cat while eating. The food gets smaller.
  if (mood == MOOD_EATING) {
    int bowlX = x + 16, bowlY = 58;
    drawSmall(BOWL_ROWS, BOWL_W, BOWL_H, bowlX, bowlY);
    int food = 12 - (int)((now - a.moodStart) * 12 / EAT_MS);   // 12 wide down to 0
    if (food > 0) oled.fillRect(bowlX + 8 - food / 2, bowlY - 2, food, 2, SSD1306_WHITE);
  }

  // Name centred above the cat (each letter is 6 pixels wide)
  oled.setCursor(x + (SPRITE_W * PET_SCALE) / 2 - strlen(NAMES[i]) * 3, 0);
  oled.print(NAMES[i]);
}

void outRender(const Mood moods[2], bool heard, unsigned long now) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  for (int i = 0; i < 2; i++) drawCat(i, moods[i], now);
  if (heard) {
    oled.setCursor(55, 56);  // bottom middle, under the gap between the cats
    oled.print("(o)");
  }
  oled.display();
}
