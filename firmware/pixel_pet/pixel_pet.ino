// Pixel Pet - Stage 1: two cats sit, breathe, swish tails and blink.
#include "board_config.h"
#include "pet_config.h"
#include "output_oled.h"

const int FRAME_MS = 50;   // redraw 20 times a second
const int CAT_Y = 10;      // top of the cats, below the names

// Animation state for one cat
struct Cat {
  const char* name;
  int x;
  int bob;                  // 0 or 1: breathing up/down
  bool tailOut;
  unsigned long nextBob;
  unsigned long nextTail;
  unsigned long nextBlink;
  unsigned long blinkUntil;
};

Cat cats[2] = {
  { PET1_NAME, 8,  0, false, 0, 0, 0, 0 },
  { PET2_NAME, 72, 0, false, 0, 0, 0, 0 },
};

unsigned long lastFrame = 0;

void setup() {
  Serial.begin(115200);
  if (!outBegin()) {
    Serial.println("OLED not found. Check wiring and OLED_ADDR.");
    while (true) delay(1000);
  }
  Serial.println("Pixel Pet stage 1 running");
}

// Each cat uses random timings, so they don't move in sync
void updateCat(Cat& c, unsigned long now) {
  if (now >= c.nextBob) {
    c.bob = 1 - c.bob;
    c.nextBob = now + random(600, 1000);
  }
  if (now >= c.nextTail) {
    c.tailOut = !c.tailOut;
    c.nextTail = now + random(300, 1500);
  }
  if (now >= c.nextBlink) {
    c.blinkUntil = now + 150;
    c.nextBlink = now + random(2000, 6000);
  }
}

void loop() {
  unsigned long now = millis();
  if (now - lastFrame < FRAME_MS) return;   // not time yet
  lastFrame = now;

  outClear();
  for (Cat& c : cats) {
    updateCat(c, now);
    Face face = (now < c.blinkUntil) ? FACE_BLINK : FACE_OPEN;
    outDrawCat(c.x, CAT_Y + c.bob, c.tailOut, face, c.name);
  }
  outShow();
}
