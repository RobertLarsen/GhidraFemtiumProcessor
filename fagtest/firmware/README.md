Firmware attack vector
======================

Greetings Agent.

We have identified a possible way for you to attack the DDING-2000.

It turns out the device can receive firmware updates. If we are able to put
together a new filesystem image, this should allow us to exploit the device and
gain the intelligence we need.

The only problem is that the filesystem is not a type we are familiar with. It's
called "U5FS" (sometimes referred to as "U5FSv1"), and it seems to be the native
filesystem for the Fenix kernel.

We have managed to acquire an internal document from Femtium Engineering
Laboratories, with a specification of the U5FS filesystem. At the same time, we
got hold of the Femtium Architecture reference manual, in case you need it.

Assignment:

 - Create a program that can read and extract files from a U5FS image. Think
   `unzip`.

 - Extract all files from the firmware image, and analyze the contents.

 - Extend your program to be able to create a new valid filesystem image, from a
   given set of files. This will allow us to build filesystem images for
   testing.

 - Create a new filesystem image. See if you can come up with a stealthy way to
   run commands on the Femtium system, extract ding event data from it, or
   both. It's up to you if this is in the form of a back door, or some way to
   leak data from the device. Or maybe some other method? Feel free to come up
   with creative solutions.

 - Bonus points for being able to automatically extract the ding event database,
   found in `/data/db`. This database keeps track of all people using the
   DDING-2000, so the data is quite important. If we could have it sent to some
   external server, or have an easy way to poll the device covertly, that would
   be great.

 - We know that some versions of the updater software perform integrity checks
   of the firmware image when updating it. In these versions, all files outside
   of `/data` are checked. See if you can find a way to make your modifications
   only in `/data` and its subdirectories. If you can find a way to make this
   work, it would significantly reduce the risk of getting discovered.

 - On the real device, all files under `/data/config` are preserved across the
   update, while the rest of `/data` is reset to factory default. If you can
   find a way to attack the device using only files under `/data/config`, the
   modification would even survive a firmware update or factory reset. See if
   you can find a way to make this work.

 - The firmware runs a daemon, `/sbin/dingsrv`, that listens on the network. We
   are not sure, but it might even be possible to attack the network protocol
   itself. This is definitely harder than understanding the U5FS image, and if
   you choose to look into this possibility, do so *after* you have a working
   U5FS packer/unpacker. We need to make sure we can modify the file system in
   any case.

Lab setup
---------

In your lab setup, you have a Femtium emulator available, as well as the disk
images. To help you get started, we have prepared a shell script to start the
emulator:

    $ ./run-emu

We have also found a copy of the control program used to communicate with the
`dingsrv` service, to query for ding events, etc. This can be run like so:

    $ ./dingctl

In your lab setup, new ding events will not happen automatically, because nobody
is actually pressing the fingerprint reader. To simulate a keypress, use the
`add` feature of the `dingctl` program to record a new ding event.

Finally, we have managed to acquire one of the source files for these tools. The
file `proto.h` is the header file for the protocol library. It should help you
get started, in case you want to look at the BEEP network protocol. This is the
protocol used to communicate between the x86 system and the Femtium system,
internally in the DDING-2000.

Notes
-----

We had another agent look briefly into this, and they noted that the filesystem
part of the disk image does not start at the first byte. It seems to start about
8KB into the file. The first part probably contains some bootloader and
partition table data. Perhaps these 8KB can simply be copied over, when you
create a new disk image. Feel free to analyze this further, but focus your work
on understanding the filesystem, and being able to read and write it.

It was also mentioned that it might be possible to modify the filesystem simply
using a hex editor. While this is true, it is not nearly flexible enough for
what we need. We know that these devices come in several versions, and we really
do need a tool that can handle the filesystem in general, not just a one time
hack.
