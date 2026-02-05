#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

#define NC            "\e[0m"
#define COLOR_RED     "\e[1;31m"
#define COLOR_GREEN   "\e[0;32m"
#define COLOR_YELLOW  "\e[1;33m"
#define COLOR_BLUE    "\e[1;34m"
#define COLOR_MAGENTA "\e[0;35m"
#define COLOR_CYAN    "\e[0;36m"
#define COLOR_BLACK   "\e[1;30m"
#define COLOR_WHITE   "\e[1;37m"

typedef struct thread_data thread_data_t;

#define msleep(milliseconds) usleep((unsigned int)milliseconds*1000)
/*void msleep(unsigned int wait_ms)
{
    struct timespec ts;
    ts.tv_sec = wait_ms / 1000;
    ts.tv_nsec = (wait_ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}*/

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    thread_data_t* thread_data_p = 0;
    if (thread_param == 0) return thread_data_p;
    thread_data_p = (thread_data_t *) thread_param;

    printf("%sThread [%lu] function started.%s\n", COLOR_YELLOW, (unsigned long)pthread_self(), NC);
    thread_data_p->thread_complete_success = false; //false as default

    //sleeps wait_to_obtain_ms number of milliseconds
    msleep(thread_data_p->wait_to_obtain_ms);

    //*** critical section ***
    //obtain the mutex
    //lock a mutex with the blocking
    int rc = pthread_mutex_lock(thread_data_p->mutex_p);
    if (rc != 0) {
    //    printf("pthread_mutex_lock failed with %d\n", rc);
        return thread_data_p;
    }

    //holds for wait_to_release_ms milliseconds
    msleep(thread_data_p->wait_to_release_ms);

    //releases
    rc = pthread_mutex_unlock(thread_data_p->mutex_p);
    //*** end critical section ***
    if (rc != 0) {
    //    printf("pthread_mutex_unlock failed with %d\n", rc);
        return thread_data_p;
    }

    //thread completed successfully, mutex is unlocked
    thread_data_p->thread_complete_success = true;
    printf("%sThread [%lu] function stoped with result %s%s%s.\n", COLOR_RED, (unsigned long)pthread_self(), COLOR_BLUE, thread_data_p->thread_complete_success ? "true" : "false", NC);

    return thread_data_p;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex, int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

    thread_data_t* thread_param_p = (thread_data_t *)malloc(sizeof(thread_data_t));
    if (!thread_param_p) {
        printf("Memory allocation failure.\n");
        return false;
    }

    thread_param_p->wait_to_obtain_ms = (unsigned int)wait_to_obtain_ms;
    thread_param_p->wait_to_release_ms = (unsigned int)wait_to_release_ms;
    thread_param_p->mutex_p = mutex;
    thread_param_p->thread_complete_success = false;

    int ret = pthread_create(thread,
                             NULL,
                             threadfunc,
                             (void *) thread_param_p);
    //printf("PThread_create returned %d\n", ret);

    if (ret == 0) {
        // Thread successfully started
        // Get the created thread ID
        printf("%sCreated thread-ID: [%ld]%s\n", COLOR_GREEN, *thread, NC);
        return true;
    }

    // Thread creation failed
    errno = ret;
    perror("pthread_create");
    free((thread_data_t*)thread_param_p); // Free memory if thread creation fails
    thread_param_p = NULL;

    return false;
}

// EOF
