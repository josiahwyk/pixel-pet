#include <Arduino.h>
#include "board_config.h"
#include "pet_config.h"
#include "input.h"

const unsigned long DEBOUNCE_MS = 30;
const unsigned long MIC_WINDOW_MS = 25;    // measure loudness over 25 ms
const unsigned long CLAP_COOLDOWN_MS = 1000;

// One press-able input (touch or button)
struct Pressable {
  int pin;
  int cat;
  int restLevel;          // reading when nobody touches it
  bool down;
  bool holdFired;         // this press is used up (fed, or was a 2nd tap)
  bool tapWaiting;        // one tap done, waiting to see if a 2nd comes
  unsigned long changedAt;
  unsigned long downAt;
  unsigned long tapAt;
};

static Pressable touch  = { PIN_TOUCH,  0, LOW, false, false, false, 0, 0, 0 };
static Pressable button = { PIN_BUTTON, 1, LOW, false, false, false, 0, 0, 0 };

static void add(InputEvent out[], int& n, int maxEvents, PetEvent ev, int cat) {
  if (n < maxEvents) out[n++] = { ev, cat };
}

static void setupPressable(Pressable& p, const char* label) {
  pinMode(p.pin, INPUT);
  p.restLevel = digitalRead(p.pin);   // learn "not pressed" at start-up
  Serial.printf("%s rest level: %d\n", label, p.restLevel);
}

void inBegin() {
  setupPressable(touch, "Touch");
  setupPressable(button, "Button");
  pinMode(PIN_MIC, INPUT);
}

// Tap = PET. Double tap = BRUSH. Hold = FEED.
static void pollPressable(Pressable& p, unsigned long now,
                          InputEvent out[], int& n, int maxEvents) {
  bool pressed = digitalRead(p.pin) != p.restLevel;

  if (pressed != p.down && now - p.changedAt > DEBOUNCE_MS) {
    p.down = pressed;
    p.changedAt = now;
    if (p.down) {
      p.downAt = now;
      p.holdFired = false;
      if (p.tapWaiting) {              // 2nd tap: brush straight away
        p.tapWaiting = false;
        p.holdFired = true;
        add(out, n, maxEvents, EV_BRUSH, p.cat);
      }
    } else if (!p.holdFired && now - p.downAt < TAP_MAX_MS) {
      p.tapWaiting = true;             // a tap. Wait for a possible 2nd one.
      p.tapAt = now;
    }
  }

  // No 2nd tap came: it was a single pat
  if (p.tapWaiting && now - p.tapAt > DOUBLE_TAP_MS) {
    p.tapWaiting = false;
    add(out, n, maxEvents, EV_PET, p.cat);
  }

  if (p.down && !p.holdFired && now - p.downAt > FEED_HOLD_MS) {
    p.holdFired = true;
    add(out, n, maxEvents, EV_FEED, p.cat);
  }
}

// Clap = loud, then quiet again quickly. Talking stays loud for longer.
static int micMin = 4095, micMax = 0;
static unsigned long micWindowStart = 0, lastClap = 0;
static bool loud = false;
static unsigned long loudSince = 0, quietSince = 0;
static int soundPeak = 0;

static void pollMic(unsigned long now, InputEvent out[], int& n, int maxEvents) {
  int v = analogRead(PIN_MIC);
  if (v < micMin) micMin = v;
  if (v > micMax) micMax = v;
  if (now - micWindowStart < MIC_WINDOW_MS) return;

  int level = micMax - micMin;
  micMin = 4095; micMax = 0; micWindowStart = now;

  if (!loud && level > NOISE_THRESHOLD) {          // sound starts
    loud = true;
    loudSince = now;
    soundPeak = level;
  } else if (loud) {
    if (level > soundPeak) soundPeak = level;
    if (level < NOISE_THRESHOLD / 2) {             // sound stops
      loud = false;
      unsigned long length = now - loudSince;
      unsigned long quietBefore = loudSince - quietSince;
      bool clap = length <= CLAP_MAX_MS && quietBefore >= CLAP_QUIET_BEFORE_MS
                  && soundPeak >= CLAP_MIN_PEAK;
      if (DEBUG_MIC) Serial.printf("sound: peak %d, %lu ms long, quiet %lu ms before -> %s\n",
                                   soundPeak, length, quietBefore, clap ? "CLAP" : "not a clap");
      if (clap && now - lastClap > CLAP_COOLDOWN_MS) {
        lastClap = now;
        add(out, n, maxEvents, EV_CLAP, ALL_CATS);
      }
      quietSince = now;
    }
  }
}

int inPoll(unsigned long now, InputEvent out[], int maxEvents) {
  int n = 0;
  pollPressable(touch, now, out, n, maxEvents);
  pollPressable(button, now, out, n, maxEvents);
  pollMic(now, out, n, maxEvents);
  return n;
}

float inHoldProgress(int cat, unsigned long now) {
  Pressable& p = (cat == 0) ? touch : button;
  if (!p.down || p.holdFired) return 0;
  unsigned long held = now - p.downAt;
  if (held < TAP_MAX_MS) return 0;   // could still be a tap
  return (float)(held - TAP_MAX_MS) / (FEED_HOLD_MS - TAP_MAX_MS);
}
