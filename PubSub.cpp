#include "PubSub.h"

char *addressFromInt(uint32_t addr) {
  char *address = new char[16];

  Serial.print("Address before filling: ");
  for (int i = 0; i < 16; i++) {
    Serial.print(address[i]);
  }
  Serial.println("");

  memset(address, 0, 16);
  sprintf(address, "%i.%i.%i.%i",
          (addr & 0x000000ff),
          (addr & 0x0000ff00) >> 8,
          (addr & 0x00ff0000) >> 16,
          (addr & 0xff000000) >> 24);

  Serial.print("Address after filling: ");
  for (int i = 0; i < 16; i++) {
    Serial.print(address[i]);
  }
  Serial.println("");

  return address;
}
