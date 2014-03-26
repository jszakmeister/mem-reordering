#ifndef INCLUDED_MERSENNE
#define INCLUDED_MERSENNE

#ifdef __cplusplus
extern "C" {
#endif

#define MT_IA  397
#define MT_LEN 624

typedef struct mersenne_t
{
    unsigned int buffer[MT_LEN];
    int index;
} mersenne_t;


void
mersenne_init(mersenne_t *m, unsigned int seed);


unsigned int
mersenne_next(mersenne_t *m);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_MERSENNE */
