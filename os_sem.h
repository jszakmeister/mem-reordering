#ifndef INCLUDED_OS_SEM
#define INCLUDED_OS_SEM

#ifdef __cplusplus
extern "C" {
#endif

#if OS_DARWIN

#include <mach/mach.h>

typedef semaphore_t os_sem_t;

#define os_sem_init(s, count) \
    semaphore_create(mach_task_self(), (s), SYNC_POLICY_FIFO, (count))

#define os_sem_wait(s) semaphore_wait(*(s))

#define os_sem_post(s) semaphore_signal(*(s))

#else

#include <semaphore.h>

typedef sem_t os_sem_t;

#define os_sem_init(s, count) sem_init((s), 0, (count))

#define os_sem_wait(s) sem_wait(s)

#define os_sem_post(s) sem_post(s);

#endif


#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_OS_SEM */
