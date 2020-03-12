OPN DINGDONG
============

*CLASSIFIED*

Briefing
--------

Greetings, Agent.

We have obtained troubling reports from field operatives, indicating that
adverse actors are regularly gathering at the Hilbert Hotel, located in the
small eastern European kingdom of Sokovia.

The reports are thus far inconclusive, but it is believed that these meetings
could pose a serious threat to national security, as well as allied forces in
nearby areas.

Through initial reconnaissance and research, we have found a promising
infiltration vector. Your mission, if you choose to accept it, is to develop a
system that will allow us to learn more about the activity of these people.

Objectives
----------

We have identified a vulnerable electronic device at the hotel, which we believe
is our best way to gather more intelligence.

Therefore, your work will focus *exclusively* on this device.

Using your skills in analysis and programming, we would like you to look into
the following areas:

 1) [exploitation] Attempt to gain unauthorized access to the system. Maybe you
    find a flaw in the system, or manage to use the system in an unintended way.

 2) [persistence] If possible, find a way to ensure permanent access, without
    having to perform the exploitation steps again. For example, you could
    develop some type of back door or hidden way to communicate with the device.

 3) [exfiltration] If possible, find a way to accumulate data from the device,
    and send to some external source. This should be done in a stealthy way, to
    ensure the surveillance will not detect our presence.

You should keep these three objectives in mind in your solution. In the best
case, your solution contains a way to handle both exploitation, persistence, and
exfiltration. If you are short on time, try to make a satisfying solution of at
least one of these objectives.

In any case, be sure to submit whatever solution you have, no matter the degree
of completeness.

Target device
-------------

The Hilbert Hotel is quite large. To help staff find and keep track of guests,
they have installed special network-enabled reception bells throughout the
hotel. Our research has identified this IoT device as the DDING-2000 from
International Bongo Machines. This device is believed to have a range of
different possible vulnerabilities. It is up to you to figure out how we can
exploit it.

You are likely already familiar with regular reception bells. It's essentially
a machine that goes "ding". The end.

The DDING-2000 is a SmartBell(tm). It functions as a regular reception bell --
guests can ring the bell to get help -- but also contains an integrated
fingerprint reader, and is connected to a network.

If a guest rings the DDING-2000, a hash of their fingerprint is saved (so staff
can see who is calling), and the "ding" is registered by the networked control
system. Each "ding event" contains a timestamp and the fingerprint hash of the
person pressing it.

We have managed to acquire a number of public (and some non-public) files
relating to the DDING-2000, including fragments of source code, firmware
updates, and some protocol specifications.

Physically the DDING-2000 looks more or less like a regular reception bell from
the outside. On the inside, however, it runs 2 separate computer systems.
There is a 64-bit x86 CPU that does most of the work, and an embedded Femtium
CPU that handles the hardware integration.

Attack vectors
--------------

We have identified three ways to attack this system. Feel free to choose
whichever you feel is more interesting or promising for the task.

1) The update daemon

It seems the manufacturer of DDING-2000 licensed a 3rd-party program to support
software updates for the device. You must research and analyze this update
system, and find a way to produce an update package that is different from what
the manufacturer intended.

Producing an update image that the official updater program will accept, would
give us a very stealthy way to infect the system.

The files for this attack vector are found in `updated/`.

2) The network protocol

The DDING-2000 has a custom network protocol (named "PBP") that allows
communication between DDING-2000 devices. The official programs that use this
protocol are somewhat limited, but maybe you can find a way to use this protocol
for our purposes?

Since the protocol enables communication between devices, this might allow us to
communicate across the network, as soon as we have access to just one device.

The files for this attack vector are found in `protocol/`.

3) The embedded system

The hardware itself runs a Fenix kernel on the Femtium architecture to manage
the fingerprint reader and notification mechanism. If we could construct a
valid firmware image for the Femtium board, we would be able to run code
directly on the embedded hardware.

There is also a minimal client-server protocol that handles communication
between the x86 CPU and the Femtium CPU. Maybe there is some way to gain access
this way, but it is almost certainly easier to create a new firmware image.

The files for this attack vector are found in `firmware/`.

Lab setup
---------

To help you develop a solution, we have procured samples of the programs,
documents and firmwares that relate to this device. We also have a virtual
machine available, with tools to compile code for Fenix. Download it at:

  https://hackerakademi.dk/fenix/sphinx-v1-md5-0e48f20fc7698ad7ab38a96f9453d2e4.7z

For the purpose of research and development, feel free to run, modify and
inspect the supplied files in any way you see fit.

However, it is *critically important* to keep in mind that your final solution
should work in a realistic scenario. After all, this is meant to be deployed on
target. For example:

  - If you attack the network protocol, your attack should work with the
    unmodified server program running under normal conditions.

  - If you attack the updater, the file you produce should work with the
    provided sample of the update daemon.

  - If you attack the firmware image, remember that files outside `/data` are
    (sometimes) checked against a whitelist, so only files under `/data` are
    safe to modify.

These examples illustrate the kinds of shortcuts that are useful while
researching, but make it impossible to deploy on target.

If you end up making a proof-of-concept of a technique that only works in a
non-realistic scenario, be sure to document the limitations and problems you
faced, when submitting your solution. But definitely *do submit* what you have.

Assumptions
-----------

When working on one attack vector you can assume that we already have working
attacks for the others. For example, if your chosen vector is the update daemon
you may assume that we have some means of connecting to it (e.g. using PBP).

Or maybe you have found a neat exfiltration method, but it requires root access
to execute it.

In all cases be sure to clearly state the assumptions you are working under.

Each attack vector has further instructions at:

  - `updated/README.md`
  - `firmware/README.md`
  - `protocol/README.md`

Note that on target, the only way to communicate with the DDING-2000 is through
PBP; all other ports are firewalled.

If time allows please do consider the consequences of running in a different
scenario than the one described in the attack vector readme (e.g. one with a
different/no firewall, different users/groups, etc.)

Deliverables
------------

You *must* focus your work on one of the three attack vectors, and it *must* be
clear from your submission which one you chose.

If you feel confident that you have solved your primary attack vector fully,
feel free to solve more than one, for bonus points. However, you still *must*
describe which one is your primary.

With this in mind, here is what you should include in your submission:

 A) Name of primary attack vector.

 B) Report of your work (in English or Danish), documenting:

    1) A summary of which parts you have worked on, and a how well they work.
       For example, you might have a very well-developed exploit, a workable
       idea for how to do persistence, and nothing concrete for exfiltration.
       Or a really nice exfiltration setup, but no exploit to make it work in
       the field.

       It is *not* a requirement to solve the task completely. Just describe
       what you have.

    2) A description of how your solution works, with the amount of technical
       detail that you find fitting. Try to explain your solution as precisely
       as possible. Assume the reader is highly skilled in technical matters,
       but not familiar with the exact system you are working on.

    3) Your observations along the way. Here you get a chance to explain *why*,
       not *how*. Are there technical constraints that guide your choice of
       language/technique/idea, or could things be done differently?  What did
       you notice along the way?  Are there other possible ways to attack the
       device that you did not have time to go into? Or did you deliberately
       chose to stay away from them? Even minor rough corners or undeveloped
       ideas fit here, but try to describe everything in precise terms.

  C) Source code:

     Use whatever programming languages you are comfortable with, as long as
     they are reasonably common. You can use publicly-available libraries and
     programs, as well as any source code you yourself have written. Anything
     that you did not produce yourself should be clearly marked as such.

     Closed source libraries and binary-only tools are strictly forbidden.

     It is permissible to use a closed-source tool to produce a data file, as
     long as the data file itself is included. For example, if you use a
     binary-only hex editor to modify a data file, that is OK. If your solution
     requires a binary-only library for calculating checksums, that is *not* OK.

     Try to keep the source code well organized and resonably clean.

  D) Additional material.

     Include any important data files, snippets, demo programs, etc., that
     support your solution.

Suggested file layout:

  - `README.md`:     Short overview of your solution, your file structure, etc.
  - `REPORT.md`:     Your report.
  - ...
  - `src/`:          All source code associated with your solution.
      - `README.md`: Brief description of your source code.
      - ...
  - `files/`:        Additional material.
      - `README.md`: Brief description of additional files.
      - ...

Organizing your files in this way is not a strict requirement, but it will make
it easier for us to work with your solution.

It is important to note that solving an attack vector, while important, is *not*
the primary goal of this challenge. Your ability to document your findings,
hypotheses, thoughts and ideas is more important. An incomplete solution with
clear findings, is preferred over a bunch of code that works perfectly, but has
no documentation.

In summary: The report is very important. Keep it precise, and brief.
