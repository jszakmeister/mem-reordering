#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "mersenne.h"
#include "os_sem.h"

#ifdef USE_SINGLE_CPU
#ifdef OS_DARWIN
#include <mach/thread_policy.h>
#else
#include <sched.h>
#endif
#endif

#ifdef USE_FENCE
// Prevent CPU reordering
#define fence() do { __asm volatile("mfence" ::: "memory"); } while (0)
#else
// Prevent compiler reordering
#define fence() do { __asm volatile("" ::: "memory"); } while (0)
#endif


os_sem_t start_sem1;
os_sem_t start_sem2;
os_sem_t end_sem;


int X, Y;
int r1, r2;


void *thread1_func(void *param)
{
    (void) param;

    mersenne_t m;
    mersenne_init(&m, 1);

    for (;;)
    {
        os_sem_wait(&start_sem1);

        while (mersenne_next(&m) % 8 != 0)
        {
        }

        X = 1;

        fence();

        r1 = Y;

        os_sem_post(&end_sem);
    }

    return NULL;
};

void *thread2_func(void *param)
{
    (void) param;

    mersenne_t m;
    mersenne_init(&m, 2);

    for (;;)
    {
        os_sem_wait(&start_sem2);  // Wait for signal

        while (mersenne_next(&m) % 8 != 0)
        {
        }

        Y = 1;

        fence();

        r2 = X;

        os_sem_post(&end_sem);
    }

    return NULL;
};


int
main(void)
{
    os_sem_init(&start_sem1, 0);
    os_sem_init(&start_sem2, 0);
    os_sem_init(&end_sem, 0);

    pthread_t thread1, thread2;

#if !defined(OS_DARWIN) || !defined(USE_SINGLE_CPU)
    pthread_create(&thread1, NULL, thread1_func, NULL);
    pthread_create(&thread2, NULL, thread2_func, NULL);
#endif

#ifdef USE_SINGLE_CPU
    // Force thread affinities to the same cpu core.

#ifdef OS_DARWIN
    #error "Darwin's thread affinity is actually L2 affinity, won't work for us"

    thread_affinity_policy_data_t affinity_policy = { 1 };

    if (pthread_create_suspended_np(&thread1, NULL, thread1_func, NULL) != 0)
        abort();

    if (pthread_create_suspended_np(&thread2, NULL, thread2_func, NULL) != 0)
        abort();

    mach_port_t mach_thread1 = pthread_mach_thread_np(thread1);
    mach_port_t mach_thread2 = pthread_mach_thread_np(thread2);

    if (thread_policy_set(mach_thread1, THREAD_AFFINITY_POLICY,
                          (thread_policy_t) &affinity_policy,
                          THREAD_AFFINITY_POLICY_COUNT) != 0)
        abort();

    if (thread_policy_set(mach_thread2, THREAD_AFFINITY_POLICY,
                          (thread_policy_t) &affinity_policy,
                          THREAD_AFFINITY_POLICY_COUNT) != 0)
        abort();

    thread_resume(mach_thread1);
    thread_resume(mach_thread2);
#else
    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    CPU_SET(0, &cpus);
    pthread_setaffinity_np(thread1, sizeof(cpu_set_t), &cpus);
    pthread_setaffinity_np(thread2, sizeof(cpu_set_t), &cpus);
#endif

#endif

    int reorders = 0;
    for (int iterations = 1; ; iterations++)
    {
        // Reset X and Y.
        X = 0;
        Y = 0;

        // Start both threads.
        os_sem_post(&start_sem1);
        os_sem_post(&start_sem2);

        // Wait for both threads.
        os_sem_wait(&end_sem);
        os_sem_wait(&end_sem);

        // Check if there was a simultaneous reorder
        if (r1 == 0 && r2 == 0)
        {
            reorders++;
            printf("%d reorders detected after %d iterations\n",
                reorders, iterations);
        }
    }

    return 0;
}
