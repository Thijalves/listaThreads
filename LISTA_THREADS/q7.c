#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

//flag que indica se eh latina
char flag = 0;

//cria o mutex estatico
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//struct para argumentos das threads
typedef struct Trio{
    char **matriz;
    int index, size;
} Trio;

//Funcao para algumas threads
void *checkLine(void *args){
    // casting da struct
    int size = ((Trio*)args)->size;
    int index = ((Trio*)args)->index;
    char **matriz =  ((Trio*)args)->matriz;

    //procura por elementos repetidos
    for(int j = 0; j < size-1; j++)          
        for(int k = j + 1; k < size; k++)   
            if(matriz[index][j] == matriz[index][k]){
                //trava o mutex para impedir que varias threads acessem o recurso
                pthread_mutex_lock(&mutex);
                flag = 1;
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
            }
    pthread_exit(NULL);
}

//Funcao para algumas threads
void *checkColumn(void *args){
    // casting da struct
    int size = ((Trio*)args)->size;
    int index = ((Trio*)args)->index;
    char **matriz =  ((Trio*)args)->matriz;

    //procura por elementos repetidos
    for(int j = 0; j < size-1; j++)          
        for(int k = j + 1; k < size; k++)   
            if((matriz+j)[0][index] == (matriz+k)[0][index]){
                //trava o mutex para impedir que varias threads acessem o recurso
                pthread_mutex_lock(&mutex);
                flag = 1;
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
            }
    pthread_exit(NULL);
}

int main (){

    //define o tamanho da matriz
    int size = 5;

    //aloca a matriz do tamanho desejado
    char** matriz = (char**) malloc(size * sizeof(char*));
    if(matriz == NULL) exit(0);
    for (int i = 0; i < size; i++){
        matriz[i] = (char*) malloc(size * sizeof(char));
        if(matriz[i] == NULL) exit(0);
    }

    int val = 0; 

    //coloca os elementos na matriz
    for(int i = 0; i < size; i++)
        for(int j = 0; j < size; j++)
           matriz[i][j] = (val++)%5;
           
    //cria o vetor de threads
    pthread_t threads[2*size];
    Trio argumentos[size];

    //cria as threads de linhas
    for(int i = 0;i < size; i++){
        argumentos[i].index = i;
        argumentos[i].size = size;
        argumentos[i].matriz = matriz;
        if(pthread_create(&threads[i], NULL, checkLine, &argumentos[i]))
            printf("Erro na criacao de threads");
    }

    //cria as threads de colunas
    for(int i = 0; i < size; i++){
        argumentos[i].index = i;
        argumentos[i].size = size;
        argumentos[i].matriz = matriz;
        if(pthread_create(&threads[i+size], NULL, checkColumn, &argumentos[i]))
            printf("Erro na criacao de threads");
    }

    //espera todas as threads encerrarem
    for(int i = 0; i < 2*size; i++)
        if(pthread_join(threads[i], NULL))
            printf("Erro na juncao de threads");

    //printa a matriz
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
           printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
    
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