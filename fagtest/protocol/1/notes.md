Proprietary Bongo Protocol
==========================

NB: In the following all multi-byte entities use network byte order (the most
    significant byte has the lowest address).

Data types
----------

### Connect/accept

To establish a connection using PBP a device must send a connect frame to the
the other DDING-2000. The recipient responds with an accept frame. They look
very much like ding/dong frames.

Connect/accept frames have types 5 and 6 respectively and share a common data
layout:

     00 01 02 03 04 05 06 07  (bytes)
    +--+--+--+--+--+--+--+--+
    |         UUID          |
    +--+--+--+--+--+--+--+--+

  - UUID: Universally unique identifier of sender.

Based on empirical research the following is true for connect/accept frames:

  - Request flag is always 0 for connect/accept frames.
  - Acknowledge flag is always 0 for connect frames.
  - Acknowledge flag is always 1 for accept frames.
  - DDING-2000 usually tries to establish a connection 10 times, each attempt
    spaced 3 second apart, before giving up on establishing a connection.

### Terminate

Implementations send a terminate frame as they close their end of the
connection.

Terminate frames have type 7 and no data.

### Acknowledge

Acknowledge frames play a special part in maintaining reliable connections over
PBP. However, I am at a loss how this works... It seems like a giant mess to
me. Whoever designed this protocol must be utterly insane.

Acknowledge frames have type 8 and no data.

### Uptime

Uptime frames have type 10 and this data layout:

     00 01 02 03  (bytes)
    +--+--+--+--+
    | Timestamp |
    +--+--+--+--+

  - Timestamp: Seconds since boot as an unsigned integer.

### Total

Total frames have type 11 and this data layout:

     00 01 02 03 04 05 06 07  (bytes)
    +--+--+--+--+--+--+--+--+
    |         Total         |
    +--+--+--+--+--+--+--+--+

  - Total: Total dings recorded since boot as an unsigned integer.
