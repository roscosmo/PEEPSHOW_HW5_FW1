# Engine API Index

The Engine sits between Platform and Game.

It owns reusable runtime abstractions, host lifecycle, package/content contracts, scene/runtime behavior, asset loading, and game-development APIs.

The Engine may understand gameplay concepts abstractly, but it must not depend on the Reference Game.

## Core Notes

- [[Runtime_Host_Contract]]
- [[Runtime_Host_Internal_State_Machines]]
- [[Package_Contract]]
- [[Package_Manager_State_Machine]]
- [[Asset_Pipeline_and_Package_Tooling_Contract]]

## Boundary

The Engine consumes Platform capabilities.

The Reference Game consumes Engine APIs.

The Engine must remain reusable for games other than the Reference Game.

