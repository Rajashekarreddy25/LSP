#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

int full = 0;
int buffer;

pthread_mutex_t mutex;
pthread_cond_t cond;

void *consumer(void *arg){

	for(int i=0;i<10;i++){
		pthread_mutex_lock(&mutex);

		while(full == 0){
			pthread_cond_wait(&cond,&mutex);
		}

		printf("The buffer is consumed:%d\n",buffer);
		full = 0;
		
		pthread_cond_signal(&cond);

		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void *producer(void *arg){

	for (int i=0;i<10;i++){
		pthread_mutex_lock(&mutex);

		while(full){
			pthread_cond_wait(&cond,&mutex);
		}

		buffer = i;
		full = 1;
		printf("The buffer is Produced :%d\n",buffer);

		pthread_cond_signal(&cond);

		pthread_mutex_unlock(&mutex);

		sleep(1);
	}

	return NULL;
}

int main(){

	pthread_t c,p;

	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond,NULL);

	pthread_create(&p,NULL,producer,NULL);
	pthread_create(&c,NULL,consumer,NULL);

	pthread_join(p,NULL);
	pthread_join(c,NULL);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

}


