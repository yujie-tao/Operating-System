/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */
/* vim: set ts=4 sw=4 et tw=78 fo=cqt wm=0: */

/* @* Yujie Tao *@
 * @* Multithreading using coarse-grained locking *@
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "lru.h"

/* Define the simple, singly-linked list we are going to use for tracking lru */
struct list_node {
    struct list_node* next;
    int key;
    int refcount;
};

static struct list_node* list_head = NULL;

/* A static mutex */
static pthread_mutex_t mutex;
static int count = 0;
static pthread_cond_t cv_low, cv_high;

static volatile int done = 0;

/* Initialize the mutex.
 * Return 1 when success, return 0 when fail*/
int init (int numthreads) {
    /* Create numthreads number of threads */
    if (pthread_mutex_init(&mutex, NULL) != 0){
        return 0;
    }

    if (pthread_cond_init(&cv_low, NULL) != 0){
        return 0;
    }

    if (pthread_cond_init(&cv_high, NULL) != 0){
        return 0;
    }

    //return 1 on success
    return 1;
}

/* Return 1 on success, 0 on failure.
 * Should set the reference count up by one if found; add if not.*/
int reference (int key) {
    /* Add global lock*/
    pthread_mutex_lock(&mutex);

    //Wait if count higher than high water mark
    while(count >= HIGH_WATER_MARK){
        pthread_cond_wait(&cv_high, &mutex);
    }

    int found = 0;
    struct list_node* cursor = list_head;
    struct list_node* last = NULL;
    while(cursor){
        if (cursor->key < key) {
            last = cursor;
            cursor = cursor->next;
        } else {
            if (cursor->key == key) {
                cursor->refcount++;
                found++;
            }
            break;
        }
    }

    if(!found){
        struct list_node* new_node = malloc(sizeof(struct list_node));
        if (!new_node) return 0;
        count++;
        new_node->key = key;
        new_node->refcount = 1;
        new_node->next = cursor;

        if (last == NULL)
            list_head = new_node;
        else
            last->next = new_node;
    }


    //Signal clean() when count above low water mark
    if (count > LOW_WATER_MARK){
        pthread_cond_signal(&cv_low);
    }

    //Unlock global lock, throw error when not success
    if(pthread_mutex_unlock(&mutex)!=0){
        perror("mutex unlock error");
        exit(1);
    }

    return 1;
}

/* Do a pass through all elements, either clear the reference bit,
 * or remove if it hasn't been referenced since last cleaning pass.
 *
 * check_water_mark: If 1, block until there are more elements in the cache
 * than the LOW_WATER_MARK.  This should only be 0 during self-testing or in
 * single-threaded mode.
 */
void clean(int check_water_mark) {
    //Add global lock
    pthread_mutex_lock(&mutex);

    //Wait if check is on and count lower or equal to low water mark
    if (check_water_mark == 1){
        while(count <= LOW_WATER_MARK){
            pthread_cond_wait(&cv_low, &mutex);
        }
    }

    struct list_node* cursor = list_head;
    struct list_node* last = NULL;

    while(cursor){
        cursor->refcount--;
        if (cursor->refcount == 0) {
            struct list_node* tmp = cursor;
            if (last) {
                last->next = cursor->next;
            } else {
                list_head = cursor->next;
            }
            tmp = cursor->next; 
            free(cursor);
            cursor = tmp;
            count--;
        } else {
            last = cursor;
            cursor = cursor->next;
        }           
    }

    //Signal reference() when count lower than high water mark
    if (count < HIGH_WATER_MARK){
        pthread_cond_signal(&cv_high);
    }

    //Unlock global lock, throw error when not sucess
    if(pthread_mutex_unlock(&mutex)!=0){
        perror("mutex unlock error");
        exit(1);
    }

}

/* Optional shut-down routine to wake up blocked threads.
   May not be required. */
void shutdown_threads () {
    //Signal all sleeping threads
    pthread_cond_signal(&cv_low);
    pthread_cond_signal(&cv_high);

    return;
}

/* Print the contents of the list.  Mostly useful for debugging. */
void print () {
    printf("=== Starting list print ===\n");
    printf("=== Total count is %d ===\n", count);
    struct list_node* cursor = list_head;
    while(cursor) {
        printf ("Key %d, Ref Count %d\n", cursor->key, cursor->refcount);
        cursor = cursor->next;
    }
    printf("=== Ending list print ===\n");
}
