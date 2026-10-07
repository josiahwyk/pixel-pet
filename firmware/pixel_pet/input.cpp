#include <Arduino.h>
#include "board_config.h"
#include "pet_config.h"
#include "input.h"

const unsigned long DEBOUNCE_MS = 30;
const unsigned long MIC_WINDOW_MS = 50;    // measure loudness over 50 ms
const unsigned long NOISE_COOLDOWN_MS = 2000;

// One press-able input (touch or button)
struct Pressable {
  int pin;
  int cat;
  int restLevel;          // reading when nobody touches it
  bool down;
  bool holdFired;
  unsigned long changedAt;
  unsigned long downAt;
};

static Pressable touch  = { PIN_TOUCH,  0, LOW, false, false, 0, 0 };
static Pressable button = { PIN_BUTTON, 1, LOW, false, false, 0, 0 };

static int micMin = 4095, micMax = 0;
static unsigned long micWindowStart = 0, lastNoise = 0, lastMicPrint = 0;
static int micPeak = 0;

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

// Press = PET straight away. Keep holding = FEED.
static void pollPressable(Pressable& p, unsigned long now,
                          InputEvent out[], int& n, int maxEvents) {
  bool pressed = digitalRead(p.pin) != p.restLevel;

  if (pressed != p.down && now - p.changedAt > DEBOUNCE_MS) {
    p.down = pressed;
    p.changedAt = now;
    if (p.down) {
      p.downAt = now;
      p.holdFired = false;
      if (n < maxEvents) out[n++] = { EV_PET, p.cat };
    }
  }

  if (p.down && !p.holdFired && now - p.downAt > FEED_HOLD_MS && n < maxEvents) {
    p.holdFired = true;
    out[n++] = { EV_FEED, p.cat };
  }
}

// Loudness = biggest swing in the mic signal over a short window
static void pollMic(unsigned long now, InputEvent out[], int& n, int maxEvents) {
  int v = analogRead(PIN_MIC);
  if (v < micMin) micMin = v;
  if (v > micMax) micMax = v;
  if (now - micWindowStart < MIC_WINDOW_MS) return;

  int level = micMax - micMin;
  micMin = 4095; micMax = 0; micWindowStart = now;
  if (level > micPeak) micPeak = level;

  if (DEBUG_MIC && now - lastMicPrint > 500) {
    Serial.printf("mic level: %d\n", micPeak);
    micPeak = 0; lastMicPrint = now;
  }

  if (level > NOISE_THRESHOLD && now - lastNoise > NOISE_COOLDOWN_MS && n < maxEvents) {
    lastNoise = now;
    out[n++] = { EV_NOISE, ALL_CATS };
  }
}

int inPoll(unsigned long now, InputEvent out[], int maxEvents) {
  int n = 0;
  pollPressable(touch, now, out, n, maxEvents);
  pollPressable(button, now, out, n, maxEvents);
  pollMic(now, out, n, maxEvents);
  return n;
}
