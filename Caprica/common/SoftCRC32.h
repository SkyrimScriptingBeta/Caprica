#pragma once
#include <cstdint>

// Software CRC32C fallback for platforms without SSE4.2 CRC32 intrinsics (e.g. WASM).
// These match the _mm_crc32_u8/u16/u32 intrinsic signatures.

namespace caprica { namespace detail {

inline uint32_t crc32c_byte(uint32_t crc, uint8_t data) {
  crc ^= data;
  for (int i = 0; i < 8; i++)
    crc = (crc >> 1) ^ (0x82F63B78U & -(crc & 1));
  return crc;
}

}}

inline uint32_t _mm_crc32_u8(uint32_t crc, uint8_t v) {
  return caprica::detail::crc32c_byte(crc, v);
}

inline uint32_t _mm_crc32_u16(uint32_t crc, uint16_t v) {
  crc = caprica::detail::crc32c_byte(crc, (uint8_t)(v));
  crc = caprica::detail::crc32c_byte(crc, (uint8_t)(v >> 8));
  return crc;
}

inline uint32_t _mm_crc32_u32(uint32_t crc, uint32_t v) {
  crc = caprica::detail::crc32c_byte(crc, (uint8_t)(v));
  crc = caprica::detail::crc32c_byte(crc, (uint8_t)(v >> 8));
  crc = caprica::detail::crc32c_byte(crc, (uint8_t)(v >> 16));
  crc = caprica::detail::crc32c_byte(crc, (uint8_t)(v >> 24));
  return crc;
}
