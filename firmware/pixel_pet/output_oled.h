// Output layer: everything the pets show on the screen.
// This is the part to rewrite for the T-QT Pro.
#pragma once

const int PET_SCALE = 2;   // each sprite pixel = 2x2 screen pixels

enum Face { FACE_OPEN, FACE_BLINK, FACE_HAPPY };

bool outBegin();   // start the screen. Returns false if not found.
void outClear();   // start a new picture
void outDrawCat(int x, int y, bool tailOut, Face face, const char* name);
void outShow();    // send the picture to the screen
