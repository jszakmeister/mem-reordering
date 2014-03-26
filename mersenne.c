#ifndef INCLUDED_MERSENNE
#include "mersenne.h"
#endif


unsigned int
mersenne_next(mersenne_t *m)
{
    int i = m->index;
    int i2 = m->index + 1;
    int j = m->index + MT_IA;

    if (i2 >= MT_LEN)
        i2 = 0;

    if (j >= MT_LEN)
        j -= MT_LEN;

    unsigned int s = (m->buffer[i] & 0x80000000) | (m->buffer[i2] & 0x7fffffff);
    unsigned int r = m->buffer[j] ^ (s >> 1) ^ ((s & 1) * 0x9908B0DF);

    m->buffer[m->index] = r;
    m->index = i2;

    r ^= (r >> 11);
    r ^= (r << 7) & 0x9d2c5680UL;
    r ^= (r << 15) & 0xefc60000UL;
    r ^= (r >> 18);

    return r;
}


void
mersenne_init(mersenne_t *m, unsigned int seed)
{
    for (int i = 0; i < MT_LEN; i++)
        m->buffer[i] = seed;

    m->index = 0;

    for (int i = 0; i < MT_LEN * 100; i++)
        mersenne_next(m);
}
