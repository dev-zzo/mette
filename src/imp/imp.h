#ifndef __mette_imp_included
#define __mette_imp_included

/* A type that fits into one register on the target. */
typedef unsigned long imp_archreg_t;

/* Internal details on how we store the data... */
struct imp_num_t {
    unsigned int length; /* Available bits count */
    imp_archreg_t *bits; /* Actual bits */
};


/* Initialize the target number for given length, bits */
extern void imp_num_init(struct imp_num_t *num, unsigned int length);

extern void imp_num_free(struct imp_num_t *num);

extern void imp_num_load1(struct imp_num_t *num, imp_archreg_t value);

extern void imp_num_print(struct imp_num_t *num);

/* Add two operands: res = lo + ro. */
extern void imp_add(const struct imp_num_t *lo, const struct imp_num_t *ro, struct imp_num_t *res);

/* Subtract two operands: res = lo - ro. */
extern void imp_sub(const struct imp_num_t *lo, const struct imp_num_t *ro, struct imp_num_t *res);

#endif // __mette_imp_included

