#include "crc32.h"

unsigned int crc32(void *data, size_t numb) {
  return crc32x(0, data, numb);
}

unsigned int crc32x(unsigned int crc, void *data, size_t numb) {
   unsigned int i, j, byte, mask;

   crc = ~crc;
   for (i = 0; i < numb; i++) {
     byte = ((unsigned char*)data)[i];
     crc = crc ^ byte;
     for (j = 0; j < 8; j++) {
       mask = -(crc & 1);
       crc = (crc >> 1) ^ (0xEDB88320 & mask);
     }
   }
   return ~crc;
}
