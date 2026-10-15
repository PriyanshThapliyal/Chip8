# CHIP-8 / SCHIP Emulator

A fast, cross-platform CHIP-8 and Super-CHIP (SCHIP) emulator written in C++17 using [SDL2](https://www.libsdl.org/) for graphics, audio, and input handling.

---

## Features

- **Accurate Instruction Cycle**: Full support for standard CHIP-8 opcodes with proper standard sprite clipping and arithmetic flag ordering.
- **Super-CHIP (SCHIP) Extensions**:
  - Extended high-resolution graphics mode ($128 \times 64$) alongside standard mode ($64 \times 32$).
  - $16 \times 16$ sprite rendering (`DXY0`).
  - Hardware scrolling instructions: Scroll Down (`00CN`), Scroll Up (`00DN`), Scroll Right (`00FB`), and Scroll Left (`00FC`).
- **Dynamic Color Themes**: Cycle through retro color palettes at runtime (Classic CRT Monochrome, Phosphor Green, Amber, Game Boy 1989, and Nord Frost).
- **Sound Generation**: Integrated $440\text{ Hz}$ audio buzzer synthesized via SDL2 audio callback.
- **Configurable Speed**: Adjust CPU execution speed (cycles per frame) on-the-fly or via command-line arguments.
- **Cross-Platform**: Ready to build on Linux, macOS, and Windows.

---

## Controls

### Keypad Mapping
The original COSMAC VIP 16-key hexadecimal keypad is mapped to standard QWERTY keyboard keys:

```text
CHIP-8 Keypad          Modern Keyboard
+---+---+---+---+     +---+---+---+---+
| 1 | 2 | 3 | C |     | 1 | 2 | 3 | 4 |
+---+---+---+---+     +---+---+---+---+
| 4 | 5 | 6 | D | --> | Q | W | E | R |
+---+---+---+---+     +---+---+---+---+
| 7 | 8 | 9 | E |     | A | S | D | F |
+---+---+---+---+     +---+---+---+---+
| A | 0 | B | F |     | Z | X | C | V |
+---+---+---+---+     +---+---+---+---+
```

### Hotkeys & Emulation Controls

| Key | Action |
| :--- | :--- |
| **`K`** | Cycle visual theme / palette |
| **`P`** | Pause / Resume emulation |
| **`O`** | Reset current game |
| **`=` / `+`** | Increase CPU speed (+5 cycles/frame) |
| **`-`** | Decrease CPU speed (-5 cycles/frame) |
| **`Esc`** | Exit emulator |

---

## Prerequisites

You need a C++17 compatible compiler (`g++` or `clang++`) and **SDL2 development libraries**.

### Linux (Debian / Ubuntu / Pop!_OS)
```bash
sudo apt update
sudo apt install build-essential libsdl2-dev
```

### Linux (Arch / Manjaro)
```bash
sudo pacman -S base-devel sdl2
```

### macOS (via Homebrew)
```bash
brew install sdl2
```

### Windows (MinGW-w64 / MSYS2)
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 make
```

---

## Building and Running

### 1. Build
Compile using the cross-platform `Makefile`:

```bash
make
```

To clean previous build artifacts:
```bash
make clean
```

### 2. Run
Launch the emulator by passing the path to any `.ch8` ROM:

```bash
./chip8 <path/to/rom.ch8> [cycles_per_frame]
```

**Examples:**
```bash
# Play Tank with default speed (5 cycles/frame)
./chip8 roms/tank.ch8

# Play Danm8ku with higher speed (15 cycles/frame)
./chip8 roms/danm8ku.ch8 15

# Run the test suite
./chip8 test/5-quirks.ch8
```

---

## Directory Structure

```text
├── Makefile          # Cross-platform build script (Linux, macOS, Windows)
├── main.cpp          # Main loop, timing (60 Hz timers), and entry point
├── chip8.cpp         # CPU registers, memory, timers, opcode decoding
├── LICENSE           # MIT License
├── chip8.hpp         # Chip8 class definition
├── display.cpp       # SDL2 windowing, texture rendering, audio, and input
├── display.hpp       # Display class and theme structures
├── roms/             # Sample games (Danm8ku, Tank, Outlaw, etc.)
└── test/             # Timendus CHIP-8 test suite ROMs
```

---

## Included Test Suite

The `test/` directory contains tests from the [Timendus CHIP-8 test suite](https://github.com/Timendus/chip8-test-suite):
- `1-chip8-logo.ch8` – Basic display test
- `2-ibm-logo.ch8` – IBM splash screen test
- `3-corax+.ch8` – Opcode execution verification
- `4-flags.ch8` – Math operations & VF carry flag test
- `5-quirks.ch8` – Interpreter quirk compliance test
- `6-keypad.ch8` – Hex keypad input test
- `7-beep.ch8` – Sound timer and buzzer test
- `8-scrolling.ch8` – Super-CHIP screen scrolling test

---

## Included Roms

The 'roms/' directory contains roms taken from the [JohnEarnest CHIP-8 Archive](https://github.com/JohnEarnest/chip8Archive)

## License

This project is licensed under the [MIT License](LICENSE).

