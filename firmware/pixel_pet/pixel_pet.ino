// Pixel Pet - Stage 2: two cats with moods, touch, button, mic and sound.
// Left cat: touch module. Right cat: button. Press = pet, keep holding 3 s = feed.
// A loud noise wakes sleepy cats. Type 'p' in Serial Monitor to poke both.
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
unsigned long lastHeard = 0;   // when the mic last heard a noise

// Pick a sound for what just happened
void reactWithSound(PetEvent ev, Mood before, Mood after) {
  if (ev == EV_FEED) soundPlay(SND_MUNCH);
  else if (ev == EV_PET && after == MOOD_HUNGRY) soundPlay(SND_MEW);   // "feed me"
  else if (after == MOOD_HAPPY) soundPlay(SND_CHIRP);
  else if (ev == EV_NOISE && before == MOOD_SLEEPY && after != MOOD_SLEEPY) soundPlay(SND_CHIRP);
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
  Serial.println("Pixel Pet stage 2 running");
}

void loop() {
  unsigned long now = millis();

  // 1. Inputs -> events
  InputEvent events[4];
  int n = inPoll(now, events, 4);
  for (int i = 0; i < n; i++) {
    if (events[i].ev == EV_NOISE) {
      if (soundBusy()) continue;   // that's our own buzzer, not you
      lastHeard = now;
    }
    sendEvent(events[i].ev, events[i].cat, now);
  }

  // Test poke from the Serial Monitor (Stage 3 will do this over Wi-Fi)
  if (Serial.available() && Serial.read() == 'p') sendEvent(EV_POKE, ALL_CATS, now);

  // 2. Time passes
  for (int i = 0; i < 2; i++) {
    petUpdate(pets[i], now);
    if (pets[i].mood != lastShown[i]) {
      lastShown[i] = pets[i].mood;
      Serial.printf("%s: %s\n", i == 0 ? PET1_NAME : PET2_NAME, moodName(pets[i].mood));
      if (pets[i].mood == MOOD_HUNGRY) soundPlay(SND_SAD);
    }
  }

  // 3. Outputs
  soundUpdate(now);
  if (now - lastFrame >= FRAME_MS) {
    lastFrame = now;
    Mood moods[2] = { pets[0].mood, pets[1].mood };
    bool showHeard = DEBUG_MIC && lastHeard > 0 && now - lastHeard < 600;
    outRender(moods, showHeard, now);
  }
}
