#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define LEVELS 7
#define SUPER_BLOCK_SIZE 4096
static inline int size2level (ssize_t size) {
    /* Your code here.
     * Convert the size to the correct power of two. 
     * Recall that the 0th entry in levels is really 2^5, 
     * the second level represents 2^6, etc.
     */
   	int start_entry = 5;
    int reVal = -1;
    for (int i = start_entry; i < start_entry+LEVELS; i++ ){
        // printf("%d\n",1<<i);
        if(size <= 1<<i){
            reVal = i - start_entry;
            break;
        }
    }

    return reVal;
}

static inline int size2level2 (ssize_t size) {
    /* Your code here.
     * Convert the size to the correct power of two.
     * Recall that the 0th entry in levels is really 2^5,
     * the second level represents 2^6, etc.
     */
    int level;
    #define MIN_ALLOC 32 /* Smallest real allocation.  Round smaller mallocs up */
    #define MAX_ALLOC 2048 

    if (size <= MIN_ALLOC){
        //if less than or equal to 32, then it is automatically set to level 0
        level = 0;
    } else {
        //else we calculate the level based on size = 32*2^level
        level = log2((int)(size/32));
        if ((size%32) != 0){
            //if there is a remainder we have to round up a level
            level++;
        }
    }
    return level;
}

int main(){
	// int result = size2level(2018);
	// printf("%d\n",result);
	int power = 0;
	printf("%d\n", SUPER_BLOCK_SIZE/(1 << (power+5)));
	printf("%d\n",SUPER_BLOCK_SIZE >> ((power + 5) - 1));
	printf("%d\n",SUPER_BLOCK_SIZE/(32*(1<<power)));
}