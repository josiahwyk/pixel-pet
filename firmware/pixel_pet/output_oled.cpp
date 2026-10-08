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

// Butterfly position this frame (-100 = not flying)
static int flyX = -100, flyY = 0;
static unsigned long flyStart = 0;
static bool flying = false;

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

// Draw any grid of '#' and '.' at cat size
static void drawRows(const char* const* rows, int w, int h, int x, int y) {
  for (int r = 0; r < h; r++)
    for (int c = 0; c < w; c++)
      if (rows[r][c] == '#') block(x + c * PET_SCALE, y + r * PET_SCALE);
}

// Same, but at normal size, with a black box behind so it shows on top
static void drawSmall(const char* const* rows, int w, int h, int x, int y, bool clearBehind = false) {
  if (clearBehind) oled.fillRect(x - 1, y - 1, w + 2, h + 2, SSD1306_BLACK);
  for (int r = 0; r < h; r++)
    for (int c = 0; c < w; c++)
      if (rows[r][c] == '#') oled.drawPixel(x + c, y + r, SSD1306_WHITE);
}

// Small text next to the cat's right ear (for "z", "!" and "~")
static void drawMark(int x, int y, const char* text) {
  oled.setCursor(x + 40, y + 2);
  oled.print(text);
}

// Scratching post: left of Cheddar, right of Halloumi (screen edges)
const bool POST_ON_RIGHT[2] = { false, true };

static void drawPost(int i) {
  int catX = CAT_X[i];
  int px = POST_ON_RIGHT[i] ? catX + SPRITE_W * PET_SCALE + 2 : catX - 8;
  int top = 22, bottom = 55;
  oled.drawRect(px, top, 6, bottom - top, SSD1306_WHITE);
  for (int y = top + 3; y < bottom - 2; y += 4)   // rope stripes
    oled.drawLine(px + 1, y + 2, px + 4, y, SSD1306_WHITE);
  oled.drawFastHLine(px - 2, bottom, 10, SSD1306_WHITE);
}

// One behaviour per mood
static void drawCat(int i, Mood mood, float hold, unsigned long now) {
  Anim& a = anim[i];
  if (mood != a.lastMood) { a.lastMood = mood; a.moodStart = now; a.bob = 0; }
  unsigned long t = now - a.moodStart;   // time in this mood
  const char* const* face = FACE_OPEN_ROWS;
  int x = CAT_X[i];
  int centre = x + SPRITE_W;             // middle of the cat on screen

  switch (mood) {
    case MOOD_IDLE:       // breathe, swish tail, blink
      if (now >= a.nextBob)   { a.bob = a.bob ? 0 : 1; a.nextBob = now + random(600, 1000); }
      if (now >= a.nextTail)  { a.tailOut = !a.tailOut; a.nextTail = now + random(300, 1500); }
      if (now >= a.nextBlink) { a.blinkUntil = now + 150; a.nextBlink = now + random(2000, 6000); }
      if (now < a.blinkUntil) face = FACE_BLINK_ROWS;
      break;

    case MOOD_HAPPY:      // after a pat: hop, wag fast, heart
      if (now >= a.nextBob)  { a.bob = a.bob ? 0 : -3; a.nextBob = now + 150; }
      if (now >= a.nextTail) { a.tailOut = !a.tailOut; a.nextTail = now + 150; }
      face = FACE_HAPPY_ROWS;
      break;

    case MOOD_EATING:     // after feeding: sit still and chew
      a.tailOut = false;
      face = (t / 200) % 2 ? FACE_CHEW_ROWS : FACE_BLINK_ROWS;
      break;

    case MOOD_BRUSHED:    // eyes shut, slow tail, purring "~"
      if (now >= a.nextTail) { a.tailOut = !a.tailOut; a.nextTail = now + 600; }
      face = FACE_BLINK_ROWS;
      break;

    case MOOD_GROOMING:   // rub the cheek with a paw, eyes shut
      if (now >= a.nextTail) { a.tailOut = !a.tailOut; a.nextTail = now + 900; }
      face = FACE_BLINK_ROWS;
      break;

    case MOOD_SCRATCHING: // claw the post
      a.tailOut = (t / 300) % 2;
      break;

    case MOOD_WATCHING:   // eyes follow the butterfly, hop when it's close
      if (flying) {
        face = (flyX < centre - 6) ? FACE_LOOK_L_ROWS : (flyX > centre + 6) ? FACE_LOOK_R_ROWS : FACE_OPEN_ROWS;
        bool close = abs(flyX + FLY_W / 2 - centre) < 14;
        a.bob = (close && (t / 150) % 2) ? -4 : 0;
      }
      a.tailOut = (t / 200) % 2;
      break;

    case MOOD_SLEEPY:     // eyes shut, slow breathing, "z"
      if (now >= a.nextBob) { a.bob = a.bob ? 0 : 1; a.nextBob = now + 1500; }
      a.tailOut = false;
      face = FACE_BLINK_ROWS;
      break;

    case MOOD_HUNGRY:     // frown, still tail, flashing "!"
      a.bob = 0;
      a.tailOut = false;
      face = FACE_HUNGRY_ROWS;
      break;
  }

  int y = CAT_Y + a.bob;
  if (mood == MOOD_SCRATCHING) drawPost(i);
  drawRows(a.tailOut ? CAT_TAIL_OUT : CAT_TAIL_IN, SPRITE_W, SPRITE_H, x, y);
  drawRows(face, FACE_W, FACE_H, x + FACE_X * PET_SCALE, y + FACE_Y * PET_SCALE);
  if (HAS_PATCH[i]) drawRows(PATCH_ROWS, PATCH_W, PATCH_H, x + PATCH_X * PET_SCALE, y + PATCH_Y * PET_SCALE);

  // Legs that are lifted up are hidden from the floor.
  // Grooming lifts one leg. Scratching lifts both.
  if (mood == MOOD_GROOMING || mood == MOOD_SCRATCHING)
    oled.fillRect(x + LEG_LEFT_COL * PET_SCALE, y + LEG_TOP * PET_SCALE, PET_SCALE, 2 * PET_SCALE, SSD1306_BLACK);
  if (mood == MOOD_SCRATCHING)
    oled.fillRect(x + LEG_RIGHT_COL * PET_SCALE, y + LEG_TOP * PET_SCALE, PET_SCALE, 2 * PET_SCALE, SSD1306_BLACK);

  // Grooming: paw rubs up and down the cheek
  if (mood == MOOD_GROOMING) {
    const char* const* paw = (t / 300) % 2 ? GROOM_UP_ROWS : GROOM_DOWN_ROWS;
    drawRows(paw, GROOM_W, GROOM_H, x + GROOM_X * PET_SCALE, y + GROOM_Y * PET_SCALE);
  }
  // Scratching: both paws claw the post, one up while the other is down
  if (mood == MOOD_SCRATCHING) {
    int outer = POST_ON_RIGHT[i] ? SPRITE_W - 2 : -2;   // paw touching the post
    int inner = POST_ON_RIGHT[i] ? outer - 3 : outer + 3; // other paw, just beside it
    bool flip = (t / 120) % 2;
    drawRows(PAW_ROWS, PAW_W, PAW_H, x + outer * PET_SCALE, y + (flip ? 8 : 12) * PET_SCALE);
    drawRows(PAW_ROWS, PAW_W, PAW_H, x + inner * PET_SCALE, y + (flip ? 12 : 8) * PET_SCALE);
  }
  // Brushing: brush sweeps back and forth across the chest
  if (mood == MOOD_BRUSHED) {
    int sweep = (t / 40) % 40;                      // 0..39
    int bx = x + 8 + (sweep < 20 ? sweep : 39 - sweep);
    drawSmall(BRUSH_ROWS, BRUSH_W, BRUSH_H, bx, y + 30, true);
  }

  if (mood == MOOD_SLEEPY) drawMark(x, y, (now / 1000) % 2 ? "z" : "Z");
  if (mood == MOOD_BRUSHED) drawMark(x, y, (now / 400) % 2 ? "~" : " ~");
  if (mood == MOOD_HUNGRY && (now / 500) % 2) drawMark(x, y, "!");
  if (mood == MOOD_HAPPY) drawSmall(HEART_ROWS, HEART_W, HEART_H, x + 40, y + 2);

  // Food bowl under the cat: fills while you hold, empties while eating
  int food = -1;
  if (hold > 0 && mood != MOOD_EATING) food = (int)(hold * 12);
  if (mood == MOOD_EATING) food = 12 - (int)(t * 12 / EAT_MS);
  if (food >= 0) {
    int bowlX = x + 16, bowlY = 58;
    drawSmall(BOWL_ROWS, BOWL_W, BOWL_H, bowlX, bowlY);
    if (food > 0) oled.fillRect(bowlX + 8 - food / 2, bowlY - 2, food, 2, SSD1306_WHITE);
  }

  // Name centred above the cat (each letter is 6 pixels wide)
  oled.setCursor(x + (SPRITE_W * PET_SCALE) / 2 - strlen(NAMES[i]) * 3, 0);
  oled.print(NAMES[i]);
}

// Butterfly flies left to right, bobbing up and down
static void updateButterfly(const Mood moods[2], unsigned long now) {
  bool anyWatching = moods[0] == MOOD_WATCHING || moods[1] == MOOD_WATCHING;
  if (anyWatching && !flying) { flying = true; flyStart = now; }
  if (!anyWatching) { flying = false; flyX = -100; return; }
  unsigned long t = now - flyStart;
  flyX = -8 + (int)(t * (OLED_WIDTH + 16) / BUTTERFLY_MS);
  flyY = 18 + (int)(8 * sin(t / 250.0));
}

void outRender(const Mood moods[2], const float hold[2], bool heard, unsigned long now) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  updateButterfly(moods, now);
  for (int i = 0; i < 2; i++) drawCat(i, moods[i], hold[i], now);
  if (flying) drawSmall((now / 120) % 2 ? FLY_OPEN_ROWS : FLY_SHUT_ROWS, FLY_W, FLY_H, flyX, flyY, true);
  if (heard) {
    oled.setCursor(55, 56);  // bottom middle, under the gap between the cats
    oled.print("(o)");
  }
  oled.display();
}
