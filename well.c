#define W 32
#define R 624
#define P 31
#define MASKU (0xffffffffU>>(W-P))
#define MASKL (~MASKU)
#define MAT0NEG(t,v) (v^(v<<(-(t))))
#define M1 70
#define M2 179
#define M3 449
#define STATE(i) (sptr[(i) <= loc ? -(i) : R - (i)])

int well19937a( unsigned int *state)
{
   static int loc = 0;
   unsigned int *sptr = state + loc;
   unsigned int rval;
   const unsigned int m1 = sptr[loc < R - M1 ? M1 : M1 - R];
   const unsigned int z1 = *sptr ^ (*sptr << 25) ^ m1 ^ (m1 >> 27);
   const unsigned int m3 = sptr[loc < R - M3 ? M3 : M3 - R];
   const unsigned int z2 = (sptr[loc < R - M2 ? M2 : M2 - R] >> 9) ^ m3 ^ (m3 >> 1);

   if( loc >= 2)
      rval = (sptr[-1] & MASKL) | (sptr[-2] & MASKU);
   else
      rval = (STATE(1) & MASKL) | (STATE(2) & MASKU);
   *sptr = z1 ^ z2;
   rval ^= (z1 << 9) ^ (z2 << 21) ^ (*sptr >> 21);
   if( !loc)
      loc = R - 1;
   else
      loc--;
   state[loc] = rval;
   return( rval);
}

int well19937at( unsigned int *state)
{
   unsigned int rval = well19937a( state);
   const unsigned int TEMPERB = 0xe46e1700U;
   const unsigned int TEMPERC = 0x9b868000U;

   rval ^= (rval << 7) & TEMPERB;
   rval ^= (rval << 15) & TEMPERC;
   return( rval);
}

