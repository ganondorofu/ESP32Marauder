# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This is an **Arduino IDE sketch** — there is no Makefile or CMake. The main sketch is `esp32_marauder/esp32_marauder.ino`.

**To build**: Open `esp32_marauder/esp32_marauder.ino` in Arduino IDE with the ESP32 board package installed. Dependencies are git submodules under `esp32_marauder/libraries/`.

Key libraries:
- `NimBLE-Arduino` — BLE advertising/scanning
- `TFT_eSPI` — display rendering
- `ArduinoJson`, `LinkedList`, `Adafruit_NeoPixel`

There is no automated test runner. `BleSpamTest/BleSpamTest.ino` is a standalone sketch for manually testing BLE spam payloads outside the full Marauder framework.

## Board Target Configuration

`esp32_marauder/configs.h` controls which hardware is compiled for. Exactly **one** board `#define` must be active (all are commented out by default — uncomment the target before building):

```cpp
//#define MARAUDER_CARDPUTER
//#define MARAUDER_MINI
//#define GENERIC_ESP32
// ... etc.
```

Hardware capability flags (`HAS_BT`, `HAS_SCREEN`, `HAS_SD`, etc.) are derived from the board target inside `configs.h`.

## Architecture

### Feature Dispatch Pattern

WiFi and BLE features are identified by integer constants defined in `WiFiScan.h` (e.g., `BT_ATTACK_SOUR_APPLE`, `WIFI_ATTACK_DEAUTH`). The flow is:

1. Menu item in `MenuFunctions.cpp` calls `wifi_scan_obj.StartScan(MODE_CONSTANT, color)`
2. `WiFiScan::StartScan()` dispatches to a `Run*()` method via if-else chain
3. The `Run*()` method loops and is driven by `WiFiScan::main(currentTime)` in the main sketch loop

**To add a new attack/scan**:
- Define a new constant in `WiFiScan.h`
- Add a `Run*()` method to `WiFiScan`
- Wire it into the `StartScan()` dispatcher in `WiFiScan.cpp`
- Add a menu node in `MenuFunctions.cpp` via `addNodes()`

### Key Files

| File | Role |
|------|------|
| `esp32_marauder.ino` | Setup/loop, jumper-triggered auto-spam on boot |
| `WiFiScan.cpp` / `.h` | All WiFi and BLE attack/scan logic (largest file ~386 KB) |
| `MenuFunctions.cpp` | UI menu tree, feature registration |
| `CommandLine.cpp` | Serial CLI interface |
| `configs.h` | Board targets, version string, hardware flags |
| `Settings.cpp` | NVS preference persistence |
| `GpsInterface.cpp` / `.h` | GPS for wardriving/location tagging |

### BLE Spam

`WiFiScan::executeBLESpam(EBLEPayloadType type)` handles all BLE spam. It:
- Forces TX power to max (`ESP_PWR_LVL_P9`) on all three NimBLE power slots
- Uses random static BLE addresses with top 2 bits forced (`rmac[5] |= 0xC0`)
- Bursts 6 advertisements per call
- Rotates MAC address each burst

Payload types (`EBLEPayloadType` enum in `WiFiScan.h`): Apple, Samsung, Google FastPair, Microsoft SwiftPair, FlipperZero, AirTag, and composite `BT_ATTACK_SPAM_ALL`.

### Jumper Auto-Spam (Boot Trigger)

Defined in `esp32_marauder.ino` setup():
- GPIO13 + GND → Apple spam (`BT_ATTACK_SOUR_APPLE`)
- GPIO12 + GND → Spam All (`BT_ATTACK_SPAM_ALL`)
