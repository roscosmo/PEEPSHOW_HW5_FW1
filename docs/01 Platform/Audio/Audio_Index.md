# Audio Index

This section defines audio ownership, sampled speaker playback, procedural BBB output, DMA refill discipline, and power coordination.

## Core Notes

- [[Audio_Contract]]
- [[Subsystem_State_Machines]]
- [[Power_and_Sleep_Policy]]
- [[HW5_Hardware_Revision_Contract]]

## Boundary

The Engine and Reference Game may request symbolic music, SFX, and BBB behavior.

The Platform owns SAI1, GPDMA audio transfer, LPTIM1/BUZZ output, `SD_MODE`, mixer/decoder state, amp control, and sleep coordination.
