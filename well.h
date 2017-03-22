void init_well19937a( uint32_t *state, const uint32_t seed);
uint32_t well19937a( uint32_t *state);
uint32_t well19937at( uint32_t *state);
uint64_t well19937a_64( uint32_t *state);
uint64_t well19937at_64( uint32_t *state);
uint64_t well19937at_get_n_bits( uint32_t *state, const int n_bits);
double well19937a_d( uint32_t *state);
double well19937at_d( uint32_t *state);

#define WELL19937_ARRAY_LENGTH   625
