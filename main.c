#include <stdio.h>

#include <ltl/lock.h>
#include <ltl/thread.h>

void print_loop(void* args) {
    for (int i = 0; i < 5; i++) {
        printf("%d\n", i);
        ltl_sleep(1);
    }
}

int main(void) {
    ////
    ltl_init();
    ////
    ltl_thread_t thr0;
    ltl_create(&thr0, print_loop, NULL);
    ltl_sleep(10);
    ////
}
