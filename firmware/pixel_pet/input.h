// Input layer: turns touch, button and mic into pet events.
// Tap = pet, double tap = brush, hold = feed, clap = clap.
#pragma once
#include "pet_logic.h"

const int ALL_CATS = -1;

struct InputEvent {
  PetEvent ev;
  int cat;   // 0 = left, 1 = right, ALL_CATS = both
};

void inBegin();
// Check the inputs. Writes events into out[], returns how many.
int inPoll(unsigned long now, InputEvent out[], int maxEvents);
// How far through a feeding hold each cat is: 0 = not holding, 1 = fed
float inHoldProgress(int cat, unsigned long now);
