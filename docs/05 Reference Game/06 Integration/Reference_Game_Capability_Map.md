# Reference Game Capability Map

This note maps Reference Game design needs to Engine and Platform contracts.

The Reference Game expresses intent. It does not own hardware behavior.

## Capability Matrix

| Reference Game Need | Game Notes | Engine / Platform Contract |
|---|---|---|
| always-visible creature presence | [[Living_Artifact_and_Persistence]], [[Ambient_Pet_Runtime]] | [[Display_and_Rendering_Contract]], [[Power_and_Sleep_Policy]], [[Runtime_Host_Contract]] |
| low-power ambient simulation | [[Ambient_Pet_Runtime]], [[Pet_Simulation_and_Hidden_State]] | [[Runtime_Host_Contract]], [[Package_Contract]], [[Power_and_Sleep_Policy]] |
| turn-based exploration | [[Static_Exploration_Runtime]], [[Turn_Based_Exploration]] | [[Runtime_Host_Contract]], [[Display_and_Rendering_Contract]], [[Joystick_Hall_Input_Contract]], [[Button_Input_Contract]] |
| short realtime bursts | [[Realtime_Microgame_Runtime]], [[Realtime_Microgame_Catalog]] | [[Runtime_Host_Contract]], [[Power_and_Sleep_Policy]], [[Audio_Contract]] |
| petting/scratching/wheel rituals | [[Living_Artifact_and_Persistence]], [[Death_and_Resurrection_Ritual]] | [[Rotary_Encoder_Input_Contract]] |
| direction/navigation/tactical choice | [[Turn_Based_Exploration]], [[Combat_and_Split_Slime]] | [[Joystick_Hall_Input_Contract]], [[Button_Input_Contract]] |
| darkness, hiding, vampire/nocturnal mechanics | [[Transformation_and_Form_System]], [[Realtime_Microgame_Catalog]] | [[Light_Sensor_Contract]] |
| carrying, shaking, balancing, step-aware behavior | [[Pet_Simulation_and_Hidden_State]], [[Realtime_Microgame_Catalog]] | [[IMU_Contract]], [[Power_and_Sleep_Policy]] |
| low battery and charging as care | [[Living_Artifact_and_Persistence]], [[Pet_Simulation_and_Hidden_State]] | [[PMIC_and_Power_Contract]] |
| music, SFX, BBB rituals | [[Art_Audio_and_Presentation]], [[Realtime_Microgame_Catalog]] | [[Audio_Contract]] |
| persistent relationship, forms, schedules, saves | [[Slime_Relationship_Model]], [[Transformation_and_Form_System]], [[Temporal_World_and_NPC_Routines]] | [[Storage_and_Installer_Contract]], [[Package_Contract]] |
| multiplayer or companion-app demonstration | [[Reference_Game_Structural_Overview]] | [[BLE_Communication_Contract]] |
| package install and content validation | [[Art_Audio_and_Presentation]] | [[Asset_Pipeline_and_Package_Tooling_Contract]], [[Package_Contract]] |
| fatal absence or empty-device emotional beat | [[Death_and_Resurrection_Ritual]] | [[Display_and_Rendering_Contract]], [[Boot_and_Fault_Supervisor_State_Machine]] |

## Engine API Requirements Discovered

These are Reference Game needs that should be converted into reusable Engine/API contracts before implementation:

- retained ambient pet state model
- runtime time-window event scheduling
- normalized sensor event subscription
- microgame lifecycle wrapper
- dirty-region scene presentation API
- bounded audio cue and BBB pattern request API
- package-safe save schema for relationship/form/world state
- NPC routine table format
- ritual event table format
- form capability and social-access table format

## Platform Authority Reminder

If a capability requires new hardware policy, update Platform docs first.

Game notes may request capabilities but may not define peripheral ownership, sleep depth, clock policy, DMA behavior, or storage ownership.