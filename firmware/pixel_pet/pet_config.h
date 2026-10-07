// Pick the pets here before gifting.
#pragma once

#define PET1_NAME "Cheddar"         // left cat, max 8 letters
#define PET2_NAME "Halloumi"        // right cat
#define PET1_PATCH false            // white chest patch? true / false
#define PET2_PATCH true
#include "sprites_nebelung.h"       // which breed's pictures to use

// Timers. Short values for testing. For a real gift use hours,
// e.g. 3UL * 60 * 60 * 1000 for 3 hours.
#define PET1_HUNGRY_AFTER_MS (2UL * 60 * 1000)   // 2 minutes
#define PET2_HUNGRY_AFTER_MS (3UL * 60 * 1000)   // 3 minutes
#define SLEEPY_AFTER_MS      (60UL * 1000)       // 1 minute with no attention
#define HAPPY_MS             3000                // how long happy lasts (after a pat)
#define EAT_MS               2500                // how long eating lasts (after feeding)
#define FEED_HOLD_MS         3000                // hold touch/button this long to feed

// Mic: how loud counts as "noise". Tune this using the Serial Monitor.
#define NOISE_THRESHOLD      120
#define DEBUG_MIC            0    // 1 = print mic levels and show (o) when heard
#define NOISE_WAKES_CATS     0    // 1 = a loud noise wakes sleepy cats
