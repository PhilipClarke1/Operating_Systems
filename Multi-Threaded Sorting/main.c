#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Initializing structs
struct sortingParams {
  double *arr;
  int size;
};
struct mergingParams{
double* first;
int firstsize;
double *second;
int secondsize;
double *dest; 
};
// Function to merge threads
void * merge(void *arg) {
  // Load the struct 
  struct mergingParams *x = arg;
  // Get len
  int len = x->firstsize + x->secondsize;
  // Check to see if through array 
  int over1 = 0;
  int over2 = 0;
  // Iterate through array 
  int check1 = 0;
  int check2 = 0;
  for (int i = 0; i < len; i++) {
    // Sees if though array
    if(check1 == x->firstsize) {
      over1 = 1;
    }
    if (check2 == x->secondsize) {
      over2 = 1;
    }
    if((over1 == 0) && (over2 == 0)) {
      if(x->first[check1] < x->second[check2]) {
        x->dest[i] = x->first[check1];
        check1 += 1;
    }else{
        x->dest[i] = x->second[check2];
        check2 += 1;
    }
    }
    // Fill dest with array 1 if 2 if full and vice versa 
    if(over1 == 1) {
      x->dest[i] = x->second[check2];
      check2 += 1;
    }
     if(over2 == 1) {
      x->dest[i] = x->first[check1];
      check1 += 1;
    }
    
  }
  return 0;
}

// Generating random numbers for arrays
double randGenerate(double min, double max) {
  // generate random int and convert it to double
  double random1 = ((double)rand() / RAND_MAX);
  double range = random1 * (max - min);
  double num = min + range;
  return num;
}
// For sort function to swap two elements of array
void swap(double *p1, double *p2) {

  double temp = *p1;
  *p1 = *p2;
  *p2 = temp;
}
// Since pthread_create expects a void pointer, this function must take a void
// pointer then is cast to struct sortingParams
void *sort(void *data) {
  struct sortingParams *a = data;
  // for loop to increase the minimum index with every step
  // -> used to access a member of a struct through a pointer
  for (int i = 0; i < a->size - 1; i++) {
    int min_index = i;
    for (int k = i + 1; k < a->size; k++) {
      if (a->arr[k] < a->arr[min_index]) {
        min_index = k;
      }
    }
    // passing pointers to the swap function to swap values
    if (min_index != i) {
      swap(&a->arr[min_index], &a->arr[i]);
    }
  }
  // Show that the function has completed
  return NULL;
}

int main(int argc, char *argv[]) {

  // Make the threads 
  pthread_t p1, p2, p3, threadM;


  // Declare seed using the current time for the RAND function
  srand(time(NULL));

  // Check to ensure that the proper number of arguments is suppied
  if (argc > 2) {
    printf("Too many arguments supplied.\n");
    return 0;
  } else if (argc != 2) {
    printf("One argument expected.\n");
    return 0;
  }

  // Read input and convert from char* to an int
  const char *str = argv[1];
  int y = atoi(str);

  if (y < 1) {
    printf("Enter a positive integer.\n");
    return 0;
  }

  // Create timespec
  struct timespec ts_begin, ts_end;
	double elapsed;
  
  // Declare array of size y
  double array[y];

  // Fill array
  for (int i = 0; i < y; i++) {
    // Generate array of random numbers
    double current = randGenerate(1, 1000);
    array[i] = current;
  }
  // Create other arrays 
  double arrayb[y];
  double firsthalf[y / 2];
  double secondhalf[y - (y/2)];
  double arrayc[y];
  
  int arr1_len = sizeof(firsthalf) / sizeof(firsthalf[0]);
  int arr2_len = sizeof(secondhalf) / sizeof(secondhalf[0]); 

  // Make arrayb the same as array
  for(int i = 0; i < y; i++) {
    arrayb[i] = array[i];
  }

  // Get Starting time
  clock_gettime(CLOCK_MONOTONIC, &ts_begin);

  // Create struct 
  struct sortingParams arr_data;
  arr_data.arr = arrayb;
  arr_data.size = y;

  // Create and join the thread
  pthread_create(&p1, NULL, sort, &arr_data);
  pthread_join(p1, NULL);

  clock_gettime(CLOCK_MONOTONIC, &ts_end);  
	elapsed = ts_end.tv_sec - ts_begin.tv_sec; 
	elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) /       1000000000.0;

  printf("\nSorting by ONE thread is done in: %f ms", elapsed*1000);

  // Split array[] into firsthalf[] and secondhalf[]
  for(int i = 0; i < y; i++) {
    if(i < (y / 2)) {
       firsthalf[i] = array[i];
    }else {
      secondhalf[i - y/2] = array[i];
    }
  }

  // Start clock 
  clock_gettime(CLOCK_MONOTONIC, &ts_begin);
  

  // Make the necessary structs 
  struct sortingParams first_data;
  first_data.arr = firsthalf;
  first_data.size = arr1_len;

  struct sortingParams second_data;
  second_data.arr = secondhalf;
  second_data.size = arr2_len;

  // create threads 
  pthread_create(&p2, NULL, sort, &first_data);
  pthread_create(&p3, NULL, sort, &second_data);
  
  // join threads
  pthread_join(p2, NULL);
  pthread_join(p3, NULL);

  // Create Merging Struct
  struct mergingParams merg;
  merg.first = firsthalf;
  merg.firstsize = y/2;
  merg.second = secondhalf;
  merg.secondsize = y - y/2;
  merg.dest = arrayc;

  // Create thread for merging
  pthread_create(&threadM, NULL, merge, &merg);

  //Join the thread
  pthread_join(threadM, NULL);

  // Stop the clock 
  clock_gettime(CLOCK_MONOTONIC, &ts_end);  
	elapsed = ts_end.tv_sec - ts_begin.tv_sec; 
	elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) /       1000000000.0;

  // Print Elapsed time
  printf("\nSorting by Two threads is done in: %f ms", elapsed*1000);
  return 0;

}
