// Pixel Pet - Stage 2: two cats with moods, touch, button, mic and sound.
// Cheddar (left): touch module. Halloumi (right): button.
// Tap = pet, double tap = brush, hold 3 s = feed. A clap wakes sleepy cats.
// They groom and scratch by themselves, and a butterfly visits now and then.
// Serial Monitor: 'p' poke both, 'b' butterfly, 'g' groom, 's' scratch.
#include "board_config.h"
#include "pet_config.h"
#include "pet_logic.h"
#include "input.h"
#include "output_oled.h"
#include "output_sound.h"

const int FRAME_MS = 50;   // redraw 20 times a second

Pet pets[2];
Mood lastShown[2] = { MOOD_IDLE, MOOD_IDLE };   // to spot mood changes
unsigned long lastFrame = 0;
unsigned long lastHeard = 0;      // when the mic last heard a clap
unsigned long nextButterfly = 0;
unsigned long lastBusy = 0;       // last time anything happened (input or mood change)

// Pick a sound for what just happened
void reactWithSound(PetEvent ev, Mood before, Mood after) {
  if (ev == EV_FEED) soundPlay(SND_MUNCH);
  else if ((ev == EV_PET || ev == EV_BRUSH) && after == MOOD_HUNGRY) soundPlay(SND_MEW);   // "feed me"
  else if (after == MOOD_HAPPY) soundPlay(SND_CHIRP);
  else if (ev == EV_CLAP && before == MOOD_SLEEPY && after != MOOD_SLEEPY) soundPlay(SND_MEOW);
  else if (ev == EV_BUTTERFLY && after == MOOD_WATCHING && before != MOOD_WATCHING) soundPlay(SND_CHIRP);
  // Brushing purrs: handled in loop()
}

// Send one event to one cat, or to both
void sendEvent(PetEvent ev, int cat, unsigned long now) {
  for (int i = 0; i < 2; i++) {
    if (cat != ALL_CATS && cat != i) continue;
    Mood before = pets[i].mood;
    petHandle(pets[i], ev, now);
    reactWithSound(ev, before, pets[i].mood);
  }
}

void setup() {
  Serial.begin(115200);
  if (!outBegin()) {
    Serial.println("OLED not found. Check wiring and OLED_ADDR.");
    while (true) delay(1000);
  }
  soundBegin();
  inBegin();
  unsigned long now = millis();
  petInit(pets[0], PET1_HUNGRY_AFTER_MS, now);
  petInit(pets[1], PET2_HUNGRY_AFTER_MS, now);
  nextButterfly = now + PICK_GAP(BUTTERFLY_GAPS_MS);
  Serial.println("Pixel Pet stage 2 running");
}

void loop() {
  unsigned long now = millis();

  // 1. Inputs -> events
  InputEvent events[4];
  int n = inPoll(now, events, 4);
  for (int i = 0; i < n; i++) {
    if (events[i].ev == EV_CLAP) {
      if (soundBusy()) continue;   // that's our own buzzer, not you
      lastHeard = now;
      Serial.println("Clap!");
    }
    sendEvent(events[i].ev, events[i].cat, now);
    lastBusy = now;
  }

  // Test commands from the Serial Monitor (Stage 3 will do this over Wi-Fi)
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'p') sendEvent(EV_POKE, ALL_CATS, now);
    if (c == 'b') nextButterfly = now;
    if (c == 'g') for (Pet& p : pets) petStartActivity(p, MOOD_GROOMING, now);
    if (c == 's') for (Pet& p : pets) { petStartActivity(p, MOOD_SCRATCHING, now); soundPlay(SND_SCRATCH); }
  }

  // 2. Time passes
  for (int i = 0; i < 2; i++) petUpdate(pets[i], now);

  // Things the cats do by themselves: only when all has been calm for a while,
  // and only one at a time
  bool calm = now - lastBusy >= SETTLE_MS;
  bool anyIdle = pets[0].mood == MOOD_IDLE || pets[1].mood == MOOD_IDLE;
  if (calm && anyIdle && now >= nextButterfly) {
    sendEvent(EV_BUTTERFLY, ALL_CATS, now);
    nextButterfly = now + BUTTERFLY_MS + PICK_GAP(BUTTERFLY_GAPS_MS);
  } else if (calm) {
    int first = random(2);   // don't always check Cheddar first
    for (int k = 0; k < 2; k++) {
      int i = (first + k) % 2;
      if (petWantsIdleAct(pets[i], now)) { petDoIdleAct(pets[i], now); break; }
    }
  }

  for (int i = 0; i < 2; i++) {
    if (pets[i].mood != lastShown[i]) {
      lastBusy = now;
      lastShown[i] = pets[i].mood;
      Serial.printf("%s: %s\n", i == 0 ? PET1_NAME : PET2_NAME, moodName(pets[i].mood));
      if (pets[i].mood == MOOD_HUNGRY) soundPlay(SND_MEOW);
      if (pets[i].mood == MOOD_SCRATCHING) soundPlay(SND_SCRATCH);
    }
  }

  // 3. Outputs
  bool purr = pets[0].mood == MOOD_BRUSHED || pets[1].mood == MOOD_BRUSHED;
  if (purr && !soundBusy()) soundPlay(SND_PURR, true);       // purr the whole time
  if (!purr && soundNow() == SND_PURR) soundStop();
  soundUpdate(now);

  if (now - lastFrame >= FRAME_MS) {
    lastFrame = now;
    Mood moods[2] = { pets[0].mood, pets[1].mood };
    float hold[2] = { inHoldProgress(0, now), inHoldProgress(1, now) };
    bool showHeard = DEBUG_MIC && lastHeard > 0 && now - lastHeard < 600;
    outRender(moods, hold, showHeard, now);
  }
}
