#include <Arduino.h>
#include "board_config.h"
#include "output_sound.h"

struct Note { int freq; int ms; };   // freq 0 = silence

static const Note CHIRP[] = { {1800, 60}, {0, 30}, {2400, 80}, {0, 0} };
static const Note MUNCH[] = { {400, 60}, {0, 140}, {350, 60}, {0, 140}, {400, 60}, {0, 140},
                              {350, 60}, {0, 140}, {400, 60}, {0, 0} };
static const Note SAD[]   = { {700, 200}, {500, 350}, {0, 0} };
static const Note MEW[]   = { {900, 70}, {1200, 120}, {0, 0} };   // "mew?"
// {0, 0} marks the end of a tune

static const Note* tune = nullptr;
static int noteIndex = 0;
static unsigned long noteEnds = 0;

static void startNote(unsigned long now) {
  const Note& n = tune[noteIndex];
  if (n.ms == 0) { noTone(PIN_BUZZER); tune = nullptr; return; }   // tune finished
  if (n.freq > 0) tone(PIN_BUZZER, n.freq);
  else            noTone(PIN_BUZZER);
  noteEnds = now + n.ms;
}

void soundBegin() {
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);
}

void soundPlay(Sound s) {
  if (s == SND_CHIRP) tune = CHIRP;
  if (s == SND_MUNCH) tune = MUNCH;
  if (s == SND_SAD)   tune = SAD;
  if (s == SND_MEW)   tune = MEW;
  noteIndex = 0;
  startNote(millis());
}

void soundUpdate(unsigned long now) {
  if (tune == nullptr || now < noteEnds) return;
  noteIndex++;
  startNote(now);
}

bool soundBusy() {
  return tune != nullptr;
}
