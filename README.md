
# Microwave Oven Firmware

A PIC microcontroller-based microwave oven firmware with a modular architecture and a screen-state finite state machine driving the UI and cooking logic.

## Overview

- Target: PIC microcontroller @ 20MHz (XC8 toolchain)
- Display: 16x4 Character LCD in 4-bit mode
- Input: 4x3 Matrix Keypad (STATE and LEVEL scanning)
- Core Pattern: Screen-state finite state machine (FSM)

## Folder Structure

| Path                                  | Purpose                                                                                                                             |
|---------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------|
| _build                                | CMake build tree (Ninja). Safe to delete; will be regenerated.                                                                      |
| cmake                                 | CMake project and presets for default and production configurations.                                                                 |
| .vscode                               | VS Code workspace settings and MPLAB project integration.                                                                           |
| out                                   | Final build artifacts (firmware images).                                                                                            |

## Hardware Mapping

- LCD Data: `PORTD` (RD7–RD4)
- LCD Control: `RE2` (RS), `RE1` (EN)
- Keypad Rows: `RD3–RD0` (outputs, pulled LOW to scan)
- Keypad Columns: `RB2–RB0` (inputs, pullups enabled)
- FAN: `RC2` (output, controlled during cooking/preheating)

## Build Instructions

Requires CMake, Ninja, and XC8 toolchain configured by presets.

- Default build:
```bash
cmake -S cmake/Microwave_Firmware/default -B _build/Microwave_Firmware/default -G Ninja
ninja -C _build/Microwave_Firmware/default
```
- Production build:
```bash
cmake -S cmake/Microwave_Firmware/default.production -B _build/Microwave_Firmware/default.production -G Ninja
ninja -C _build/Microwave_Firmware/default.production
```
Artifacts will appear under the respective `_build/.../` folders and `out/` if configured.

## Operating Modes

- 1 Micro: Show power (900W) → Time input (MM:SS) → Cooking countdown
- 2 Grill: Show power (1200W) → Time input (MM:SS) → Cooking countdown
- 3 Convection: Temperature input (°C) → Preheating countdown → Time input → Cooking countdown
- 4 Start: Quick start at 30 seconds; while running, key `4` adds +30 seconds

### Cooking Controls (shared)
- `4`: Start/Resume when paused; if running, add +30 seconds
- `5`: Pause
- `6`: Stop and return to main menu
- `*`: Clear (only during time/temp input)
- `#`: Confirm (only during time/temp input)

## State Machine Flow

- POWER_SCREEN → MODES_SCREEN
- MICRO_MODE → TIME_INPUT → COOKING_MODE
- GRILL_MODE → TIME_INPUT → COOKING_MODE
- CONVECTION_MODE → TEMP_INPUT → PREHEAT_COUNTDOWN → TIME_INPUT → COOKING_MODE
- QUICK_START_MODE → COOKING_MODE

FSM uses pre-draw before switching states to minimize LCD flicker, and sparse updates (only redraws time when it changes).

## Keypad Scanning

- Modes: `STATE` (debounced, one event per press) and `LEVEL` (raw continuous)
- Mapping:
  - Row1: `1`, `2`, `3`
  - Row2: `4`, `5`, `6`
  - Row3: `7`, `8`, `9`
  - Row4: `*`, `0`, `#`

## LCD Driver

- 4-bit mode: two nibbles per character/instruction
- Critical timing: 4.1ms initialization wait, 100µs between nibbles
- Addressing macros: `LINE1(x)`..`LINE4(x)` for DDRAM positions
- API:
  - `clcd_putch()` put one character at address
  - `clcd_print()` print string starting at address
  - `clcd_write()` low-level nibble transmission

## Timer2 ISR

- ISR fires on Timer2 interrupt; counts to 1250 ticks ≈ 1 second at 20MHz with 1:16 prescale and `PR2=250`
- Volatile globals: `timer2_counter`, `total_seconds`
- Timer2 remains OFF until cooking begins; controlled via `TMR2ON` in FSM

## Development Notes

- Cursor handling: Time/Temp input uses digit highlighting via redraw; cursor is hidden on confirm `#`
- Flicker mitigation: Pre-draw screens on entry and only update changing fields during countdown
- Convection preheat: Placeholder 10 seconds; can be tied to temperature in future
- No persistent settings yet; EEPROM could store favorites
- Audio/fan not integrated; hardware blocks exist

## Files

- [main.c](main.c): Entry point and FSM orchestration; keypad, cooking control, sparse LCD updates
- [oven.h](oven.h): Screen state macros, UI function declarations
- [oven.c](oven.c): UI rendering: power banners, time/temp input, countdown display
- [clcd.h](clcd.h) / [clcd.c](clcd.c): Character LCD HAL in 4-bit mode
- [matrix_keypad.h](matrix_keypad.h) / [matrix_keypad.c](matrix_keypad.c): Matrix keypad HAL with STATE/LEVEL scanning
- [timers.h](timers.h) / [timers.c](timers.c): Timer2 initialization and ISR integration

## Quick Test

- Flash the compiled binary to the PIC using MPLAB or your setup
- On boot, verify the power screen splash, then modes menu
- Try `4` for Quick Start (30s), add time via `4` while running; pause `5`, stop `6`
- Verify mode flows for Micro (1), Grill (2), and Convection (3)
