// Output layer: turns each cat's mood into pictures on the screen.
// This is the part to rewrite for the T-QT Pro.
#pragma once
#include "pet_logic.h"

bool outBegin();   // start the screen. Returns false if not found.
// heard = true shows a small "(o)" so you can see the mic heard you
void outRender(const Mood moods[2], bool heard, unsigned long now);
