#include <stdint.h>

float celsius_to_fahrenheit(float degC) { return degC * 1.8 + 32; }

float fahrenheit_to_celsius(float degF) { return (degF - 32.0) * (5.0 / 9.0); }

uint8_t sensirion_crc(uint8_t data[2]) {
  // Checksum generator
  //
  // CRC-8
  // 8 bit
  // polynomial 0x31 (x8 + x5 + x4 + 1)
  // Initialization 0xFF
  // Reflect input FALSE
  // Reflect output FALSE
  // Final xor 0x00
  // CRC (0xbe 0xef) = 0x92
  uint8_t crc = 0xFF;
  for (int i = 0; i < 2; i++) {
    crc ^= data[i];
    for (uint8_t bit = 8; bit > 0; --bit) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x31u;
      } else {
        crc = (crc << 1);
      }
    }
  }
  return crc;
}
