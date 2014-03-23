This project is an attempt to utilize information learned in LFD312.  As such,
clarity is chosen over speed, and using multiple ways of doing the same thing
(ie, shared libraries and DLLs) to demonstrate usage vs consistency across the
whole application.

The *_dl.so files are "game servers" that pass data with a main thread and 
multiple player threads using pipes. These libraries are also executable -
running the library will run a simulation test suite against the library.

The broader idea is to write a daemon that will accept network connections to 
create a game, join a game, and get status on running games.

The client program can run on the same machine or on a separate machine.  
Potentially it could just be telnet, but since this is a learning experiment
the client is created as a separate application.

Source code available at ###...

Licensed under GPL license V3.
(c) 2014 Richard Hendricks, richardhendricks@pobox.com
