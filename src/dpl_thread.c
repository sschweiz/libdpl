/**
 * @file dpl_thread.c
 * @brief Sourcecode for functions listed in dpl_thread.h
 */

#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "dpl_thread.h"

extern int DPL_HEART_ALIVE;

void dpl_signal_heartbeat(int s)
{
	DPL_HEART_ALIVE ^= 1;
}

void *dpl_beat(void *args)
{
	dpl_heartbeat_t *t = (dpl_heartbeat_t *)args;

	while (DPL_HEART_ALIVE)
	{
		printf("Heartbeat found.\n");
		sleep(t->frequency);
	}

	pthread_exit(NULL);
}

int dpl_init_heartbeat(dpl_heartbeat_t *b, int freq)
{
	int rc;
	
	b->frequency = freq;

	DPL_HEART_ALIVE = 1;

	/* set the system sig call for heartbeat */
	signal(SIGBEAT, dpl_signal_heartbeat);

	pthread_attr_init(&(b->beat_attr));
	/* pthread_attr_setdetachstate(&(b.beat_attr), PTHREAD_CREATE_JOINABLE); */
	pthread_mutex_init(&(b->beat_mutex), NULL);
	pthread_cond_init(&(b->beat_cond), NULL);

	rc = pthread_create(&(b->beat_thread), &(b->beat_attr), dpl_beat, (void *)b);
	if (rc != 0) { perror("pthread failed to create heartbeat!."); }

	return rc;
}

void dpl_destroy_heartbeat(dpl_heartbeat_t *b)
{
	pthread_attr_destroy(&(b->beat_attr));
	pthread_mutex_destroy(&(b->beat_mutex));
	pthread_cond_destroy(&(b->beat_cond));

	pthread_cancel(b->beat_thread);

	pthread_exit(NULL);
}
