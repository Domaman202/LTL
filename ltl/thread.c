#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include <ltl/thread_list.h>

void ltl_sleep(uint32_t seconds) {
    struct timeval time;
    gettimeofday(&time, NULL);
    const uint32_t start = time.tv_sec;
    do {
        usleep((seconds - (time.tv_sec - start)) * 100);
        gettimeofday(&time, NULL);
    } while (time.tv_sec - start < seconds);
}

ltl_thread_t ltl_self(void) {
    return getpid();
}

int ltl_attach(ltl_thread_t thread) {
    // Get thread node
    ltl_thread_node_t* node = ltl_threads_get(thread);
    // Check state (running)
    if (!node->state.running) {
        // Invalid state (thread stopped)
        return LTL_ERR_STOPPED;
    }
    // Check state (detach)
    if (!node->state.detach) {
        // Invalid state (thread detached)
        return LTL_ERR_ATTACHED;
    }
    // Attach thread
    node->state.detach = false;
    return LTL_ERR_SUCCESS;
}

int ltl_detach(ltl_thread_t thread) {
    // Get thread node
    ltl_thread_node_t* node = ltl_threads_get(thread);
    // Check state (running)
    if (!node->state.running) {
        // Invalid state (thread stopped)
        return LTL_ERR_STOPPED;
    }
    // Check state (detach)
    if (node->state.detach) {
        // Invalid state (thread detached)
        return LTL_ERR_DETACH;
    }
    // Detach thread
    node->state.detach = true;
    return LTL_ERR_SUCCESS;
}

void ltl_free(ltl_thread_t thread) {
    ltl_thread_node_t* node = ltl_threads_remove(thread);
    printf("[%d] Free (%d)\n", (int) thread, !!node);
    if (node) {
        if (node->stack)
            free(node->stack);
        free(node);
    }
}

void ltl_free0(ltl_thread_node_t* thread) {
    printf("[%d] Free\n", (int) thread->pid);
    if (thread->stack)
        free(thread->stack);
    free(thread);
}
