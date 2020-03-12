#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <errno.h>
#include <err.h>
#include <fcntl.h>

#include "misc.h"
#include "handler.h"

#include "updated.h"

/* Default values */
struct updated G =
  {.port    = 1999,
   .tmpdir  = "/tmp/",
   .uid     = 1000,
   .gid     = 1000,
   .logfile = "/var/log/updated.log",
   .keyfile = NULL,
   .timeout = 15 * 60,

   .onlysigned = 0,
   .onlycrypto = 0,
  };

void handle_SIGINT(int signum) {
  (void)signum;
  logmsg("Received SIGINT; shutting down\n");
  fclose(G.logf);
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
  int i;

  if (2 == argc && 0 == strcmp(argv[1], "--help")) {
    goto USAGE;
  }

  if (2 == argc && 0 == strcmp(argv[1], "--version")) {
    printf("UPDATEd v. " VERSION "\n");
    return EXIT_SUCCESS;
  }

#define OPTARG(name, conv)                                          \
  if (0 == memcmp("--" #name "=", argv[i], strlen(#name) + 3)) {    \
    G.name = conv(&argv[i][strlen(#name) + 3]);                     \
    continue;                                                       \
  }

#define OPTFLG(name)                            \
  if (0 == strcmp("--" #name, argv[i])) {       \
    G.name = 1;                                 \
    continue;                                   \
  }

  for (i = 1; i < argc; i++) {
    OPTARG(port,     atoi);
    OPTARG(tmpdir,   strdup);
    OPTARG(logfile,  strdup);
    OPTARG(keyfile,  strdup);
    OPTARG(uid,      atoi);
    OPTARG(gid,      atoi);
    OPTARG(timeout,  atoi);

    OPTFLG(onlysigned);
    OPTFLG(onlycrypto);

  USAGE:
    fprintf(stderr, "usage: %s [--FLAG=VALUE ...]\n", argv[0]);
    fprintf(stderr, "  --port=PORT (default: 1999):\n");
    fprintf(stderr, "    Listen for connections on PORT.\n");
    fprintf(stderr, "  --tmpdir=DIR (default: /tmp):\n");
    fprintf(stderr, "    Save temporary files in DIR.\n");
    fprintf(stderr, "  --logfile=FILE (default: /var/log/updated.log):\n");
    fprintf(stderr, "    Save log to FILE.\n");
    fprintf(stderr, "  --uid=UID, --gid=GID (default: 1000, 1000):\n");
    fprintf(stderr, "    Run as UID:GID.\n");
    fprintf(stderr, "  --timeout=SECONDS (default: 15*60):\n");
    fprintf(stderr, "    Abort unfinished updates after SECONDS seconds.\n");
    fprintf(stderr, "  --keyfile=FILE (default: none):\n");
    fprintf(stderr, "    Master key used for signing/encryption.\n");
    fprintf(stderr, "  --onlysigned:\n");
    fprintf(stderr, "    Accept only signed UPD images.  Recommended for "
            "devices connected to the Internet.\n");
    fprintf(stderr, "  --onlycrypto:\n");
    fprintf(stderr, "    Accept only encrypted UPD images.\n");
    return EXIT_FAILURE;
  }

  G.logf = fopen(G.logfile, "wa");
  if (!G.logf) {
    err(EXIT_FAILURE, "Could not open log file: %s", G.logfile);
  }

  if (G.keyfile) {
    chown(G.keyfile, G.uid, G.gid);
    chmod(G.keyfile, 0600);
    if (!read_file(G.keyfile, &G.key, &G.key_len)) {
      err(EXIT_FAILURE, "Could not open key file: %s", G.keyfile);
    }
  }

  signal(SIGINT, handle_SIGINT);

  logmsg("UpdateD started; listening on port %d\n", G.port);
  server(handler, G.port);

#undef OPTARG
#undef OPTFLG

  return EXIT_SUCCESS;
}
