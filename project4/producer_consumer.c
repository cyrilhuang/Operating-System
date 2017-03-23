#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "buffer.h"

#define rand_divisor 10

buffer_item buffer[BUFFER_SIZE];
int count = 0;

pthread_mutex_t mutex;
sem_t full, empty;

pthread_t tid;
pthread_attr_t attr;

int insert_item(buffer_item item)
{
	if (count < BUFFER_SIZE){
		buffer[count++] = item;
		return 0;
	}
	else return -1;
}

int remove_item(buffer_item *item)
{
	if (count > 0){
		*item = buffer[--count];
		return 0;
	}
	else return -1;
}

void *producer(void *param)
{
	buffer_item randNum;
	
	while(1){
		int r = rand()%rand_divisor;
		sleep(r);
		
		randNum = rand();
		
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		
		printf("Producer produced %d\n", randNum);
		if (insert_item(randNum))
			fprintf(stderr, "report error condition\n");
		
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
	}
}

void *consumer(void *param)
{
	buffer_item randNum;
	
	while(1){
		int r = rand()%rand_divisor;
		sleep(r);
		
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
		
		if (remove_item(&randNum))
			fprintf(stderr, "report error condition\n");
		else
			printf("Consumer consumed %d\n", randNum);
		
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
	}
}

int main(int argc, char *argv[])
{
	if (argc < 4){
		printf("error input format\n");
	}
	
	int sleepTime = atoi(argv[1]);
	int numPro = atoi(argv[2]);
	int numCon = atoi(argv[3]);
	
	pthread_mutex_init(&mutex, NULL);
	
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	
	pthread_attr_init(&attr);
	
	printf("producer-consumer problem starts:\n");
	
	int i;
	for(i = 0; i < numPro; ++i){
		pthread_create(&tid, &attr, producer, NULL);
	}
	
	for(i = 0; i < numCon; ++i){
		pthread_create(&tid, &attr, consumer, NULL);
	}
	
	sleep(sleepTime);
	exit(0);
}