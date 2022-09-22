#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMTRAINS 10
#define NUMINTERSECTIONS 5

//threads
pthread_t threads[NUMTRAINS];
int *taskids[NUMTRAINS];

//creates array of intersections with 0 trains on each
int intersections[NUMINTERSECTIONS] = {0}; 

//mutexes to control access to the intersections array
pthread_mutex_t intersectionsMutexes[NUMINTERSECTIONS];

//condition variables, one for each intersection/mutex
pthread_cond_t condVariables[NUMINTERSECTIONS];


void* train(void* trainNum){
    int trainId = *(int *)trainNum;

    while(1){
        for(int i = 0; i < NUMINTERSECTIONS; i++ ){
            pthread_mutex_lock(&intersectionsMutexes[i]);
                while(intersections[i] >= 2){
                    //wait using condition variables
                    printf("Train %d at intersection %d, but busy. Waiting... \n", trainId, i);
                    pthread_cond_wait(&condVariables[i], &intersectionsMutexes[i]);
                }
                //if it gets here, there is less than 2 trains at the intersection

                //increment intersection counter
                intersections[i]++;
                //unlock the mutex
                pthread_mutex_unlock(&intersectionsMutexes[i]);
                //wait
                printf("Train %d passing intersection %d \n", trainId, i);
                usleep(500000);
                //lock the mutex
                pthread_mutex_lock(&intersectionsMutexes[i]);
                //decrement intersection counter
                intersections[i]--;
            pthread_mutex_unlock(&intersectionsMutexes[i]);
            pthread_cond_broadcast(&condVariables[i]);
        }
    }

}

int main(void){

    //initializes mutexes for each intersection
    for(int i = 0; i < NUMINTERSECTIONS; i++)
        pthread_mutex_init(&intersectionsMutexes[i], NULL);
    

    //initializes condition variables
    for(int i = 0; i < NUMINTERSECTIONS; i++)
        pthread_cond_init(&condVariables[i], NULL);
    

    //creates one thread for each train
    for(int i = 0; i < NUMTRAINS; i++){
        taskids[i] = (int *) malloc(sizeof(int));
        *taskids[i] = i;
        pthread_create(&threads[i], NULL, train, (void *)taskids[i]);
    }
    
    

    //joins every train thread
    for(int i = 0; i < NUMTRAINS; i++)
        pthread_join(threads[i], NULL);
    

    
    //destroys intersections mutexes
    for(int i = 0; i < NUMINTERSECTIONS; i++)
        pthread_mutex_destroy(&intersectionsMutexes[i]);
    

    //destroys condition variables
    for(int i = 0; i < NUMINTERSECTIONS; i++)
        pthread_cond_destroy(&condVariables[i]);

    return 0;
}