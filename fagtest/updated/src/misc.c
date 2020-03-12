#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <err.h>

#include "updated.h"

#include "misc.h"

void *mmap_file(const char *path, int flags) {
  int fd;
  struct stat st;
  void *ret;

  fd = open(path, flags);
  if (-1 == fd) {
    return NULL;
  }

  if (-1 == fstat(fd, &st)) {
    close(fd);
    return NULL;
  }

  ret = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  return ret;
}

int read_file(const char *path, unsigned char **bufout, size_t *lenout) {
  int fd, ret;
  unsigned char *buf;
  size_t i, len;

  fd = open(path, O_RDONLY);
  if (-1 == fd) {
    return 0;
  }

  buf = NULL;
  i = len = 0;
  for (;;) {
    if (i == len) {
      len += 0x1000;
      buf = realloc(buf, len);
      if (!buf) {
        return 0;
      }
    }
    ret = do_read(fd, &buf[i], len - i);
    if (-1 == ret) {
      close(fd);
      return 0;
    }
    if (0 == ret) {
      close(fd);
      break;
    }
    i += ret;
  }

  /* Only shrinking, guaranteed to succeed */
  *bufout = realloc(buf, i);
  *lenout = i;
  return 1;
}

int do_read(int fd, void *buf, size_t len) {
  int ret;
  do {
    ret = read(fd, buf, len);
  } while (-1 == ret && EINTR == errno);
  return ret;
}

int do_write(int fd, const void *buf, size_t len) {
  int ret;
  do {
    ret = write(fd, buf, len);
  } while (-1 == ret && EINTR == errno);
  return ret;
}

int readn(int fd, void *buf, size_t len) {
  size_t i;
  int ret;

  for (i = 0; i < len;) {
    ret = do_read(fd, buf + i, len - i);
    if (ret <= 0) {
      return 0;
    }
    i += ret;
  }

  return 1;
}

int writen(int fd, const void *buf, size_t len) {
  size_t i;
  int ret;

  for (i = 0; i < len;) {
    ret = do_write(fd, buf + i, len - i);
    if (ret <= 0) {
      return 0;
    }
    i += ret;
  }

  return 1;
}


void logmsg(char *fmt, ...) {
  char ts[128];
  va_list ap;
  time_t now;
  struct tm *ti;

  va_start(ap, fmt);

  if (-1 == time(&now)) {
    perror("time");
    exit(EXIT_FAILURE);
  }

  ti = localtime(&now);
  if (0 == strftime(ts, sizeof(ts), "%F %T", ti)) {
    fprintf(stderr, "strftime(): error\n");
    exit(EXIT_FAILURE);
  }

  fprintf(G.logf, "[%5d %s] ", getpid(), ts);
  vfprintf(G.logf, fmt, ap);

  va_end(ap);
}

void lograw(char *s) {
  fputs(s, G.logf);
}

static
int do_listen(unsigned short int port) {
  int sock, yes = 1;
  struct sockaddr_in saddr;

  memset(&saddr, 0, sizeof saddr);
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = INADDR_ANY;

  if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt()");
    exit(EXIT_FAILURE);
  }

  if (bind(sock, (struct sockaddr *)&saddr, sizeof saddr) == -1) {
    close(sock);
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(sock, 100) == -1) {
    perror("listen()");
    exit(EXIT_FAILURE);
  }

  /* Reap children */
  signal(SIGCHLD, SIG_IGN);

  return sock;
}

int do_fork() {
  pid_t pid;

  /* Make sure buffers are empty so buffered output is not "inherited". */
  fflush(stdout);
  fflush(stderr);
  fflush(G.logf);

  pid = fork();
  if (-1 == pid) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  return pid;
}

void teelog(int sock) {
  int fds[2], ret;
  char buf[0x1000];

  if (-1 == pipe(fds)) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  /* Enable line buffering before `dup2`ing. */
  setlinebuf(stdout);
  setlinebuf(stderr);

  if (do_fork()) {
    /* Child */

    /* Close write end */
    close(fds[1]);

    /* Proxy data */
    for (;;) {
      /* Read data */
      ret = do_read(fds[0], buf, sizeof(buf) - 1);
      if (-1 == ret) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      if (0 == ret) {
        break;
      }

      /* Log */
      buf[ret] = 0;
      logmsg(buf);
      if ('\n' != buf[ret - 1]) {
        lograw(" ...\n");
      }

      /* Write output from buffer */
      if (!writen(sock, buf, ret)) {
        err(EXIT_FAILURE, "write");
      }
    }

    exit(EXIT_SUCCESS);
  } else {
    /* Parent */

    /* Close read end */
    close(fds[0]);

    /* Redirect stdout/stderr */
    dup2(fds[1], STDOUT_FILENO);
    dup2(fds[1], STDERR_FILENO);
    close(fds[1]);
  }
}

void server(void (*handler)(void), unsigned short int port) {
  struct sockaddr_in caddr;
  int sock, nsock, counter;
  unsigned int nsocklen;

  sock = do_listen(port);
  srand(getpid() + time(0));
  counter = rand();

  nsocklen = sizeof(caddr);
  for (;;) {
    if ((nsock = accept(sock, (struct sockaddr *)&caddr, &nsocklen)) == -1) {
      perror("accept");
      continue;
    }
    logmsg("Connection from %d.%d.%d.%d:%d\n",
           ((unsigned char*)&caddr.sin_addr)[0],
           ((unsigned char*)&caddr.sin_addr)[1],
           ((unsigned char*)&caddr.sin_addr)[2],
           ((unsigned char*)&caddr.sin_addr)[3],
           ntohs(caddr.sin_port)
           );
    counter = rand();

    if (do_fork()) {
      /* Parent */
      close(nsock);

    } else {
      /* Child */
      alarm(G.timeout);
      srand(counter);
      close(sock);

      /* Redirect stdout/stderr to logfile and nsock */
      teelog(nsock);

      /* Redirect nsock to stdin */
      dup2(nsock, STDIN_FILENO);
      close(nsock);

      /* Handle update */
      handler();

      exit(EXIT_SUCCESS);
    }
  }
}
