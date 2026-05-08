Place compiled AppleSpamOnly firmware here as: firmware.bin

Build steps:
1. Open AppleSpamOnly/AppleSpamOnly.ino in Arduino IDE
2. Board: "ESP32 Dev Module" (same as Marauder generic build)
3. Compile
4. Copy the output .bin here as "firmware.bin"

Library required: NimBLE-Arduino
  -> Already in esp32_marauder/libraries/NimBLE-Arduino/
  -> Add that path in Arduino IDE: File > Preferences > Additional libraries

Offsets used by WebFlasher:
  0x1000  bootloader.bin   <- already here
  0x8000  partitions.bin   <- already here
  0xE000  boot_app0.bin    <- already here
  0x10000 firmware.bin     <- YOU MUST PROVIDE THIS
