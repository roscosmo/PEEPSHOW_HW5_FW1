# Reference Game Open Questions

This note tracks design questions that should be answered before implementation or content production.

## Engine/API Questions

- What is the reusable representation for ambient pet state?
- How are time-window events represented in packages?
- How does a package request a realtime microgame burst and return result?
- What is the common table format for NPC routines?
- What is the common table format for form social access?
- How are normalized sensor events exposed to game code?
- How are BBB patterns represented in package assets?

## Content Questions

- What are the first five forms?
- What is the first non-fake transformation event?
- What is the first midnight ritual the player can observe?
- Who are the first NPCs with real schedules?
- What is the first dungeon and what form/social rule does it teach?
- What is the first enemy-specific realtime microgame?

## Emotional Structure Questions

- When does the player first see the slime refuse something?
- What early behavior makes the slime feel like an agent?
- What small habit becomes familiar enough to matter after resurrection?
- What exact event can make the device feel empty without feeling like a cheap reset?

## Production Questions

- What tile size and map chunk format best fit the display and memory budgets?
- What portrait sizes are affordable?
- How many frames should common ambient animations use?
- What audio budget is reserved for music versus SFX versus BBB patterns?
- Which realtime microgames require preload guarantees?

## Rule

Do not answer these by adding hardware assumptions in Reference Game docs.

If a question touches Platform behavior, update the relevant Platform or Engine contract first.