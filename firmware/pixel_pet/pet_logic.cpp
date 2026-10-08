#include <Arduino.h>   // only for random()
#include "pet_logic.h"
#include "pet_config.h"

void petInit(Pet& p, unsigned long hungryAfter, unsigned long now) {
  p.mood = MOOD_IDLE;
  p.hungryAfter = hungryAfter;
  p.lastFed = now;
  p.lastAttention = now;
  p.happyUntil = 0;
  p.eatingUntil = 0;
  p.brushedUntil = 0;
  p.activity = MOOD_IDLE;
  p.activityUntil = 0;
  p.nextIdleAct = now + PICK_GAP(IDLE_ACT_GAPS_MS);
}

static bool isHungry(const Pet& p, unsigned long now) {
  return now - p.lastFed > p.hungryAfter;
}

static void stopActivity(Pet& p) {
  p.activity = MOOD_IDLE;
  p.activityUntil = 0;
}

void petHandle(Pet& p, PetEvent ev, unsigned long now) {
  petUpdate(p, now);   // make sure the mood is up to date first
  switch (ev) {
    case EV_PET:
    case EV_POKE:
      p.lastAttention = now;
      stopActivity(p);
      if (!isHungry(p, now)) p.happyUntil = now + HAPPY_MS;   // hungry cats want food, not pats
      break;
    case EV_BRUSH:
      p.lastAttention = now;
      stopActivity(p);
      if (!isHungry(p, now)) p.brushedUntil = now + BRUSH_MS;
      break;
    case EV_FEED:
      p.lastFed = now;
      p.lastAttention = now;
      p.eatingUntil = now + EAT_MS;
      p.happyUntil = 0;
      p.brushedUntil = 0;
      stopActivity(p);
      break;
    case EV_CLAP:
      if (CLAP_WAKES_CATS && p.mood == MOOD_SLEEPY) p.lastAttention = now;   // wakes up
      break;
    case EV_BUTTERFLY:
      if (petIsFree(p)) {
        p.activity = MOOD_WATCHING;
        p.activityUntil = now + BUTTERFLY_MS;
      }
      break;
  }
  petUpdate(p, now);
}

void petUpdate(Pet& p, unsigned long now) {
  if (p.activity != MOOD_IDLE && now >= p.activityUntil) stopActivity(p);

  if (now < p.eatingUntil)                                p.mood = MOOD_EATING;
  else if (now < p.brushedUntil)                          p.mood = MOOD_BRUSHED;
  else if (now < p.happyUntil)                            p.mood = MOOD_HAPPY;
  else if (isHungry(p, now))                              p.mood = MOOD_HUNGRY;
  else if (now - p.lastAttention > SLEEPY_AFTER_MS)       p.mood = MOOD_SLEEPY;
  else if (p.activity != MOOD_IDLE)                       p.mood = p.activity;
  else                                                    p.mood = MOOD_IDLE;
}

// Is it this cat's turn to groom or scratch by itself?
bool petWantsIdleAct(const Pet& p, unsigned long now) {
  return p.mood == MOOD_IDLE && now >= p.nextIdleAct;
}

// Groom or scratch by itself (doesn't count as attention), then pick the next gap
void petDoIdleAct(Pet& p, unsigned long now) {
  p.activity = random(2) ? MOOD_GROOMING : MOOD_SCRATCHING;
  p.activityUntil = now + IDLE_ACT_MS;
  p.nextIdleAct = p.activityUntil + PICK_GAP(IDLE_ACT_GAPS_MS);
  petUpdate(p, now);
}

// Start grooming or scratching now (for testing and filming).
// Wakes the cat too. A hungry cat still shows hungry.
void petStartActivity(Pet& p, Mood activity, unsigned long now) {
  p.lastAttention = now;
  p.happyUntil = 0;
  p.brushedUntil = 0;
  p.activity = activity;
  p.activityUntil = now + IDLE_ACT_MS;
  petUpdate(p, now);
}

bool petIsFree(const Pet& p) {
  return p.mood == MOOD_IDLE || p.mood == MOOD_GROOMING || p.mood == MOOD_SCRATCHING;
}

const char* moodName(Mood m) {
  switch (m) {
    case MOOD_HAPPY:      return "HAPPY";
    case MOOD_EATING:     return "EATING";
    case MOOD_BRUSHED:    return "BRUSHED";
    case MOOD_GROOMING:   return "GROOMING";
    case MOOD_SCRATCHING: return "SCRATCHING";
    case MOOD_WATCHING:   return "WATCHING";
    case MOOD_SLEEPY:     return "SLEEPY";
    case MOOD_HUNGRY:     return "HUNGRY";
    default:              return "IDLE";
  }
}
