/*  Andy Sisinger
    Project 1 - Multithreaded Sorting Algorithm
    10/12/17
    COMP323
*/

#include <stdio.h>    /* For basic IO */
#include <stdlib.h>   /* For rand() */
#include <pthread.h>  /* For pthreads */
#include <time.h>     /* needed for the time seed */
#include "proj1.h"    /* this project header file */

/* a struct to hold the parameters for calling my thread sort functions */
struct subarray {
  int *index; /* a pointer to the start of the subarray */
  int length; /* we need to know the length of our subarray so we don't go out of bounds */
};

/* a struct to hold parameters for calling the merge thread function */
struct  mergeParam {
  int *orig; /* pointer to original array */
  int *merged; /* pointer to the array we are putting the merged results into */
  int size; /* the size of our arrays */
};

int main(void)
{
  /* initializes the random seed with current time so we get different random arrays each time we run */
  srand(time(NULL));

  /* Print header and ask for input. Loop until they correctly enter a non-zero, non-negative number */
  printf("\nAndy Sisinger - COMP323 Fall 2017\nProject 1 - Multithreaded Sorting Algorithm\n");
  printf("Warning - Console ouput may be interleaved (appear to be scrambled) due to multithreading!\n\n");
  int size = 0; /* size of random array to be sorted */
  int ask = 1; /* used as a boolean for loop */
  char raw[20]; /* very small raw input buffer */
  while (ask) {
    printf("Please enter the number of integers to sort: ");
    fgets(raw, 20, stdin);
    size = atoi(raw); /* try to get something out of the raw buffer */
    if (size > 1) {
      ask = 0;
      break;
    } else {
      printf("Please enter an integer greater than or equal to 2. (Sorting one number isn't very interesting)\n");
    }

  }


  /* allocate the appropriate size of memory for our int array */
  int *nums = calloc(size, sizeof(int));
  if (nums==NULL) exit (1);
  /* fill the original array with random numbers */
  for (int i=0; i<size; i++) {
    nums[i] = rand() % RAND_NUM_HIGH_LIMIT;  /* pick a random number from 0 to 99 */
  }

  /* for debug purposes, print out the original list */
  printf("\nHere are the %d numbers to sort:\n", size);
  for (int i=0; i<size; i++) {
    printf("%d", nums[i]);
    if (size-i == 1) {
        printf("\n");
    } else {
      printf(", ");
    }
  }

  /* declare our thread variables */
  struct subarray sa1, sa2; /* sub array structs */
  pthread_t s1_thr, s2_thr, merge_thr; /* two sort threads, and one merge thread */
  pthread_attr_t attr; /* Thread attributes (using defaults here) */

  /* Calculate subarray lengths from size of nums, and deal with odd sizes */
  sa1.index = &nums[0]; /* first subarray will always start at begging of orig array */
  sa1.length = size/2; /*the length of subarray1 is the floor division result */
  sa2.index = &nums[size/2]; /* second subarray starts halfway */
  sa2.length = size/2; /* second subarray length is half total length unless... */
  if (size%2) { /* if odd size */
    sa2.length++; /* need to add one to length for odd subarray if odd orig array */
  }
  /* Create our two sorting threads */
  pthread_attr_init(&attr); /* initialize attributes */
  pthread_create(&s1_thr, &attr, sort_thread, &sa1);
  pthread_create(&s2_thr, &attr, sort_thread, &sa2);
  /* we wait till they complete */
  pthread_join(s1_thr, NULL);
  pthread_join(s2_thr, NULL);

  /* initialize the final answer array, merge thread will put answers into this array */
  int *answer = calloc(size, sizeof(int));
  if (answer==NULL) exit (1);
  /* make parameters for merge thread, original array, and merged answer array */
  struct mergeParam mergeParams = { .orig = nums, .merged = answer, .size = size };

  /* Now create our final merge and sort thread */
  pthread_create(&merge_thr, &attr, merge_thread, &mergeParams);
  pthread_join(merge_thr, NULL);

  /* Print the final sorted array. The answer at last! */
  printf("\nThe answer is: \n");
  for (int i=0; i<size; i++) {
    printf("%d", answer[i]);
    if (size-i == 1) {
        printf("\n");
    } else {
      printf(", ");
    }
  }

  free(nums);
  return 0;
}

void *sort_thread(void *param)
{
  struct subarray *s = (struct subarray *)param;
  printf("\n - A mysort thread is working on subarray: ");
  for (int i=0; i<s->length; i++) {
    printf("%d ", *(s->index+i));
  }
  printf("\n");

  /* call my select sort on the sub array */
  myselectsort(s->index, s->length);

  /* now print sorted (hopefully!) list */
  printf(" - thread sorted subarray: ");
  for (int i=0; i<s->length; i++) {
    printf("%d ", *(s->index+i));
  }
  printf("\n");

  pthread_exit(NULL);
}

void *merge_thread(void *param)
{
  /* make the passed void param the correct merge struct param */
  struct mergeParam *p = (struct mergeParam *)param;
  printf("\n - The mymerge thread is working... \n");

  /* simple interleave of the the two subarrays, this will make for a fast bubblesort */
  int j=0;
  for (int i=0; i<p->size; i++) {
    if (p->orig[i] < p->orig[p->size/2]) {
      p->merged[j] = p->orig[i];
      p->merged[j+1] = p->orig[p->size/2 + i];
    } else {
      p->merged[j] = p->orig[p->size/2 + i];
      p->merged[j+1] = p->orig[i];
    }
    j = j + 2;
  }

  /* now lets bubblesort the (almost sorted) merged array */
  mybubblesort(p->merged, p->size);

  pthread_exit(NULL);
}

void mybubblesort(int *array, int size) {

  int swapped; /* using as boolean value to check for bubblesort completion */
  do {
    swapped = 0; /* reset our loop flag */
    for (int i=0; i<size-1; i++) {
      if (array[i] > array[i+1]) {
        int tmp = array[i];
        array[i] = array[i+1];
        array[i+1] = tmp;
        swapped = 1;
      }
    }
  } while (swapped); /* the loop is complete when we went through whole array without swapping anymore */
}

void myselectsort(int *array, int size) {
  for (int i=0; i<size; i++) {
    int mindex = i;
    int min = array[i];
    for (int j=i; j<size; j++) {
      if (array[j] < min) {
        mindex = j;
        min = array[j];
      }
    }
    /* swap the new found minimum value */
    int tmp = array[i];
    array[i] = array[mindex];
    array[mindex] = tmp;
  }
}
