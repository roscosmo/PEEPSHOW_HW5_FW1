# Realtime Microgame Catalog

Realtime microgames are short, high-intensity sequences used as punctuation.

They should remain rare enough to feel special.

## Design Rules

Each microgame should define:

- trigger
- fiction
- duration
- required capabilities
- input mapping
- success result
- failure result
- timeout result
- return mode

See [[Realtime_Microgame_Runtime]].

## Trigger Families

| Trigger Family | Example Use | Notes |
|---|---|---|
| critical hit | player or slime special | fast physical input with clear payoff |
| enemy lunge | first-person dodge | large enemy portrait, sudden timing window |
| parry | weapon-specific timing | should vary by weapon class |
| ritual | midnight rune tracing | emotional and atmospheric, not spammy |
| transformation | unstable form event | weird input pattern reflecting form pressure |
| environmental hazard | darkness, collapse, gust, balance | sensor-rich, short |
| social event | secret gathering participation | subtle, tense, timing-based |

## Hardware-Flavored Patterns

| Pattern | Capabilities | Example |
|---|---|---|
| rune tracing | rotary encoder, buttons, display | turn wheel through symbolic timing gates |
| balance | IMU, display | keep device steady during ritual or enemy attack |
| darkness cover | light sensor | cover sensor to hide from a light-sensitive enemy |
| dodge direction | joystick/buttons | choose direction during first-person lunge |
| rhythm | buttons/audio/BBB | match occult tones or creature calls |
| frantic spin | rotary encoder | splatter, charge, or overstimulate slime |

## Enemy-Specific Rule

Enemy types should have distinct realtime flavor.

A legendary weapon or major enemy should not reuse beginner timing unchanged.

## Boundary

Microgames consume normalized Engine/Platform events.

They do not configure peripherals directly.