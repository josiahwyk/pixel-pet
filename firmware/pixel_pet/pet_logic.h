// Pet logic: moods and rules. No hardware in here.
#pragma once

// What a cat is doing right now. Higher in the list = shown first.
enum Mood {
  MOOD_IDLE,
  MOOD_HAPPY,       // after a pat
  MOOD_EATING,      // after feeding
  MOOD_BRUSHED,     // after a brush (purring)
  MOOD_GROOMING,    // by itself, while idle
  MOOD_SCRATCHING,  // by itself, while idle
  MOOD_WATCHING,    // watching the butterfly
  MOOD_SLEEPY,
  MOOD_HUNGRY,
};

enum PetEvent { EV_PET, EV_FEED, EV_BRUSH, EV_CLAP, EV_POKE, EV_BUTTERFLY };

struct Pet {
  Mood mood;
  unsigned long hungryAfter;     // this cat's appetite
  unsigned long lastFed;
  unsigned long lastAttention;   // last pet, feed, brush, clap or poke
  unsigned long happyUntil;
  unsigned long eatingUntil;
  unsigned long brushedUntil;
  Mood activity;                 // grooming, scratching or watching (or idle)
  unsigned long activityUntil;
  unsigned long nextIdleAct;     // when to groom or scratch next
};

void petInit(Pet& p, unsigned long hungryAfter, unsigned long now);
void petHandle(Pet& p, PetEvent ev, unsigned long now);   // something happened
void petUpdate(Pet& p, unsigned long now);                // time passes
void petStartActivity(Pet& p, Mood activity, unsigned long now);   // groom/scratch on demand
bool petWantsIdleAct(const Pet& p, unsigned long now);   // due to groom/scratch by itself?
void petDoIdleAct(Pet& p, unsigned long now);            // start one
bool petIsFree(const Pet& p);   // idle or doing its own thing (can watch a butterfly)
const char* moodName(Mood m);
