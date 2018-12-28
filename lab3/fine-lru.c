/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */
/* vim: set ts=4 sw=4 et tw=78 fo=cqt wm=0: */

/* @* Yujie Tao *@
 * @* that deanonymize your work inside this syntax *@
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
    // Protects this node's contents
    pthread_mutex_t mutex;
};

static struct list_node* list_head = NULL;

/* A static mutex; protects the count and head.
 * XXX: We will have to tolerate some lag in updating the count to avoid
 * deadlock. */
static pthread_mutex_t mutex;
static int count = 0;
static pthread_cond_t cv_low, cv_high;

static volatile int done = 0;

/* Initialize the mutex. */
int init (int numthreads) {
    if (pthread_mutex_init(&mutex, NULL) != 0){
        return 0;
    }  

    if (pthread_cond_init(&cv_low, NULL) != 0){
        return 0;
    }

    if (pthread_cond_init(&cv_high, NULL) != 0){
        return 0;
    }

    return 0;
}

/* Return 1 on success, 0 on failure.
 * Should set the reference count up by one if found; add if not.*/
int reference (int key) {

    while(count >= HIGH_WATER_MARK){
        pthread_cond_wait(&cv_high, &mutex);
    }

    /* Add global lock to initialize found, cursor, last*/
    pthread_mutex_lock(&mutex);
    int found = 0;
    struct list_node* cursor = list_head;
    struct list_node* last = NULL; 

    /* Unlock global lock to release access to found, cursor, last*/
    pthread_mutex_unlock(&mutex);    

    while(cursor) {
        if (cursor->key < key) {
            last = cursor;    
            cursor = cursor->next;          
        } else {
            if (cursor->key == key) {
                cursor->refcount++;
                /* Add global lock to protect found*/
                pthread_mutex_lock(&mutex);
                found++;
                /* Unlock global lock to release access to found*/
                pthread_mutex_unlock(&mutex);
            }
            break;
        }
    }

    if (!found) {
        // Handle 2 cases: the list is empty/we are trying to put this at the
        // front, and we want to insert somewhere in the middle or end of the
        // list.
        /* Add global lock to protect count*/
        pthread_mutex_lock(&mutex);
        struct list_node* new_node = malloc(sizeof(struct list_node));
        if (!new_node) return 0;
        count++;
        /* Unlock global lock to release access to count*/
        pthread_mutex_unlock(&mutex);
        new_node->key = key;
        new_node->refcount = 1;
        new_node->next = cursor;

        if (last == NULL){
            /* Add global lock to protect list_head*/
            pthread_mutex_lock(&mutex);
            list_head = new_node;
            /* Unlock global lock to release access to list_head*/
            pthread_mutex_unlock(&mutex);
        }else{
            /* Add lock to protect last node*/
            pthread_mutex_lock(&last->mutex);
            last->next = new_node;
            /* Unlock to release access to last node*/
            pthread_mutex_unlock(&last->mutex);
        }
    }

    //Signal clean() when count above low water mark
    if (count > LOW_WATER_MARK){
        pthread_cond_signal(&cv_low);
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
    //Wait if check is on and count lower or equal to low water mark
    if (check_water_mark == 1){
        while(count <= LOW_WATER_MARK){
            pthread_cond_wait(&cv_low, &mutex);
        }
    }

    /* Add global lock to initialize cursor, last*/
    pthread_mutex_lock(&mutex);

    struct list_node* cursor = list_head;
    struct list_node* last = NULL;

    /* Unlock global lock to release access to cursor, last*/
    pthread_mutex_unlock(&mutex);

    while(cursor){
        cursor->refcount--;
        if (cursor->refcount == 0) {
            struct list_node* tmp = cursor;
            if (last) {
                /* Add lock to protect last*/
                pthread_mutex_lock(&last->mutex);
                last->next = cursor->next;
                /* Unlock to release access to last*/
                pthread_mutex_unlock(&last->mutex);
            } else {
               /* Add global lock to protect list_head*/
               pthread_mutex_lock(&mutex);
               list_head = cursor->next;
               /* Unlock global lock to release access to list_head*/
               pthread_mutex_unlock(&mutex);
            }
            tmp = cursor->next; 
            free(cursor);
            cursor = tmp;
            /* Add global lock to protect count*/
            pthread_mutex_lock(&mutex);
            count--;
            /* Unlock global lock to release access to count*/
            pthread_mutex_unlock(&mutex);
        } else {
            last = cursor;
            cursor = cursor->next;
        }           
    }

    //Signal reference() when count lower than high water mark
    if (count < HIGH_WATER_MARK){
        pthread_cond_signal(&cv_high);
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
