#ifndef __LTL_THREAD_LIST_H__
#define __LTL_THREAD_LIST_H__

#include <ltl/thread.h>

/// Состояние потока.
typedef struct ltl_thread_state {
    bool running : 1;
    bool detach  : 1;
} ltl_thread_state_t;

/// Нода списка потоков.
typedef struct ltl_thread_node {
    /// Прошлый поток.
    struct ltl_thread_node* prev;
    /// Process ID
    ltl_thread_t pid;
    /// Stack Pointer
    void* stack;
    /// State
    ltl_thread_state_t state;
} ltl_thread_node_t;

/// Список потоков.
extern ltl_thread_node_t* ltl_threads;
/// Добавить поток в список.
void ltl_threads_add(ltl_thread_node_t* thread);
/// Получить поток из списка.
ltl_thread_node_t* ltl_threads_get(ltl_thread_t thread);
/// Удалить поток из списка.
ltl_thread_node_t* ltl_threads_remove(ltl_thread_t thread);

#endif // __LTL_THREAD_LIST_H__
