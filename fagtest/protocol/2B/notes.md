Proprietary Bongo Protocol
==========================

NB: In the following all multi-byte entities use network byte order (the most
    significant byte has the lowest address).

Data types
----------

### Command submit/result

The DDING-2000 can be remotely administrated by sending submit frames and
receiving result frames.

Command submit/result frames have types 20 and 21 respectively and share a
common data layout:

     00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15  (bytes)
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     Data                      |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     Data                      | (+ 16)
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     Data                      | (+ 32)
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |      Data       |                               (+ 48)
    +--+--+--+--+--+--+

  - Data: Newline terminated command/result.
