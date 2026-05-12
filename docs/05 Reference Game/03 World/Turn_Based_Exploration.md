# Turn Based Exploration

The main world uses turn-based tile movement and low-power world updates.

## Turn Loop

```text
Player moves or acts
-> slime moves, acts, follows, refuses, or splits
-> enemies and world systems update
-> changed regions redraw
```

## Overworld

The overworld may include:

- towns
- forests
- ruins
- graveyards
- roads
- castles
- wilderness
- hidden areas

It should encourage wandering, observing, following, revisiting, and temporal investigation.

## Dungeons

Dungeons contain:

- enemies
- forms
- equipment
- rituals
- traversal challenges
- hidden social or ecological systems

The player ventures with the slime companion and uses tactical form interactions.

## Progression

Progression should include Zelda/Metroid-style access changes, but forms should also carry emotional and social meaning.

See [[Transformation_and_Form_System]].

## Runtime Link

This is primarily part of [[Static_Exploration_Runtime]].