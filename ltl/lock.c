#include <sched.h>
#include <stdatomic.h>

#include <ltl/lock.h>

void ltl_lock(ltl_lock_t* lock) {
    const uint16_t waiter = atomic_fetch_add(&lock->waiter, 1);
    while (atomic_load(&lock->dish) != waiter) sched_yield();
}

bool ltl_try_lock(ltl_lock_t* lock) {
    if (atomic_load(&lock->waiter) == atomic_load(&lock->dish)) {
        ltl_lock(lock);
        return true;
    }
    return false;
}

void ltl_unlock(ltl_lock_t* lock) {
    atomic_fetch_add(&lock->dish, 1);
}