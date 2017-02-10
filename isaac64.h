/*
------------------------------------------------------------------------------
isaac64.h: definitions for a random number generator
Bob Jenkins, 1996, Public Domain
------------------------------------------------------------------------------
*/
#include "stdint.h"


#ifndef ISAAC64
#define ISAAC64

void isaac64_init( const int flag);
uint64_t isaac64( void);

#endif  /* RAND */

