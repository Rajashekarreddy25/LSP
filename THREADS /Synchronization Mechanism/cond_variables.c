#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

int data = 0;

pthread_mutex_t mutex;
pthread_cond_t cond;

void *consumer(void *arg){
	pthread_mutex_lock(&mutex);

	while(data == 0){
		printf("consumer waiting ..\n");

		pthread_cond_wait(&cond,&mutex);

	}

	printf("consumer received data=%d\n",data);

	pthread_mutex_unlock(&mutex);

	return NULL;
}

void *producer(void *arg){

	sleep(2);

	pthread_mutex_lock(&mutex);
	
	data = 100;

	printf("Producer Produced\n");
	
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);

	return NULL;
}

int main(){

	pthread_t t1,t2;

	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond,NULL);

	pthread_create(&t1,NULL,consumer,NULL);
	pthread_create(&t2,NULL,producer,NULL);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

}


