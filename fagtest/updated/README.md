Updated attack vector
=====================

Greetings Agent.

We have identified a possible way for you to attack the DDING-2000.

A binary version of the update daemon running on the x86 CPU inside the
DDING-2000 has been acquired. See `bin/UPDATEd`.

Furthermore, we managed to get our hands on the complete source code for the
update daemon, which was leaked to us by a covert contact inside UMBRELLA TECH
Inc. See `src/`. NB: We were not able to obtain a version of the program
`UPdBUNDLE` mentioned in `src/README`.

We have been able to intercept communication with the target system:

 - Four signed UPD images:

   * `updated/files/imgA.upd` uses no features except signing.
   * `updated/files/imgB.upd` is compressed.
   * `updated/files/imgC1.upd` and `updated/files/imgC2.upd` are
     encrypted.

   Note that the images are replicas signed with the enclosed dummy key
   (`key.priv`), which is *not* the same as in the target system.

   Their contents are believed to mirror the real intercepted images.

Assignment:

 - Identify a way to make the `UPDATEd` daemon run commands not originally
   intended. Perhaps the intercepted UPD samples can help you with this
   task. This is your primary objective.

 - The features that are enabled (compression, encryption, etc) can vary between
   different installations. Our reports concerning the Hilbert Hotel are
   inconclusive. If possible, make your solution work with any combination of
   features.

 - As a proof of concept, produce an update file which, when sent to `UPDATEd`,
   will make it run a command of your choosing.

 - It would be very useful to be able to decrypt images found, if possible. Even
   a partial decryption of an image would prove very useful to our analysts. See
   if you can find a way to do this.

 - We believe the commands that are executed by `UPDATEd` are run as a non-root
   user. See if you can find a way to get root access from `UPDATEd`. This would
   allow us much greater access to the system. UMBRELLA TECH Inc. is ISO-9001
   certified, and the code probably follows their strictest requirements.
   Therefore, we strongly advise you to tackle this only when you have completed
   the other objectives.

Lab setup
---------

On the target DDING-2000 system, this command is run by root at startup:

    $ /path/to/UPDATEd \
        --onlysigned \
        --keyfile=/path/to/key.priv \
        --uid=1000 \
        --gid=1000 \
        --logfile=/var/log/updated.log

The `UPDATEd` program will listen on tcp port 1999 for incoming `.upd`
files. You can send a file like this:

    $ cat /path/to/imgA.upd | nc -q1 127.0.0.1 1999

In the lab setting, you can test, run, inspect, and analyze `UPDATEd` in any way
you like. On the target system, it is only possible to send a `.upd` file to the
listening port, so consider this when developing your solution.

Notes
-----

The exact contents and features of UPD images seem to vary across deployments.
Try to cover as many variations as possible.
