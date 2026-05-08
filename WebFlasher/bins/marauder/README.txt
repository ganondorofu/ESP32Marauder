Place compiled Marauder firmware here as: firmware.bin

Build steps:
1. Open esp32_marauder/esp32_marauder.ino in Arduino IDE
2. Select target board and compile
3. Find the output .bin (Arduino IDE shows path in build output)
4. Copy it here as "firmware.bin"

Offsets used by WebFlasher:
  0x1000  bootloader.bin   <- already here
  0x8000  partitions.bin   <- already here
  0xE000  boot_app0.bin    <- already here
  0x10000 firmware.bin     <- YOU MUST PROVIDE THIS
