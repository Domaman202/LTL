#ifdef __linux__

#define _GNU_SOURCE

#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>

#include <ltl/thread.h>
#include <ltl/thread_list.h>>

void ltl_sigchld_handler(int sig) {
    ltl_thread_t thread;
    int status;
    while ((thread = waitpid(-1, &status, __WALL)) > 0) {
        ltl_free(thread);
    }
}

void ltl_exit_handler(void) {
    while (ltl_threads) {
        if (!ltl_threads->state.running || ltl_threads->state.detach) {
            ltl_destroy0(ltl_threads->pid);
        } else {
            ltl_join(ltl_threads->pid);
        }
    }
}

void ltl_init(void) {
    // thread dead
    struct sigaction sigchld_sa;
    sigchld_sa.sa_handler = ltl_sigchld_handler;
    sigchld_sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&sigchld_sa.sa_mask);
    sigaction(SIGCHLD, &sigchld_sa, nullptr);
    // exit
    atexit(ltl_exit_handler);
}

int ltl_create(ltl_thread_t* thread, void* func, void* args) {
    struct rlimit rlimit;
    getrlimit(RLIMIT_STACK, &rlimit);
    return ltl_create0(thread, rlimit.rlim_cur, CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_IO | SIGCHLD, func, args);
}

int ltl_create0(ltl_thread_t* thread, uint32_t stack_size, int flags, void* func, void* args) {
    void* stack = malloc(stack_size);
    ltl_thread_t pid = clone(func, stack + stack_size, flags, args);
    if (pid == -1) {
        free(stack);
        return LTL_ERR_ERRNO;
    }
    ltl_thread_node_t* node = malloc(sizeof(ltl_thread_node_t));
    node->prev = nullptr;
    node->pid = pid;
    node->stack = stack;
    node->state.running = true;
    node->state.detach  = false;
    ltl_threads_add(node);
    *thread = pid;
    return LTL_ERR_SUCCESS;
}

int ltl_join(ltl_thread_t thread) {
    // Getting node
    ltl_thread_node_t* node = ltl_threads_remove(thread);
    // Check state (running)
    if (!node->state.running) {
        // Invalid state (thread stopped)
        ltl_threads_add(node);
        return LTL_ERR_STOPPED;
    }
    // Check state (detach)
    if (node->state.detach) {
        // Invalid state (thread detached)
        ltl_threads_add(node);
        return LTL_ERR_DETACH;
    }
    // Try wait
    if (waitpid(thread, nullptr, __WALL) == -1) {
        // Wait failed
        ltl_threads_add(node);
        return LTL_ERR_ERRNO;
    }
    // Wait success - free memory
    ltl_free0(node);
    return LTL_ERR_SUCCESS;
}

int ltl_destroy(ltl_thread_t thread) {
    // Get thread node
    ltl_thread_node_t* node = ltl_threads_remove(thread);
    // Check state (running)
    if (!node->state.running) {
        // Invalid state (thread stopped)
        ltl_threads_add(node);
        return LTL_ERR_STOPPED;
    }
    // Check state (detached)
    if (node->state.detach) {
        // Invalid state (thread detached)
        ltl_threads_add(node);
        return LTL_ERR_DETACH;
    }
    // Try kill
    if (kill(thread, SIGKILL) == -1) {
        // Kill failed
        ltl_threads_add(node);
        return LTL_ERR_ERRNO;
    }
    // Kill success - free memory
    ltl_free0(node);
    return LTL_ERR_SUCCESS;
}

void ltl_destroy0(ltl_thread_t thread) {
    // Try kill
    kill(thread, SIGKILL);
    // Kill success - free memory
    ltl_free0(ltl_threads_remove(thread));
}

#endif // __linux__