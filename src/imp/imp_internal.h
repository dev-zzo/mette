#ifndef __mette_imp_internal_included
#define __mette_imp_internal_included

#include "imp.h"

/* Helper macro to round off LSBs with mask. */
#define ROUND_MASK(x, m) (((x) + (m)) & (~(m)))

/* ARCHITECTURE DEPENDENT DEFINES */
/* TODO: Move them somewhere more appropriate */

#define ARCH_REG_LENGTH_BITS (sizeof(imp_archreg_t) * 8)

#endif // __mette_imp_internal_included

