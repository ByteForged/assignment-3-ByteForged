#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Optional: use these functions to add debug or error prints to your application
//#define DEBUG_LOG(msg,...)
#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)
#define MS_TO_US(_ms) ((_ms) * 1000)

void* threadfunc(void* thread_param)
{
    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    struct thread_data* td = (struct thread_data*)thread_param;
    if (td) {
        if (usleep(MS_TO_US(td->wait_to_obtain_ms)) == 0) {
            if (pthread_mutex_lock(td->mutex) == 0) {
                bool waited = false;
                if (usleep(MS_TO_US(td->wait_to_release_ms)) == 0) {
                    waited = true;
                } else {
                    ERROR_LOG("Sleep before unlocking mutex failed - %s", strerror(errno));
                }
                if (pthread_mutex_unlock(td->mutex) == 0) {
                    td->thread_complete_success = waited;
                } else {
                    ERROR_LOG("Unlocking mutex failed - %s", strerror(errno));
                }
            } else {
                ERROR_LOG("Locking mutex failed - %s", strerror(errno));
            }
        } else {
            ERROR_LOG("Sleep before locking mutex failed - %s", strerror(errno));
        }
        
    } else {
        ERROR_LOG("Thread param is NULL");
    }

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    if (thread && mutex && (wait_to_obtain_ms >= 0) && (wait_to_release_ms >= 0)) {
        struct thread_data* td = (struct thread_data*)malloc(sizeof(struct thread_data));
        if (td) {
            td->mutex = mutex;
            td->wait_to_obtain_ms = wait_to_obtain_ms;
            td->wait_to_release_ms = wait_to_release_ms;
            td->thread_complete_success = false;
            if (pthread_create(thread, NULL, &threadfunc, td) == 0) {
                return true;
            } else {
                ERROR_LOG("Failed to create pthread - %s", strerror(errno));
            }
        } else {
            ERROR_LOG("Failed to alloc memory for thread parameter");
        }
    } else {
        ERROR_LOG("Bad Arg(s): thread(%s), mutex(%s), wait_to_obtain_ms(%s), wait_to_release_ms(%s)",
            (thread                  ? "OK" : "BAD - NULL"),
            (mutex                   ? "OK" : "BAD - NULL"),
            (wait_to_obtain_ms  >= 0 ? "OK" : "BAD - NEG" ),
            (wait_to_release_ms >= 0 ? "OK" : "BAD - NEG" )
        );
    }

    return false;
}

