/**
 * @brief DPL thread and heartbeat control header
 * @file dpl_thread.h
 */
#ifndef __DPL_THREAD_H__
#define __DPL_THREAD_H__

#include <pthread.h>

/**
 * @brief Heartbeat transition signal
 *
 * This is a signal that can be used by the user to turn on or off the
 * heartbeat (as long as DPL_HEARTBEAT is specified in init).  However,
 * it will not interupt the current heartbeat.  It will only take affect
 * after the current heartbeat is finished.  You can use it anywhere in
 * your code by doing the following:
 * @code
 * #include <signal.h>
 * ...
 * raise(SIGBEAT);
 * @endcode
 */
#define SIGBEAT 0x0f3c

int DPL_HEART_ALIVE;

/** @brief DPL threaded heartbeat structure */
typedef struct __dplthreadstruct
{
	/** @brief heartbeat frequency in seconds */
	int frequency;
	/** @brief heartbeat thread structure */
	pthread_t	beat_thread;
	/** @brief heartbeat thread mutex structure */
	pthread_mutex_t	beat_mutex;
	/** @brief heartbeat thread condition variable structure*/
	pthread_cond_t	beat_cond;
	/** @brief heartbeat thread attribute structure */
	pthread_attr_t	beat_attr;
} dpl_heartbeat_t;

/**
 * @brief Initializes the heartbeat structure and creates the thread
 * @param b pointer to heartbeat structure
 * @param freq desired heartbeat frequency
 */
int dpl_init_heartbeat(dpl_heartbeat_t *b, int freq);

/**
 * @brief Destroys the heartbeat structure and terminates the thread
 * @param b pointer to heartbeat structure
 */
void dpl_destroy_heartbeat(dpl_heartbeat_t *b);

/**
 * @brief Function to handle signal generated by SIGBEAT
 * @param s signal
 */
void dpl_signal_heartbeat(int s);

/**
 * @brief Heartbeat thread function
 * @param args void* typecast of the heartbeat structure
 */
void *dpl_beat(void *args);

#endif
