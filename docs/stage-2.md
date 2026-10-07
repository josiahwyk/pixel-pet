# Stage 2: moods, touch, button, mic and sound

## How it behaves
| Mood | What you see | What causes it |
|---|---|---|
| Idle | Breathes, swishes tail, blinks | Default |
| Happy | `^ ^` eyes, hops, fast tail wag, chirp | Pet or feed (lasts 3 s) |
| Sleepy | Eyes shut, slow breathing, "z Z" | No attention for 1 min |
| Hungry | Frown, flashing "!", sad tone once | Not fed for 2 min (left cat) or 3 min (right cat) |

Inputs:
- **Left cat (Cheddar) = touch module.** Press = pet (instant). Keep holding 3 s = feed (bowl + munch).
- **Right cat (Halloumi) = button.** Press = pet. Keep holding 3 s = feed.
- **Loud noise** (clap, talk loudly near the mic) wakes sleepy cats.
- Hungry cats ignore pats. They want food.
- Timers are short for testing. Change them in `pet_config.h` before gifting.

## Parts
- Stage 1 setup (board, shield, OLED, still wired)
- Capacitive touch module, digital push button module, analog sound sensor, passive buzzer module (all from the 48-in-1 kit)
- 12 more female-to-female jumper wires (3 per module)

## Wiring
Unplug USB first. Each module has 3 pins, usually labelled G, V (or +), S. Match each one to the same-coloured row on the shield:
- G goes to G (black)
- V goes to 3.3V (red)
- S goes to S (blue)

| Module | Module pin | Shield row |
|---|---|---|
| Capacitive touch | G / V / S | **IO27** row: G / 3.3V / S |
| Push button | G / V / S | **IO26** row: G / 3.3V / S |
| Analog sound sensor | G / V / S | **IO34** row: G / 3.3V / S |
| Passive buzzer | G / V / S | **IO25** row: G / 3.3V / S |

If the sound sensor has 4 pins (A0, G, +, D0), use **A0** as S and leave D0 empty.
The mic must stay on IO34. Analog reads on the IO25–27 pins stop working once Wi-Fi is on (Stage 3).

## Test checklist
Claude uploads and watches the Serial Monitor.
- [ ] Start-up prints `Touch rest level` and `Button rest level`. Don't touch anything while it boots.
- [ ] Tap touch: left cat hops with `^ ^` and chirps. Right cat does nothing.
- [ ] Press button: right cat hops and chirps.
- [ ] Hold touch 3 s: bowl appears, cat chews, munch sound.
- [ ] Leave it alone 1 min: both cats show "z Z".
- [ ] Clap near the mic: sleepy cats wake up with a chirp.
- [ ] Wait 2 min without feeding the left cat: frown, flashing "!", sad tone.
- [ ] Hold touch to feed it: back to happy.
- [ ] Unplug and run from the wall charger: same behaviour.

## If it doesn't work
| Symptom | Try |
|---|---|
| A cat chirps over and over by itself | That input is floating or was touched at boot. Restart without touching anything. Check the S wire. |
| Touch/button does nothing | Check the S wire is on the right IO row. Check the rest level print: it should change when pressed. |
| Clapping doesn't wake them | Set `DEBUG_MIC 1` in `pet_config.h` and read the levels. Set `NOISE_THRESHOLD` between quiet and clap. Some modules have a small screw (potentiometer) for sensitivity. |
| Cats wake up all the time | Raise `NOISE_THRESHOLD`. |
| No sound | Make sure it's the **passive** buzzer module (the active one only beeps one note). Check IO25. |
| Screen stops working after adding modules | A power wire may be on 5V or shorted. Unplug and recheck the G/V rows. |
