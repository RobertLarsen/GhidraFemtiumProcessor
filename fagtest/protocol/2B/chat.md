Chat log
========

Thursday afternoon
------------------

A. G. Bell (principal network architect, master of dings, accumulator of
technical debt): has entered the chat

Q. A. Simodo (chief systems design officer, master of bells and whistles,
manager of dumpster fires): has entered the chat

A. G. Bell: Darn it! Guess what I just heard while I was getting my coffee. The
            Femtium guys may be going to change the dingctl commands again.

Q. A. Simodo: What, three letter abbrs are not cutting it for them anymore?

Q. A. Simodo: That'll be a real headache in case someone updates dingctl but not
              pbpd or vice versa.

A. G. Bell: Well, the customers are not supposed to update their devices on
            their own. That's what our support tech guys are for.

Q. A. Simodo: Sure. But users are users.

A. G. Bell: Tell you what. We'll just put in a catch-all frame type that'll
            include the dingctl command to run.  I mean, we already implemented
            reliable connections so this should be easy.

A. G. Bell: Added benefit: we'll be able to debug problems with dingctl
            remotely.

Q. A. Simodo: I don't see what could go wrong; I'll work on that.. after coffee.
