// Output layer: buzzer sounds. Plays without stopping the animation.
#pragma once

enum Sound { SND_CHIRP, SND_MUNCH, SND_SAD, SND_MEW };

void soundBegin();
void soundPlay(Sound s);
void soundUpdate(unsigned long now);   // call often from loop()
bool soundBusy();                      // true while a tune is playing
