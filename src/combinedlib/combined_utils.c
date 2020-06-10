#include "combined_utils.h"
uint32_t ror(uint32_t value, uint32_t shift) {
  uint32_t shifted, rotated;
  shift %= 32;

  shifted = value >> shift;
  rotated = value << (32 - shift);
  return shifted | rotated;
}
