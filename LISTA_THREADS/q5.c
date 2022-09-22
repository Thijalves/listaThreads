#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include <unistd.h>

#define N 5

#define TAKEN 'T'
#define AVAILABLE 'A'

int left(int id){
    int left;
    left = (id+N-1)%N;
    return left;
}

int right(int id){
    int right;
    right = (id+1)%N;
    return right;
}

void think(int id);    
void get_forks(int id);
void put_forks(int id);
void* filosofo(void* args);

//mutex para o array todo
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//variaveis de condicao
pthread_cond_t cond[N] = {PTHREAD_COND_INITIALIZER};

//array com o estado de cada garfo
char estadoGarfos[N];
//array com o estado de cada filosofos
char estadoFilos[N];

//funcao para representar a mesa
void draw(){
    printf("---------\n");
    printf("  %c   %c  \n", estadoFilos[0], estadoFilos[1]);
    printf(" %c     %c  \n", estadoFilos[4], estadoFilos[2]);
    printf("    %c  \n", estadoFilos[3]);
    printf("---------\n");
}

int main (){

    //ilustra a mesa dos filosofos onde H eh hungry, E eh eating e T significa thinking 
    for(int i = 0; i < 5; i++)
        estadoFilos[i] = 'T';
    draw();

    //cria as threads
    pthread_t threads[5];
    int* philosoherID[5];
    for(int i = 0; i < 5; i++){
        philosoherID[i] = (int *) malloc(sizeof(int)); 
        *philosoherID[i] = i;
        pthread_create(&threads[i], NULL, filosofo, (void*) philosoherID[i]);
    }

    for(int i = 0; i < 5; i++)
        pthread_join(threads[i], NULL);

    return 0;
}

//funcoes chamadas pelo filosofo
void* filosofo(void* args){
    int id = *((int*) args);
    while (1){
        sleep(1); //think
        get_forks(id);
        sleep(1); //eat
        put_forks(id);
    }
}

void get_forks(int id){

    //trava o mutex
    pthread_mutex_lock(&mutex);

    //enquanto nao puder pegar os garfos fica dormindo
    while (estadoGarfos[id] == TAKEN || estadoGarfos[right(id)] == TAKEN) {
        estadoFilos[id] = 'H';
        pthread_cond_wait(&cond[id], &mutex);
    }

    //aqui ele pode comer
    estadoGarfos[id] = TAKEN;
    estadoGarfos[right(id)] = TAKEN;
    printf("%d pegou os grafos %d e %d\n", id, id, right(id));
    estadoFilos[id] = 'E';
    //ilustra a mesa dos filosofos onde H eh hungry e E eh eating
    draw();

    //destrava o mutex
    pthread_mutex_unlock(&mutex);
    
}

void put_forks(int id){

    //trava o mutex
    pthread_mutex_lock(&mutex);

    //Solta os grafos
    estadoGarfos[id] = AVAILABLE;
    estadoGarfos[right(id)] = AVAILABLE;
    printf("%d soltou os garfos %d e %d\n", id, id, right(id));
    estadoFilos[id] = 'T';
    //ilustra a mesa dos filosofos onde H eh hungry e E eh eating
    draw();

    //avisa aos vizinhos que soltou
    pthread_cond_signal(&cond[left(id)]);
    pthread_cond_signal(&cond[right(id)]);
   
    //destrava o mutex
    pthread_mutex_unlock(&mutex);


}