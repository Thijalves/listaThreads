#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

//number of positions in the database array
#define DATABASESIZE 10

//database to be written/read
int database[DATABASESIZE];
pthread_mutex_t databaseMutex[DATABASESIZE];

//counter that tells how many threads are currently waiting to write on database
int waitingToWrite[DATABASESIZE] = {0};
pthread_mutex_t waitingToWriteMutex[DATABASESIZE];

int currReading[DATABASESIZE] = {0};

//condition variable to sinalize that reading threads can act now
pthread_cond_t finishWriting[DATABASESIZE];
pthread_cond_t finishReading[DATABASESIZE];

int getDatabaseIndex(){
    return rand()%DATABASESIZE;
}

void *writer(void *args){
    int threadId = *(int *) args;
    printf("Writer thread No %d executing\n", *(int *)args);

    while(1){
        int databaseIndex = getDatabaseIndex();

        pthread_mutex_lock(&waitingToWriteMutex[databaseIndex]);
            //this avoids reading threads from getting ahead of this writer thread
            waitingToWrite[databaseIndex]++;
            while(currReading[databaseIndex] != 0){
                printf("[%d] Writer %d waiting for readers to finish...\n", databaseIndex, threadId);
                //waits for reading threads to finish
                pthread_cond_wait(&finishReading[databaseIndex], &waitingToWriteMutex[databaseIndex]);
            }
                //if this writer thread is allowed to proceed
                printf("[%d] Writer %d allowed to proceed | currReading = %d\n", databaseIndex, threadId, currReading[databaseIndex]);
        pthread_mutex_unlock(&waitingToWriteMutex[databaseIndex]);

        //tries to lock the mutex, if it cannot, waits on the condition variable
        while(pthread_mutex_trylock(&databaseMutex[databaseIndex])){
            pthread_mutex_lock(&waitingToWriteMutex[databaseIndex]);
                pthread_cond_wait(&finishWriting[databaseIndex], &waitingToWriteMutex[databaseIndex]);
                printf("[%d] Writer %d tried to access database, but blocked\n", databaseIndex, threadId);
            pthread_mutex_unlock(&waitingToWriteMutex[databaseIndex]);
        }
        // pthread_mutex_lock(&databaseMutex[databaseIndex]);
            //writes a random number on database
            database[databaseIndex] = rand();
            printf("[%d] Writer %d writing\n", databaseIndex, threadId);
            //sleeps for a random time from 0 to 1,5 seconds
            usleep(rand()%1500000);
        pthread_mutex_unlock(&databaseMutex[databaseIndex]);

        //after finishing writing, decrements waiting to write counter
        pthread_mutex_lock(&waitingToWriteMutex[databaseIndex]);
            waitingToWrite[databaseIndex]--;
        pthread_mutex_unlock(&waitingToWriteMutex[databaseIndex]);
        printf("[%d] Writer %d finished writing\n", databaseIndex, threadId);
        pthread_cond_broadcast(&finishWriting[databaseIndex]);

    }

    pthread_exit(NULL);
}

void *reader(void *args){
    int threadId = *(int *) args;
    printf("Reader thread No %d executing\n", threadId);

    while(1){
        int databaseIndex = getDatabaseIndex();

        pthread_mutex_lock(&waitingToWriteMutex[databaseIndex]);
            while(waitingToWrite[databaseIndex] != 0){
                printf("[%d] Reader %d waiting for writers to finish...\n", databaseIndex, threadId);
                //waits for writing threads to finish execution
                pthread_cond_wait(&finishWriting[databaseIndex], &waitingToWriteMutex[databaseIndex]);
            }
            printf("[%d] Reader %d allowed to proceed...\n", databaseIndex, threadId);
            currReading[databaseIndex]++;
        pthread_mutex_unlock(&waitingToWriteMutex[databaseIndex]);

        //reads database
        printf("[%d] Reader %d reads from database\n", databaseIndex, threadId);
        //sleeps for a random time from 0 to 1,5 seconds
        usleep(rand()%1500000);

        //after it finishes reading, decrements current reading counter
        pthread_mutex_lock(&waitingToWriteMutex[databaseIndex]);
            currReading[databaseIndex]--;
        pthread_mutex_unlock(&waitingToWriteMutex[databaseIndex]);
        printf("[%d] Reader %d finished reading\n", databaseIndex, threadId);
        pthread_cond_broadcast(&finishReading[databaseIndex]);
    }
}

int main(void){
    int numReader, numWriter;

    //initializes random number generator
    srand(time(NULL));

    //initializes mutexes
    for(int i = 0; i < DATABASESIZE; i++)
        pthread_mutex_init(&databaseMutex[i], NULL);

    for(int i = 0; i < DATABASESIZE; i++)
        pthread_mutex_init(&waitingToWriteMutex[i], NULL);

    //initializes condition variables
    for(int i = 0; i < DATABASESIZE; i++)
        pthread_cond_init(&finishReading[i], NULL);

    for(int i = 0; i < DATABASESIZE; i++)
        pthread_cond_init(&finishWriting[i], NULL);


    printf("How many reader threads?\n");
    scanf("%d", &numReader);
    printf("How many writer threads?\n");
    scanf("%d", &numWriter);

    //threads vectors
    pthread_t readerThreads[numReader];
    pthread_t writerThreads[numWriter];

    //creates reader threads
    int *readerIds[numReader];
    for(int i = 0; i < numReader; i++){
        readerIds[i] = (int *) malloc(sizeof(int));
        *readerIds[i] = i;
        if(pthread_create(&readerThreads[i], NULL, reader, (void *)readerIds[i])){
            printf("Error while creating threads \n");
            exit(1);
        }
        
    }

    //creates writer threads
    int *writerIds[numWriter];
    for(int i = 0; i < numWriter; i++){
        writerIds[i] = (int *) malloc(sizeof(int));
        *writerIds[i] = i;
        if(pthread_create(&writerThreads[i], NULL, writer, (void *)writerIds[i])){
            printf("Error while creating threads \n");
            exit(1);
        }
    }

    //joins reader threads
    for(int i = 0; i < numReader; i++){
        if(pthread_join(readerThreads[i], NULL)){
            printf("Error while joining threads\n");
            exit(1);
        }
    } 

    //joins writer threads
    for(int i = 0; i < numWriter; i++){
        if(pthread_join(writerThreads[i], NULL)){
           printf("Error while joining threads\n");
            exit(1); 
        }
    }

    //initializes mutexes
    for(int i = 0; i < DATABASESIZE; i++)
        pthread_mutex_destroy(&databaseMutex[i]);

    for(int i = 0; i < DATABASESIZE; i++)
        pthread_mutex_destroy(&waitingToWriteMutex[i]);

    //initializes condition variables
    for(int i = 0; i < DATABASESIZE; i++)
        pthread_cond_destroy(&finishReading[i]);

    for(int i = 0; i < DATABASESIZE; i++)
        pthread_cond_destroy(&finishWriting[i]);

    return 0;
}