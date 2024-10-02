#ifndef __LTL_THREAD_H__
#define __LTL_THREAD_H__

#ifdef __linux__
#include <sys/wait.h>
#include <sys/resource.h>
#elifdef _WIN32
#include <windows.h>
#else
#warning "Unsupported platform!"
#endif

#include <stdint.h>
#include <stddef.h>

/// Успешное выполнение функции.
#define LTL_ERR_SUCCESS     0x0
/// Ошибка передана в ERRNO.
#define LTL_ERR_ERRNO       0x1
/// Поток запущен.
#define LTL_ERR_RUNNING     0x1
/// Поток не запущен.
#define LTL_ERR_STOPPED     0x2
/// Потом присоединён
#define LTL_ERR_ATTACHED    0x3
/// Поток отсоединён
#define LTL_ERR_DETACH      0x4

#ifdef __linux__
/// Идентификатор потока.
typedef pid_t ltl_thread_t;
#elifdef _WIN32
/// Идентификатор потока.
typedef uintptr_t ltl_thread_t;
#endif

/// Нода списка потоков.
typedef struct ltl_thread_node ltl_thread_node_t;

/// Инициализация библиотеки.
void ltl_init(void);
/// Отправить поток спать на N секунд.
void ltl_sleep(uint32_t seconds);

/// Получение текущего потока.
ltl_thread_t ltl_self(void);
/// Создание нового потока.
int ltl_create(ltl_thread_t* thread, void* func, void* args);
/// Создание нового потока.
int ltl_create0(ltl_thread_t* thread, uint32_t stack_size, int flags, void* func, void* args);
/// Ожидание завершения потока.
int ltl_join(ltl_thread_t thread);
/// Присоединение потока.
int ltl_attach(ltl_thread_t thread);
/// Отсоединение потока.
int ltl_detach(ltl_thread_t thread);
/// Принудительное завершение потока.
int ltl_destroy(ltl_thread_t thread);
/// Принудительное завершение потока.
void ltl_destroy0(ltl_thread_t thread);
/// Освобождение памяти после завершения потока.
void ltl_free(ltl_thread_t thread);
/// Освобождение памяти после завершения потока.
void ltl_free0(ltl_thread_node_t* thread);

#endif // __LTL_THREAD_H__
