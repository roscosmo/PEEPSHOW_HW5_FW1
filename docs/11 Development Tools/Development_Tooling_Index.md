# Development Tooling Index

This section defines host-side and firmware-facing development tools for PeepShow.

Development tools support bring-up, package authoring, telemetry, live-safe tuning, replay, and developer workflows. They must not become hidden game APIs or bypass Platform ownership.

## Core Notes

- [[USB_Development_Mode_Contract]]
- [[Live_Tuning_And_Knobs_Contract]]
- [[Telemetry_And_Debug_Dashboard_Contract]]
- [[Tracealyzer_Snapshot_Evidence_Contract]]
- [[Knobs_and_Tuning_Contract]]
- [[Debug_and_Observability]]
- [[Debug_Workflows]]
- [[Digital_Twin_Host_Runtime_Contract]]
- [[Asset_Pipeline_and_Package_Tooling_Contract]]

## Tooling Lanes

Development tooling is split into four lanes:

| Lane | Purpose | Primary Interfaces |
|---|---|---|
| observation | understand what the device is doing | [[Telemetry_And_Debug_Dashboard_Contract]], SWO structured events, Tracealyzer snapshots, staged log export, optional CDC telemetry |
| control | change live-safe development settings | [[Live_Tuning_And_Knobs_Contract]] over dev-only CDC or digital twin adapters |
| authoring | produce valid content and packages | asset pipeline, package compiler, validators |
| replay | reproduce runtime behavior | digital twin, trace capture, deterministic input/time/sensor streams |

## Boundary

Tools may make development faster.

Tools must not:

- mutate Platform state through raw memory pokes as the normal workflow
- bypass owner-thread request paths
- expose hardware controls to package/game tools
- make USB MSC and firmware storage access share the same FAT volume at the same time
- treat digital twin or host tooling output as hardware bring-up evidence

## V1 Priorities

1. keep USB MSC as the normal user package-install/export workflow
2. use SWO for structured observation during bring-up
3. add USB CDC only as a mutually exclusive developer personality
4. make live tuning generated, typed, validated, and owner-routed
5. build dashboards and replay tooling from the same structured events used by the digital twin
