#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

uint32_t wellrng1024( uint32_t *state);
void init_well1024( uint32_t *state, const uint32_t seed);
uint64_t wellrng1024_64( uint32_t *state);
uint64_t wellrng1024_bits( uint32_t *state, const int n_bits);
double wellrng1024_d( uint32_t *state);

/* The WELL1024 algorithm requires 32 32-bit integers to get 1024 bits,
   plus a 33rd integer to store our current index within that array : */

#define WELL1024_STATE_ARRAY_SIZE   33

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */
