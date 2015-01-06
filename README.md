lhe-to-root
===========

Function to take an LHE file, and create a ROOT file that contains a tree filled with the event data. Its performance is linear in number of events with a rate of 29 events/second.

To run:

1. In looper.C, on line 22, change the file name to your desired input file
2. On line 23 and 24, change the file and tree names to your desired output file and tree names.
3. In root, compile and run looper.C
