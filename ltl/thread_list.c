#include <ltl/thread_list.h>

ltl_thread_node_t* ltl_threads;

void ltl_threads_add(ltl_thread_node_t* thread) {
    thread->prev = ltl_threads;
    ltl_threads = thread;
}

ltl_thread_node_t* ltl_threads_get(ltl_thread_t thread) {
    ltl_thread_node_t* node = ltl_threads;
    while (node) {
        if (node->pid == thread)
            return node;
        node = node->prev;
    }
    return nullptr;
}

ltl_thread_node_t* ltl_threads_remove(ltl_thread_t thread) {
    if (ltl_threads == NULL)
        return nullptr;

    ltl_thread_node_t* node = ltl_threads;
    if (node->pid == thread) {
        ltl_threads = node->prev;
        return node;
    }

    while (node->prev) {
        if (node->prev->pid == thread) {
            ltl_thread_node_t* deleted = node->prev;
            node->prev = deleted->prev;
            return deleted;
        }
        node = node->prev;
    }

    return nullptr;
}

