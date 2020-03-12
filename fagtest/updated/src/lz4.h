#ifndef _LZ4_H
#define _LZ4_H

// read one byte from input, see getByteFromIn()  for a basic implementation
typedef unsigned char (*GET_BYTE)  (void* userPtr);
// write several bytes,      see sendBytesToOut() for a basic implementation
typedef void          (*SEND_BYTES)(const unsigned char*, unsigned int, void* userPtr);

int unlz4(GET_BYTE getByte, SEND_BYTES sendBytes, void* userPtr);

#endif /* _LZ4_H */
