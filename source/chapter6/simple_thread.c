/*
 * $file : simple_thread.c
 * $desc : demonstrate a simple multi-thread example
 * $author : rockins
 * $date : Sun, 11 May 2008 21:57:59 +0800
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
 * preserve thread identification of A and B
 */
static pthread_t thA;
static pthread_t thB;

/*
 * counter for Thread A and B
 */
static long cntA = 0;
static long cntB = 0;

/*
 * a *BIG* lock
 */
static pthread_mutex_t gMux = PTHREAD_MUTEX_INITIALIZER;

/*
 * common thread body for both thread A and B.
 * print the thread which is in ThreadBody(),
 * protected by gMux.
 */
static void * ThreadBody(void * arg)
{
    pthread_t tid;
    int verdict;

    while (1) {
        tid = pthread_self();

        verdict = pthread_equal(tid, thA);
        if (verdict) {
            pthread_mutex_lock(&gMux);
            cntA++;
            printf("in Thread A: %ld\n", cntA);
            pthread_mutex_unlock(&gMux);
            continue;
        }

        verdict = pthread_equal(tid, thB);
        if (verdict) {
            pthread_mutex_lock(&gMux);
            cntB++;
            printf("in Thread B: %ld\n", cntB);
            pthread_mutex_unlock(&gMux);
        }
    };
}

int main(void)
{
    int ret;

    ret = pthread_create(&thA, NULL, ThreadBody, NULL);
    if (ret != 0) {
        fprintf(stderr, "create thread A faild\n");
        exit(-1);
    }

    ret = pthread_create(&thB, NULL, ThreadBody, NULL);
    if (ret != 0) {
        fprintf(stderr, "create thread A faild\n");
        exit(-1);
    }

    /*
     * wait for thread A and B exit
     */
    pthread_join(thA, NULL);
    pthread_join(thB, NULL);

    return (0);
}

