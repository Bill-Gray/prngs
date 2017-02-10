# prngs
Assorted pseudo-random number (PRN) generators.  I mostly just use the
Mersenne Twister 64-bit variation,  simply because MT has been very
thoroughly tested by a lot of people and its weaknesses are known.
Determining where a PRNG is 'bad' is rather difficult;  I have no
good reason not to just use one that has been extensively investigated.
If MT has problems,  a _lot_ of scientific papers relying on it will
have to be investigated.

On Linux,  run `make` to build the library and various test executables.
(You can also do this with MinGW under Windows.)  For BSD,  and probably
OS/X,  run `gmake CLANG=Y` (GNU make,  with the clang compiler).
