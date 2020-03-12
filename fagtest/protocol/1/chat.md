Chat log
========

During deployment Friday
------------------------

A. G. Bell (principal network architect, master of dings, accumulator of
technical debt): has entered the chat

Q. A. Simodo (chief systems design officer, master of bells and whistles,
manager of dumpster fires): has entered the chat

A. G. Bell: OK, so I have implemented the diagnostic frame types and stuff.
            But. I have forgotten how the dd2k knows which diagnostics to send?

Q. A. Simodo: You remember the request flag, right?

Q. A. Simodo: The backend server sends a frame that has the type of the desired
              diagnostics and the request flag set. Then the dd2k sends the
              diagnostics in response.

A. G. Bell: Ah yes, I forgot. What do I do about the frame data on the requests.

Q. A. Simodo: Just leave it empty, it's not used for anything.

A. G. Bell: Sounds reasonable. Let's ship this thing!

Q. A. Simodo: Break fast and move dings!

Next Monday
-----------

A. G. Bell: Holy shit bro, have you seen the logs from the test deployments?

Q. A. Simodo: Ehm, no?! What's up?

A. G. Bell: Things are going completely haywire. Tons of diagnostic data has
            gone missing.

Q. A. Simodo: How can that be? The implementation passed all the tests in the CI
              pipeline...

A. G. Bell: It looks like we are dealing with massive packet loss. Based on my
            calculations around 5-10% of the frames never show up at their
            destination.

Q. A. Simodo: Let's switch to TCP then.

A. G. Bell: But we don't have a TCP stack on dd2k, remember? We tried, but we
            ran out of time and it turned out to be more better to just skip
            it...

Q. A. Simodo: Management will not be happy about this delay.

Q. A. Simodo: thisisfine.png

Later that day
--------------

A. G. Bell: I got an idea. Maybe you don't have to talk to management about the
            delays quite yet. Let's first see if we can implement our own
            reliable connections!

Q. A. Simodo: I'm not sure. I got a feeling there is a reason why people don't
              invent their own network stacks every day.

A. G. Bell: Nah bro, it will be ez. I already figured out all the nitty gritty
            details. Took me nothing but 5 minutes to sketch this shit out on a
            napkin.

Q. A. Simodo: I'm listening.

A. G. Bell:

Legend:

  | ----- type(index, flags) ----> |: Frame arrives at recipient
  | ----- type(index, flags) ----x |: Frame is lost
  |                                |: Time passing

Base case (no drops):

  A                            B
  | ----- connect(0)     ----> |
  | <---- accept(0,a)    ----- |
  | ----- acknowledge(0) ----> |
  | ----- uptime(1,r)    ----> |
  | ----- total(2,ra)    ----> |
  | <---- acknowledge(2) ----- |
  | <---- uptime(1)      ----- |
  | <---- total(2,a)     ----- |
  | ----- acknowledge(2) ----> |
  | ----- terminate(3,a) ----> |
  | <---- acknowledge(3) ----- |

Drops during request-reply:

  A                            B
  | ----- uptime(1,r)    ----x |
  | ----- total(2,ra)    ----> |
  |                            |
  | ----- uptime(1,r)    ----> |
  | ----- total(2,ra)    ----> |
  | x---- acknowledge(2) ----- |
  |                            |
  | ----- uptime(1,r)    ----x |
  | ----- total(2,ra)    ----> |
  | <---- acknowledge(2) ----- |
  | <---- uptime(1)      ----- |
  | x---- total(2,a)     ----- |
  |                            |
  | <---- uptime(1)      ----- |
  | <---- total(2,a)     ----- |
  | ----- acknowledge(2) ----> |

Drops during connect:

  A                            B
  | ----- connect(0)     ----x |
  |                            |
  | ----- connect(0)     ----> |
  | x---- accept(0,a)    ----- |
  |                            |
  | <---- accept(0,a)    ----- |
  | ----- acknowledge(0) ----x |
  |                            |
  | <---- accept(0,a)    ----- |
  | ----- acknowledge(0) ----> |

A. G. Bell: We add a frame index field to the header. That way we can keep track
            of the order of frames and whether any frames have gone missing.

A. G. Bell: Each end of the connection maintains two numbers: last frame index
            sent and last frame index received.

A. G. Bell: We also add an acknowledge flag to the header. When the acknowledge
            flag is set, the implementation should wait for an acknowledge frame
            before sending any more frames. The other end of the connection
            sends an acknowledge frame with the index of the frame they are
            acknowledging if and only if they have received all the frames up
            until this point. It's forbidden to set the acknowledge flag on a
            acknowledge frame.

A. G. Bell: If the sender doesn't receive an acknowledgment within a reasonable
            time frame (like within latency*2) then it should re-send all the
            frames since the last acknowledgment. That way connections can
            remain reliable even with massive packet loss.

Q. A. Simodo: Sounds like you got it all figured out. Elegant and bulletproof
              design, nothing can go wrong. Let's do this!
