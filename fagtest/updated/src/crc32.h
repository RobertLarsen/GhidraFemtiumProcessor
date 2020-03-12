#ifndef _CRC32_H
#define _CRC32_H

#include <stdlib.h>

unsigned int crc32(void *data, size_t numb);
unsigned int crc32x(unsigned int crc, void *data, size_t numb);

#endif /* _CRC32_H */
