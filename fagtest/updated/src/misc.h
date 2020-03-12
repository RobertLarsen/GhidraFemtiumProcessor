#ifndef _MISC_H
#define _MISC_H

#include <stdlib.h>

int do_read(int fd, void *buf, size_t len);
int do_write(int fd, const void *buf, size_t len);
int readn(int fd, void *buf, size_t len);
int writen(int fd, const void *buf, size_t len);

int read_file(const char *path, unsigned char **bufout, size_t *lenout);
void *mmap_file(const char *path, int flags);

void server(void (*handler)(void), unsigned short int port);

void logmsg(char *fmt, ...);

#endif /* _MISC_H */
