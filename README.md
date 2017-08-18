# prngs
Assorted pseudo-random number (PRN) generators.  I mostly just use the
Mersenne Twister 64-bit variation,  simply because MT-64 has been very
thoroughly tested by a lot of people and its weaknesses are known
(essentially,  it doesn't have any known biases).  Determining where a
PRNG is 'bad' is quite difficult;  as with cryptographic algorithms,
you probably shouldn't trust _any_ PRNG unless it has been completely
examined and no biases found. If MT-64 has problems,  a _lot_ of
scientific papers relying on it will have to be investigated.

However,  I've played around with some other PRNGs,  just out of curiosity,
and most of the resulting code is here.  With the possible exception of
the WELL algorithms,  or for use in a game,  I wouldn't use any of them.

Among other toys,  this has code to :

- generate Gaussian-distributed PRNs using the Monty Python algorithm;

- find prime numbers in a desired range (limited to 64 bits,  though.)

- generate random Latin squares and 'cages' for KenKen(TM) puzzles (haven't
gotten beyond just making the squares and cages,  though)

- Improved version of Paul Hsieh's SuperFastHash function (the original
had collision issues and was for 32-bit systems)

- Code to generate Blum-Blum-Shub PRNs (limited to 64 and 128 bits;  this
isn't nearly enough to be cryptographic,  but may be a good source of
PRNs for non-crypto situations)

On Linux,  run `make` to build the library and various test executables.
(You can also do this with MinGW under Windows.)  For BSD,  and probably
OS/X,  run `gmake CLANG=Y` (GNU make,  with the clang compiler).
