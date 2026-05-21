# Runtime Host Contract

This document defines platform-provided runtime hosts and their lifecycle API.

---

## Runtime Classes

- `SHELL`: built-in OS shell
- `LP_GRAPH`: low-power event/state driven runtime
- `LP_MODULE`: Engine-hosted low-power module with a predefined bounded loop shape
- `RT_SCENE`: higher activity runtime for richer scenes
- `INSTALLER`: package staging and install workflow

Runtimes are hosts, not game engines embedded in platform core.

---

## Lifecycle Contract

All runtimes must implement:
1. `mount`
2. `start`
3. `suspend`
4. `resume`
5. `stop`
6. `unmount`

No runtime switch is allowed without orderly lifecycle completion.

---

## Suggested Interface (C-Level)

```c
typedef enum {
    HOST_OK = 0,
    HOST_ERR_INVALID_STATE,
    HOST_ERR_DEPENDENCY,
    HOST_ERR_RESOURCE,
    HOST_ERR_INTERNAL
} host_result_t;

typedef struct {
    uint32_t package_id;
    uint32_t runtime_unit_id;
    uint32_t runtime_flags;
    const void *manifest;
} host_mount_args_t;

typedef struct {
    host_result_t (*mount)(const host_mount_args_t *args);
    host_result_t (*start)(void);
    host_result_t (*suspend)(void);
    host_result_t (*resume)(void);
    host_result_t (*stop)(void);
    host_result_t (*unmount)(void);
    void (*tick)(uint32_t now_ms);
} host_vtable_t;
```

---

## Runtime Logic Execution

Runtime hosts execute validated package logic through [[Runtime_Logic_State_API_Contract]].

Hosts may dispatch package-visible events, evaluate bounded state/action tables, and run approved realtime frame ticks for the active runtime unit.

Runtime logic execution must not expose RTOS threads, queues, timers, interrupts, or Platform hardware APIs to packages.

---

## Host To Platform Requests

Hosts may request:
- present scene or frame updates
- input focus scope activation through [[Input_Focus_API_Contract]]
- symbolic audio cue playback through [[Audio_API_Contract]]
- wake, lifecycle, timer, cadence, and power-intent hints through [[Time_And_Power_Intent_API_Contract]]
- package asset reads/views through [[Package_Asset_Loading_API_Contract]]
- communication sessions and bounded messages through [[Communication_API_Contract]]
- bounded package diagnostics through [[Diagnostics_API_Contract]]
- transition to another declared runtime unit through the runtime manager

Hosts may not:
- touch HAL handles directly
- change clocks or sleep mode directly
- mount/unmount storage volumes directly
- expose RTOS threads, queues, timers, or interrupts to packages
- transition to undeclared runtime units directly
- store package asset chunk offsets or storage addresses directly
- consume raw GPIO, EXTI, timer, I2C, joystick register, or debounce state directly

---

## Suspend/Resume Rules

- `suspend` must be bounded and idempotent.
- `resume` must revalidate dependencies and fail cleanly.
- Host state needed for resume must use explicit retained-state contracts.
- Resume failure must route to shell with a user-visible error.

---

## Failure Handling

Runtime failures must map to one of:
- recover in-place
- safe stop and return to shell
- force runtime unmount and package quarantine

Do not leave runtime manager in partial state.

---

## Power Intent Interface

Detailed package-facing rules are defined in [[Time_And_Power_Intent_API_Contract]].

Runtime expresses intent only:
- desired update cadence
- wake intents needed
- tolerance for latency
- active/idle hints

Runtime unit transitions must preserve this model. A realtime unit must return to a declared low-power unit or shell/system route according to its package manifest and power policy.

`RT_SCENE` has no fixed maximum active duration at this contract level. Platform inactivity timeout always applies, and realtime units must declare idle detection, suspend/resume behavior, and fallback routing.

Bounded interactive peer-wait grace is admitted only through [[Communication_API_Contract]] and target-profile policy. Runtime hosts must not treat an active communication session as an unlimited stay-awake grant.

Power manager maps intent to hardware policy.

---

## Host Compliance Checklist

A runtime host is compliant only if:
1. lifecycle methods are complete and bounded
2. no forbidden direct hardware access exists
3. suspend/resume tests pass
4. power intent is explicit
5. install/update interactions are safe
