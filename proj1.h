/*  Andy Sisinger
    Project 1 - Header file
*/

#define RAND_NUM_HIGH_LIMIT 100 /* largest number to randomly generate, non-inclusive */
void *sort_thread(void *param);
void *merge_thread(void *param);
void mybubblesort(int *array, int size);
void myselectsort(int *array, int size);
