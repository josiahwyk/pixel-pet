#include <Arduino.h>
#include "board_config.h"
#include "output_sound.h"

// A note slides from freq to freqEnd over ms. freq 0 = silence.
struct Note { int freq; int freqEnd; int ms; };
const Note END = { 0, 0, 0 };   // marks the end of a tune

static const Note CHIRP[]   = { {1800, 1800, 60}, {0, 0, 30}, {2400, 2400, 80}, END };
static const Note MUNCH[]   = { {400, 400, 60}, {0, 0, 140}, {350, 350, 60}, {0, 0, 140},
                                {400, 400, 60}, {0, 0, 140}, {350, 350, 60}, {0, 0, 140},
                                {400, 400, 60}, END };
static const Note MEOW[]    = { {650, 1100, 150}, {1100, 550, 350}, END };    // "mi-aow"
static const Note MEW[]     = { {900, 1300, 160}, END };                      // short "mew?"
static const Note PURR[]    = { {55, 55, 700}, {0, 0, 120}, {45, 45, 600}, {0, 0, 300}, END };
static const Note SCRATCH[] = { {3200, 2600, 30}, {0, 0, 50}, {3000, 2400, 30}, {0, 0, 50},
                                {3200, 2600, 30}, {0, 0, 50}, {3000, 2400, 30}, END };

static const Note* tune = nullptr;
static Sound playing = SND_NONE;
static bool looping = false;
static int noteIndex = 0;
static unsigned long noteStart = 0;
static int lastFreq = -1;
static unsigned long stoppedAt = 0;   // when the last sound ended

static void setFreq(int f) {
  if (f == lastFreq) return;
  ledcWriteTone(PIN_BUZZER, f);   // 0 = silent
  lastFreq = f;
}

void soundBegin() {
  ledcAttach(PIN_BUZZER, 2000, 10);
  setFreq(0);
}

void soundPlay(Sound s, bool loop) {
  switch (s) {
    case SND_CHIRP:   tune = CHIRP;   break;
    case SND_MUNCH:   tune = MUNCH;   break;
    case SND_MEOW:    tune = MEOW;    break;
    case SND_MEW:     tune = MEW;     break;
    case SND_PURR:    tune = PURR;    break;
    case SND_SCRATCH: tune = SCRATCH; break;
    default:          soundStop();    return;
  }
  playing = s;
  looping = loop;
  noteIndex = 0;
  noteStart = millis();
}

void soundStop() {
  if (tune != nullptr) stoppedAt = millis();
  tune = nullptr;
  playing = SND_NONE;
  setFreq(0);
}

void soundUpdate(unsigned long now) {
  if (tune == nullptr) return;
  const Note* n = &tune[noteIndex];
  unsigned long t = now - noteStart;

  if (t >= (unsigned long)n->ms) {               // next note
    noteIndex++;
    noteStart = now;
    t = 0;
    n = &tune[noteIndex];
    if (n->ms == 0) {                            // end of tune
      if (looping) { noteIndex = 0; n = &tune[0]; }
      else { soundStop(); return; }
    }
  }

  // Slide the pitch for meows; steady for everything else
  int f = n->freq + (long)(n->freqEnd - n->freq) * (long)t / n->ms;
  if (n->freq == 0) f = 0;
  setFreq(f - f % 10);   // round to 10 Hz so we don't update every loop
}

// The mic still hears a sound for a moment after it ends, so count that too
bool soundBusy() { return tune != nullptr || millis() - stoppedAt < 400; }
Sound soundNow() { return playing; }
