# Engine protocol (the contract)

Status: **to be designed in step 1**, before any engine code is written.

This file will define:

1. How the server talks to the C engine (one text command per line on stdin, one reply on
   stdout), and the exact command list: add, search, brute-force search, neighbours, trace,
   stats, save, load.
2. The reply format for each command, including errors.
3. The binary file formats: vectors file (written by the Python pipeline) and index file
   (written by the engine).
4. The distance measure (cosine on normalised vectors vs Euclidean).

Every change to this file must be matched by the C engine and the Python server in the
same commit.
