# NINA-B112 BLE Bring-up Runbook

This runbook records the measured HW5 procedure for bringing up the `NINA-B112-04B` BLE module.

Related:

- [[BLE_Communication_Contract]]
- [[Brought_Up_Tracker]]
- [[Debug_Workflows]]

---

## Scope

This runbook covers:

- reset and strap behavior, including avoidance of accidental bootloader/factory-reset entry
- `LPUART1` with RTS/CTS validation
- module boot-ready evidence
- command/profile discovery
- advertising
- connection
- pairing/bonding persistence
- bounded TX/RX
- suspend/resume behavior

---

## Preconditions

- `LPUART1` TX/RX/RTS/CTS pins validated
- NINA control pins mapped in CubeMX
- `thComm` owns all UART and NINA GPIO behavior
- bonding storage path through `thStorage` defined

---

## Control Pin Ledger

Record exact behavior once measured.

| Signal | Pin | Safe Default | Active Polarity | Required At Boot | Notes |
|---|---|---|---|---|---|
| `NINA_SW1` | `PC4` | high-Z / no-pull | config-dependent, do not drive low unless intentionally used | no, unless bootloader/default-reset behavior is intended | low with `SWITCH_2` during startup may enter bootloader; low with `SWITCH_2` held 10 s may factory reset |
| `NINA_SW2` | `PC5` | high-Z / no-pull | config-dependent, do not drive low unless intentionally used | no, unless UART-default, bootloader, connection, or factory-reset behavior is intended | low during startup restores UART defaults; can also be used for BLE connection behavior |
| `NINA_NRST` | `PC6` | low / reset asserted while BLE off | active low | yes, reset release is controlled by `thComm` | release only after SW1/SW2/DTR/DSR safe states are confirmed |
| `NINA_DTR` | `PC7` | high-Z / no-pull until configured | module-status indication, configuration-dependent | no | may indicate system mode, SPS peer, bonded BLE connection, or GAP connection depending on NINA config |
| `NINA_DSR` | `PC8` | high-Z / no-pull until configured | host control input, configuration-dependent | no | may enter command mode, disconnect/toggle connectable status, enable/disable UART, or wake/sleep depending on NINA config |

---

## Validation Procedure

1. Validate safe defaults for NINA GPIO without attempting communication: `NINA_NRST` asserted low, `NINA_SW1`/`NINA_SW2`/`NINA_DTR`/`NINA_DSR` high-Z/no-pull unless the test intentionally uses them.
2. Validate reset assertion/release timing, ensuring `NINA_SW1` and `NINA_SW2` are not accidentally driven low at release.
3. Validate `LPUART1` TX/RX at the CubeMX baseline baud rate.
4. Validate RTS/CTS behavior.
5. Capture module boot-ready evidence.
6. Discover or confirm the module command/profile interface.
7. Start and stop advertising.
8. Establish and drop a connection.
9. Pair/bond and persist bonding data through `thStorage`.
10. Reboot and confirm bonding data is reused or invalidated according to policy.
11. Run bounded TX/RX.
12. Quiesce and resume without stale UART or connection state.

---

## Evidence

Every successful validation must link evidence from [[Brought_Up_Tracker]].

Do not mark NINA reset, strap, UART, pairing, or transfer behavior known-good without measured HW5 evidence.