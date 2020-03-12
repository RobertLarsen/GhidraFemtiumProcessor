Proprietary Bongo Protocol
==========================

NB: In the following all multi-byte entities use network byte order (the most
    significant byte has the lowest address).

Data types
----------

### Stream begin

Stream begin frames have type 25 and this data layout:

     00 01 02 03 04 05 06 07  (bytes)
    +--+--+--+--+--+--+--+--+
    |         Length        |
    +--+--+--+--+--+--+--+--+

  - Length: The length of the byte stream as an unsigned integer.

### Stream data

Stream data frames have type 26 and this data layout:

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

  - Data: Binary data.

### Upgrade

Upgrade frames initiate an upgrade. It is expected that the initiating party
starts streaming the upgrade immediately after the upgrade frame has been
acknowledged. Once the upgrade has been uploaded, it is expected that the party
receiving the upgrade starts streaming a reply.

Upgrade data frames have type 30 and no data. The upgrade data is sent as a
stream.

After receiving an upgrade image, the PBP daemon will attempt to connect to the
`UPDATEd` service on `127.0.0.1:1999`, and send it the upgrade image. The
response from `UPDATEd` is sent back as the reply.
