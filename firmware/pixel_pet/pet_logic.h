// Pet logic: moods and rules. No hardware in here.
#pragma once

enum Mood { MOOD_IDLE, MOOD_HAPPY, MOOD_EATING, MOOD_SLEEPY, MOOD_HUNGRY };
enum PetEvent { EV_PET, EV_FEED, EV_NOISE, EV_POKE };

struct Pet {
  Mood mood;
  unsigned long hungryAfter;     // this cat's appetite
  unsigned long lastFed;
  unsigned long lastAttention;   // last pet, feed, noise or poke
  unsigned long happyUntil;
  unsigned long eatingUntil;
};

void petInit(Pet& p, unsigned long hungryAfter, unsigned long now);
void petHandle(Pet& p, PetEvent ev, unsigned long now);   // something happened
void petUpdate(Pet& p, unsigned long now);                // time passes
const char* moodName(Mood m);
