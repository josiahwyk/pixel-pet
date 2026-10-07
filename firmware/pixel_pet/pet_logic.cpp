#include "pet_logic.h"
#include "pet_config.h"

void petInit(Pet& p, unsigned long hungryAfter, unsigned long now) {
  p.mood = MOOD_IDLE;
  p.hungryAfter = hungryAfter;
  p.lastFed = now;
  p.lastAttention = now;
  p.happyUntil = 0;
  p.eatingUntil = 0;
}

static bool isHungry(const Pet& p, unsigned long now) {
  return now - p.lastFed > p.hungryAfter;
}

void petHandle(Pet& p, PetEvent ev, unsigned long now) {
  switch (ev) {
    case EV_PET:
    case EV_POKE:
      p.lastAttention = now;
      if (!isHungry(p, now)) p.happyUntil = now + HAPPY_MS;   // hungry cats want food, not pats
      break;
    case EV_FEED:
      p.lastFed = now;
      p.lastAttention = now;
      p.eatingUntil = now + EAT_MS;
      p.happyUntil = 0;
      break;
    case EV_NOISE:
      if (NOISE_WAKES_CATS && p.mood == MOOD_SLEEPY) p.lastAttention = now;   // wakes up
      break;
  }
  petUpdate(p, now);
}

void petUpdate(Pet& p, unsigned long now) {
  if (now < p.eatingUntil)                        p.mood = MOOD_EATING;
  else if (now < p.happyUntil)                    p.mood = MOOD_HAPPY;
  else if (isHungry(p, now))                      p.mood = MOOD_HUNGRY;
  else if (now - p.lastAttention > SLEEPY_AFTER_MS) p.mood = MOOD_SLEEPY;
  else                                            p.mood = MOOD_IDLE;
}

const char* moodName(Mood m) {
  switch (m) {
    case MOOD_HAPPY:  return "HAPPY";
    case MOOD_EATING: return "EATING";
    case MOOD_SLEEPY: return "SLEEPY";
    case MOOD_HUNGRY: return "HUNGRY";
    default:          return "IDLE";
  }
}
