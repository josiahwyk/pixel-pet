// Output layer: buzzer sounds. Plays without stopping the animation.
#pragma once

enum Sound { SND_NONE, SND_CHIRP, SND_MUNCH, SND_MEOW, SND_MEW, SND_PURR, SND_SCRATCH };

void soundBegin();
void soundPlay(Sound s, bool loop = false);   // loop = repeat until stopped
void soundStop();
void soundUpdate(unsigned long now);   // call often from loop()
bool soundBusy();                      // true while playing, and briefly after (echo)
Sound soundNow();                      // what is playing (SND_NONE if quiet)
