#ifndef __LTL_LOCK_H__
#define __LTL_LOCK_H__

#include <stdint.h>

typedef struct ltl_lock {
    uint16_t waiter;
    uint16_t dish;
} ltl_lock_t;

void ltl_lock(ltl_lock_t* lock);
bool ltl_try_lock(ltl_lock_t* lock);
void ltl_unlock(ltl_lock_t* lock);

#endif // __LTL_LOCK_H__
