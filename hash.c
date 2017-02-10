#include <stdint.h>
#include <string.h>        /* For memcpy() */
#include <stddef.h>        /* For NULL #define */

/* Modified version of Paul Hsieh's "super-fast" non-cryptographic hash
function.  The original version worked with 32 bits,  not 64,  and had
serious collision issues.  See 'hsieh.txt' for details.  Test code for
this is in 'hashtest.c'.         */

#undef get64bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get64bits(d) (*((const uint64_t *) (d)))
#endif

#if !defined (get64bits)
#define get64bits(d) ((((uint64_t)(((const uint8_t *)(d))[7])) << 56)\
                     +(((uint64_t)(((const uint8_t *)(d))[6])) << 48)\
                     +(((uint64_t)(((const uint8_t *)(d))[5])) << 40)\
                     +(((uint64_t)(((const uint8_t *)(d))[4])) << 32)\
                     +(((uint64_t)(((const uint8_t *)(d))[3])) << 24)\
                     +(((uint64_t)(((const uint8_t *)(d))[2])) << 16)\
                     +(((uint64_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint64_t)(((const uint8_t *)(d))[0]) )
#endif

uint64_t SuperFastHash64 (const char * data, size_t len);

uint64_t SuperFastHash64 (const char * data, size_t len) {
uint64_t hash = len;
size_t rem = len & 7;

    if (!len || data == NULL) return 0;

    if( rem)
       {
       uint64_t tval = 0;

       memcpy( &tval, data + len - rem, rem);
       hash ^= tval;
       hash ^= hash >> 29;
       hash ^= hash << 17;
       }
    len >>= 3;

    /* Main loop */
    while( len--)
      {
      hash += get64bits( data);
      hash += hash >> 29;
      hash ^= hash << 17;
      data += sizeof( uint64_t);
      }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash >> 32;
    hash ^= hash << 28;
    hash ^= hash >> 16;
    hash ^= hash << 9;
    hash ^= hash >> 3;
    hash ^= hash << 1;

    return( hash);
}
