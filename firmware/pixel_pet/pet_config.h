// Pick the pets here before gifting.
#pragma once

#define PET1_NAME "Cheddar"         // left cat, max 8 letters
#define PET2_NAME "Halloumi"        // right cat
#define PET1_PATCH false            // white chest patch? true / false
#define PET2_PATCH true
#include "sprites_nebelung.h"       // which breed's pictures to use

// Timers. Short values for testing. For a real gift use hours,
// e.g. 3UL * 60 * 60 * 1000 for 3 hours.
#define PET1_HUNGRY_AFTER_MS (3UL * 60 * 1000)   // 3 minutes after last meal
#define PET2_HUNGRY_AFTER_MS (3UL * 60 * 1000)
#define SLEEPY_AFTER_MS      (60UL * 1000)       // 1 minute with no attention
#define HAPPY_MS             3000                // happy after a pat
#define EAT_MS               2500                // eating after feeding
#define BRUSH_MS             4000                // purring after a brush

// Things the cats do by themselves while idle (grooming, scratching).
// The gap before the next one is picked at random from this list.
const unsigned long IDLE_ACT_GAPS_MS[] = { 15000, 60000, 90000 };
#define IDLE_ACT_MS          4000                // how long each one lasts
#define SETTLE_MS            8000                // calm this long before any of these start

// Butterfly visits. Gap picked at random from this list.
const unsigned long BUTTERFLY_GAPS_MS[] = { 60000, 120000, 180000 };
#define BUTTERFLY_MS         6000                // how long it flies across

// Pick a random gap from one of the lists above
#define PICK_GAP(list) (list[random(sizeof(list) / sizeof(list[0]))])

// Touch / button
#define FEED_HOLD_MS         3000                // hold this long = feed
#define TAP_MAX_MS           500                 // let go before this = a tap
#define DOUBLE_TAP_MS        350                 // 2nd tap within this = brush

// Mic: only a short, sharp sound (a clap) counts. Talking is ignored.
#define NOISE_THRESHOLD      120                 // how loud a clap must be
#define CLAP_MAX_MS          300                 // louder for longer = talking
#define CLAP_MIN_PEAK        650                 // claps are loud (talking peaked ~520)
#define CLAP_QUIET_BEFORE_MS 500                 // must be quiet this long before a clap
#define CLAP_WAKES_CATS      1                   // 1 = a clap wakes sleepy cats
#define DEBUG_MIC            1                   // 1 = describe each sound in the log, show (o)
