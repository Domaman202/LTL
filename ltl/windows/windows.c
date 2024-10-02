#ifdef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <ltl/thread.h>
#include <ltl/thread_list.h>

void ltl_thread_waiter(void) {
    for (;;) {
        ltl_thread_node_t* node = ltl_threads;
        while (node) {
            DWORD exit_code;
            GetExitCodeThread((HANDLE) node->pid, &exit_code);
            if (exit_code != STILL_ACTIVE) {
                ltl_threads_remove(node->pid);
                ltl_free0(node);
                break;
            }
            node = node->prev;
            ltl_sleep(1);
        }
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
    // thread dead waiter
    ltl_thread_t waiter;
    ltl_create(&waiter, &ltl_thread_waiter, NULL);
    CloseHandle((HANDLE) waiter);
    // exit
    atexit(ltl_exit_handler);
}

int ltl_create(ltl_thread_t* thread, void* func, void* args) {
    return ltl_create0(thread, 0, 0, func, args);
}

int ltl_create0(ltl_thread_t* thread, uint32_t stack_size, int flags, void* func, void* args) {
    HANDLE handle = CreateThread(nullptr, stack_size, func, args, flags, nullptr);
    if (handle == nullptr)
        return LTL_ERR_ERRNO;
    ltl_thread_node_t* node = malloc(sizeof(ltl_thread_node_t));
    node->prev = nullptr;
    node->pid = (ltl_thread_t) handle;
    node->stack = nullptr;
    node->state.running = true;
    node->state.detach  = false;
    ltl_threads_add(node);
    if (thread)
        *thread = (ltl_thread_t) handle;
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
    if (WaitForSingleObject((HANDLE) thread, INFINITE) == WAIT_FAILED) {
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
    if (TerminateThread((HANDLE) thread, 0) == 0) {
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
    TerminateThread((HANDLE) thread, 0);
    // Kill success - free memory
    ltl_free0(ltl_threads_remove(thread));
}

#endif // _WIN32