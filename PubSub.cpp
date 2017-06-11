#include "PubSub.h"

char *addressFromInt(uint32_t addr) {
  char address[16];
  sprintf(address, "%i.%i.%i.%i",
          (addr & 0xff000000) >> 24,
          (addr & 0x00ff0000) >> 16,
          (addr & 0x00000ff00) >> 8,
          addr & 0x000000ff);
  return address;
}
