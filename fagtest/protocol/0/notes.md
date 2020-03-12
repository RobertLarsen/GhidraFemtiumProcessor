Proprietary Bongo Protocol
==========================

NB: In the following all multi-byte entities use network byte order (the most
    significant byte has the lowest address).

Protocol stack
--------------

    +-------------+
    | PBP         | Application layer
    +-------------+
    | UDP         | Transport layer
    +-------------+
    | IPv6        | Network layer
    +-------------+

Layout
------

Frame:

     00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15  (bytes)
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |           Header            |      Data       |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     Data                      | (+ 16)
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     Data                      | (+ 32)
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     Data                      | (+ 48)
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

Header:

      00   01   02   03   04   05   06   07   08   09   (bytes)
    +----+----+----+----+----+----+----+----+----+----+
    |  MVFT   |   Connection ID   |       Index       |
    +----+----+----+----+----+----+----+----+----+----+

NB: The following diagrams concern bit-wise entities. As is convention, the
    least significant bit is written to the right.

MVFT:

     15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00  (bits)
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |    Magic     |  Version  | RA  |     Type     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

RA:

               01                      00             (bits)
    +-----------------------+-----------------------+
    |       Request         |      Acknowledge      |
    +-----------------------+-----------------------+

Fields
------

  - Frame is the complete frame that includes header:
    - Header: Header.
    - Data: Frame data (54 bytes).

  - Header is the frame header that includes mess
    - MVFT: MVFT.
    - Connection ID: Connection identifier.
    - Index: Frame index from the senders point of view.

  - MVFT is a series of tightly packed fields that includes RA:
    - Magic: Frames always begin with this bit sequence.
    - Version: Protocol version.
    - RA: RA.
    - Type: Frame type.

  - RA:
    - Request: If request flag is set the frame is a request frame.
    - Acknowledge: If acknowledge flag is set the sender expects the recipient
      to acknowledge the packet once it is received.

Data types
----------

### Ding/dong

Devices implementing PBP can send ding frames to each other, and should expect a
dong frame in return.

Ding/dong frames have types 0 and 1 respectively and share a common data layout:

     00 01 02 03 04 05 06 07  (bytes)
    +--+--+--+--+--+--+--+--+
    |         UUID          |
    +--+--+--+--+--+--+--+--+

  - UUID: Universally unique identifier of sender.

Because ding/dong frames aren't "really" part of a connection (connections were
added at a later stage in protocol development), the following caveats apply:

  - Request and acknowledge flags are always 0.
  - The index of ding/dong frames is always 0.
  - A dong should always have the same connection ID as its corresponding ding.

### Error

Error frames have type 31 and this frame layout:

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

  - Data: Newline terminated error message.
