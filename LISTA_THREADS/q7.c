#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

//flag para sabe se eh latina
char flag = 0;

//cria o mutex estatico
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//struct para argumentos das threads
typedef struct Trio{
    char **matriz;
    int index, size;
} Trio;

void *checkLine(void *args){
    int size = ((Trio*)args)->size;
    int index = ((Trio*)args)->index;
    char **matriz =  ((Trio*)args)->matriz;

    for(int j = 0; j < size-1; j++)          
        for(int k = j + 1; k < size; k++)   
            if(matriz[index][j] == matriz[index][k]){
                pthread_mutex_lock(&mutex);
                flag = 1;
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
            }
    pthread_exit(NULL);
}

void *checkColumn(void *args){
    int size = ((Trio*)args)->size;
    int index = ((Trio*)args)->index;
    char **matriz =  ((Trio*)args)->matriz;
    for(int j = 0; j < size-1; j++)          
        for(int k = j + 1; k < size; k++)   
            if((matriz+j)[0][index] == (matriz+k)[0][index]){
                pthread_mutex_lock(&mutex);
                flag = 1;
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
            }
    pthread_exit(NULL);
}

int main (){

    int size;
    scanf("%d", &size);

    //aloca a matriz do tamanho desejado
    char** matriz = (char**) malloc(size * sizeof(char*));
    if(matriz == NULL) exit(0);
    for (int i = 0; i < size; i++){
        matriz[i] = (char*) malloc(size * sizeof(char));
        if(matriz[i] == NULL) exit(0);
    }

    //le os valores da matriz
    for(int i = 0; i < size; i++)
        for(int j = 0; j < size; j++)
            scanf(" %c", &matriz[i][j]);

    //cria o vetor de threads
    pthread_t threads[2*size];
    Trio argumentos[size];

    //cria as threads de linhas
    for(int i = 0;i < size; i++){
        argumentos[i].index = i;
        argumentos[i].size = size;
        argumentos[i].matriz = matriz;
        pthread_create(&threads[i], NULL, checkLine, &argumentos[i]);
    }

    //cria as threads de colunas
    for(int i = 0; i < size; i++){
        argumentos[i].index = i;
        argumentos[i].size = size;
        argumentos[i].matriz = matriz;
        pthread_create(&threads[i+size], NULL, checkColumn, &argumentos[i]);
    }

    //espera todas as threads encerrarem
    for(int i = 0; i < 2*size; i++)
        pthread_join(threads[i], NULL);
    
    //toma a decisao baseado na flag
    if(flag)
        printf("A matriz NAO eh um quadrado latino\n");
    else
        printf("A matriz eh um quadrado latino\n");
    
    //free na matriz
    for(int i = 0; i < size; i++)
        free(matriz[i]);
    free(matriz);

    return 0;
}