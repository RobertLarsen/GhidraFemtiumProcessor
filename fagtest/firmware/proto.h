#ifndef DING_PROTO_H
#define DING_PROTO_H

#include <stdint.h>

/********************************************************************************
 ** Binary Enterprise Event Protocol
 **
 **     contact@ibm.local
 **
 **    Copyright(C) 1970-2020
 ** International Bongo Machines
 ********************************************************************************/

#define BEEP_PORT 48873

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

enum beep_req_type {
    BEEP_PING = 0x10,
    BEEP_LEN  = 0x20,
    BEEP_GET  = 0x30,
    BEEP_DEL  = 0x40,
    BEEP_ADD  = 0x50,
    BEEP_UPT  = 0x60,
    BEEP_TOT  = 0x70,
};

enum beep_resp_type {
    BEEP_YEAH = 0, /* Things went well */
    BEEP_NOPE = 1, /* Things went less well */
};

struct beep_event {
    /* seconds since epoch. There's no way people are still using
       these things in 2038... */
    i32 date;

    /* Hash of the unique fingerprint pattern. It could be described
       as a fingerprint fingerprint. */
    u8 hash[16];
};

/* BEEP request packet */
struct beep_req {
    /* one of enum beep_req_type */
    u16 type;

    /* index of queue item to BEEP_GET or BEEP_DEL */

    /* 2017-12-12: john@ibm.local
       FIXME: Did we end up using this for BEEP_ADD as well? */

    /* 2018-04-20: noah@ibm.local
       NO WAY - using the .index field for length was a terrible hack!

       If we need to rename this field, please submit a change request
       with your project manager. Use form #OG4D, "Late-Stage protocol
       change request for deployed customer-facing product", and
       remember to send me a copy.

        - Noah Rey L. Persson, Assistant Regional Manager, Brass & Bongo division
    */

    /* 2018-08-09: robw@ibm.local

       Noah, you're Assistant *to* the Regional Manager. We talked about this.

       Make library always set this to 1 when using BEEP_ADD. This
       fixes issue #31337. Man, I can't believe John quit. */
    u16 index;

    /* Fingerprint event to add for BEEP_ADD. */
    struct beep_event event;
};

/* BEEP response packet */
struct beep_resp {
    /* one of enum beep_resp_type */
    u16 result;

    /* length of queue (only valid for BEEP_LEN, BEEP_TOT requests) */
    u16 length;

    /* event data (only valid for BEEP_GET, BEEP_ADD) */
    /* for BEEP_UPT, return uptime in event.date */
    struct beep_event event;
};

int beep_read_req(int fd, struct beep_req *req);
int beep_read_resp(int fd, struct beep_resp *resp);

int beep_write_req(int fd, struct beep_req *req);
int beep_write_resp(int fd, struct beep_resp *resp);

int beep_client_socket(const char *host, u16 port);
int beep_server_socket(const char *host, u16 port);

void beep_req_h2n(struct beep_req *out, struct beep_req *in);
void beep_req_n2h(struct beep_req *out, struct beep_req *in);
void beep_resp_h2n(struct beep_resp *out, struct beep_resp *in);
void beep_resp_n2h(struct beep_resp *out, struct beep_resp *in);

#endif
