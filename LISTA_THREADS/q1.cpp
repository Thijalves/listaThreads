#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int * votos;
int cont, numArq, numCdt;
pthread_mutex_t * mutexCdt = NULL; 
pthread_mutex_t fileFlag = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
    int winnerIdx;
    int winnerPoints;
} winner;

void * funcao(void * args){
  char voto[10];
  int n=0, i=0;

  FILE ** archive = (FILE**) args;
 while(cont < numArq){
    
    pthread_mutex_lock(&fileFlag);
    i=cont;
    cont++;
    pthread_mutex_unlock(&fileFlag);
    n=0;

  while (fscanf(archive[i], "%s", voto) != -1 && n < numCdt) {
    if(voto[0]!=48){
      pthread_mutex_lock(&mutexCdt[n]);
      votos[n]++;
      pthread_mutex_unlock(&mutexCdt[n]);
      }
      if(voto[0]==48){
        pthread_mutex_lock(&mutexCdt[numCdt]);
        votos[numCdt]++;
        pthread_mutex_unlock(&mutexCdt[numCdt]);
      }
      n++;
    }
  }
  pthread_exit(NULL);
  }

void changeName(char * fileName, int n){
  sprintf(fileName, "%d.in", n);
}

int main(){
  int arq, trd, cdt, n=1;
  winner winner;
  float porcentagem=0, totalVotos=0;
  winner.winnerPoints = 0;
  char file[100];
  scanf("%d %d %d", &arq, &trd, &cdt);
  votos = (int*)malloc(sizeof(int)*(cdt+1));
  mutexCdt= (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*(cdt+1));

  for(int i=0;i<=cdt;i++){
    mutexCdt[i] = PTHREAD_MUTEX_INITIALIZER;
  }
  cont = 0;numArq = arq;numCdt = cdt;

  FILE *arqvs[arq];
  for(int i=1, count=0;i<=arq;i++, count++){
    changeName(file, i);
    arqvs[count] = fopen(file, "r");
    
    if(arqvs[count]==NULL){
      printf("Erro ao abrir arquivo:(\n");
      exit(1);
    }
  }
  pthread_t thread[trd];
  for(int i=0;i<trd; i++){
    //printf("Thread:%d\n", i);
    pthread_create(&thread[i], NULL, funcao, &arqvs);
  }

  for(int i = 0; i < trd; i++){
        pthread_join(thread[i], NULL);
  }
  for(int i = 0; i < cdt; i++){totalVotos += votos[i];}
 for(int i=0;i<=cdt;i++){
    if(i!=cdt){
    porcentagem = (votos[i]/totalVotos)*100;
    printf("Candidato : %d | Votos : %.0f%c\n", i+1, porcentagem, 37);
    }
    else{
        printf("Brancos : %d\n", votos[numCdt]);
    }
    if(votos[i] > winner.winnerPoints && i!= numCdt){
        winner.winnerPoints = votos[i];
        winner.winnerIdx = i;
    }
  }
  printf("\n\nVencedor : %d\n\n", winner.winnerIdx+1);

  free(votos);
  free(mutexCdt);
  
  return 0;
}
  