#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define N 8
#define MaxItems 8192
pthread_mutex_t mutex;
sem_t empty;
sem_t full;
int* buffer;
int ind=0;
int j=0;

void insert_item(int item, int index,int Pp){
    buffer[ind]=item;
    printf("Producer %d inserted item %d at %d\n", Pp, buffer[ind],ind);
}

void* producer(void* p){
    int P = atoi(p);
    int item;
    int i=0;
    while(i<MaxItems){
        item=rand();
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        insert_item(item,ind,P);
        ind=(ind+1)%N;
        for (int k=0; k<1000; k++){
            //printf("Producer [%d], treatement [%d]\n",P,k);
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        i++;
    }
}
 
void* consumer(void* p){
    int P = atoi(p);
    int i=0;
    while(i<MaxItems)
    {
        sem_wait(&full); // waiting for free place
        pthread_mutex_lock(&mutex);
        int item=buffer[j];
        printf("Consumer %d removed item %d at %d\n", P, item,j);
        j=(j+1)%N;
        for (int k=0; k<1000; k++){
            //printf("Consumer [%d], treatement [%d]\n",P,k);
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&empty); // one more place free
        i++;
    }
}

int main(int argc, char const *argv[]){

    buffer=malloc(N*sizeof(int));
    int cons = atoi(argv[1]);
    // printf("Cons = %d\n",cons);
    int prod = atoi(argv[2]);
    // printf("Prod = %d\n",prod);
    pthread_t pro[cons],con[prod];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty,0,N);
    sem_init(&full,0,0);
    int* a_prod = malloc(sizeof(int)*prod);
    for(int i=0; i<prod; i++){
        a_prod[i]=i;
    }
    int* a_cons = malloc(sizeof(int)*cons);
    for(int i=0; i<cons; i++){
        a_cons[i]=i;
    }
    for(int i = 0; i < prod; i++) {
        pthread_create(&pro[i], NULL, (void *)producer, (void*)&a_prod[i]);
    }

    for(int i = 0; i < cons; i++) {
         pthread_create(&con[i], NULL, (void *)consumer, (void*)&a_cons[i]);
     }

    for(int i = 0; i < prod; i++) {
        pthread_join(pro[i], NULL);
    }
    for(int i = 0; i < cons; i++) {
        pthread_join(con[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}
