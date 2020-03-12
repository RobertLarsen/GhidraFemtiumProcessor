Proprietary Bongo Protocol attack vector
========================================

Intro
-----

Greetings Agent.

We have identified a possible way for you to attack the DDING-2000.

International Bongo Machines (IBM) has developed PBP for monitoring and remote
administration of DDING-2000.

During the development of PBP, IBM grew increasingly worried about abuse of
DDING-2000 through PBP. Rather than implementing proper security, the engineers
at IBM decided on deploying the tried and true security paradigm StO (Security
through Obscurity). The engineers were sworn to secrecy, and very little
information about PBP has ever escaped the gnarly brains of the architects. It
is rumored that even the designers of PBP don't quite understand how the
protocol works anymore and that the implementation is the only true reference.

Assignment:

For details of each task, see below.  Here a summary:

 - Determine the UUID of the lab device.

 - Establish a reliable connection to the lab device over an unreliable network.

 - Achieve each of the following on the lab device:
   - Retrieve diagnostics.
   - Retrieve fingerprint entries.
   - Execute arbitrary commands.
   - Upgrade the device with a provided image.

Lab setup
---------

We managed to acquire a server implementation of PBP (`pbpd`), from a real
deployed DDING-2000. You can test your implementation against this server.

Additionally, we have obtained a UDP proxy that simulates slow, unreliable
networks. Our intel suggests that this proxy is used by the developers of the
PBP server to simulate realistic deployment scenarios.

Both binaries are compiled for 64-bit Linux.

Both binaries support `-help` for listing command line options and defaults.

Here is an example of how to run the binaries:

    $ ./pbpd -address=[::]:1337 -action=listen

    $ ./proxy -source [::]:1338 -destination [::]:1337 -latency 50 -droprate 10

In the lab you can connect directly to the PBP daemon on port 1337, or through
the proxy on port 1338.  The proxy forwards your traffic to port 1337, while
simulating an unreliable network (in the above case each packet will be delayed
50ms, and about 10% of them will be dropped).

0: Ding
-------

IBM first started developing PBP because they wanted to monitor DDING-2000 after
they were shipped and deployed. For this initial version, all IBM wanted to know
was whether a specific DDING-2000 was deployed correctly and connected to the
internet. They did so by sending ding frames to a central server which responds
with dong frames. Ding/dong is still part of PBP, and we recently captured
traffic of IBM's backend server interacting with a DDING-2000 using ding/dong.

One of your colleagues has made some headway reverse engineering the protocol.
Make sure to read their notes carefully.

### Task

- Implement ding/dong and confirm that you can talk with the lab device using
  PBP.

- Determine the UUID of the lab device.

### Resources

- Colleague's notes on PBP.

- PCAP of ding/dong interaction between IBM's backend server and a DDING-2000.

1: Connect
----------

After the success of ding/dong, the engineers at IBM decided to expand the
protocol quite significantly with frame types for harvesting diagnostic
information from DDING-2000. Initially adding a few extra frame types was a
trivial change, and they quickly had a working implementation running locally.

IBM was eager to get the diagnostics features in production and reap the
benefits of "big data". However, initial test deployments were unsuccessful. It
turned out many DDING-2000 were deployed on unreliable connections that had high
packet loss rates.

At this point two of the lead architects at IBM had a conversation about how
they would go about fixing the issue. We have managed to "find" this
conversation. We also captured some traffic of IBM's backend server gathering
diagnostics from a DDING-2000.

Your colleague has made additional notes on PBP.

### Task

- Implement reliable connections (in an unreliable network; use the provided
  proxy as a simulation).

- Demonstrate that you can fetch diagnostics from the lab device through the
  unreliable proxy (submit PCAP).

### Resources

- Additional notes from colleague.

- Conversation between the two lead architects.

- PCAP of IBM's backend server gathering diagnostics from a DDING-2000.

Intermezzo
----------

With reliable connections sky was the limit for the PBP development team. They
began to tinker with new uses for PBP and found three perfect use cases for
their shiny protocol.

2A: Fetch
---------

Based on recent information, IBM has started using PBP for retrieving entries
from DDING-2000 for centralized data analysis. We expect each entry to consist
of a timestamp and a fingerprint. Since the DDING-2000 has very limited memory,
the entries probably disappear after retrieval.

Your colleague stumbled upon a struct definition in the firmware that might be
related to this feature, and believe this PCAP contains some relevant frames.

### Task

- Implement support for fetching entries from DDING-2000.

- Demonstrate that you can reliably retrieve entries periodically from the lab
  device through the unreliable proxy (submit PCAP). Fetching all entries from
  DDING-2000 every 30 seconds for 5 minutes should suffice.

### Resources

- Additional notes from colleague.

- PCAP of IBM's backend server fetching entries from a DDING-2000.

2B: Administrate
----------------

Providing good customer support is crucial. IBM has added remote administration
support to PBP, enabling technical support to live-debug problems for IBM's most
valued enterprise accounts without jumping on a plane.

Your colleague has made some notes on the subject.

### Task

- Implement a remote administration command line client for DDING-2000.

- Explore and document the capabilities of the remote shell.

- Demonstrate that you can remotely administer the lab device through the
  unreliable proxy (submit shell log and PCAP).

### Resources

- Conversation between the two lead architects.

- Additional notes from colleague.

2C: Upgrade
-----------

A data stream feature has been added to PBP for sending large amounts of data,
making it feasible to upgrade the firmware of DDING-2000 remotely.

We got our hands on brand new upgrade images for DDING-2000, and your colleague
has made some notes on the subject. We don't know which, if any, features these
updates bring...

I guess there is only one way to find out.

### Task

- Implement an upgrade command line utility for DDING-2000.

- Demonstrate that you can upgrade the lab device through the unreliable proxy
  (submit utility log and PCAP).

### Resources

- Additional notes from colleague.

- PCAP of an upgrade request/reply.

- Sample upgrade images can be found in `../updated/files`.  See the `updated
  attack vector` for further details.
