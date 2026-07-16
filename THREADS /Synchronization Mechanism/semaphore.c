#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>


sem_t sem;

void *worker(void *arg){

	sem_wait(&sem);

	printf("Thread %ld entered\n", (long)arg);

	sleep(3);

	printf("Thread %ld leaving\n", (long)arg);

	sem_post(&sem);

	return NULL;
}

int main(){

	pthread_t t[4];

	sem_init(&sem,0,2);

	 for(int i=0; i<4 ;i++)
		 pthread_create(&t[i],NULL,worker,(void *)i);
			
	for(int i=0; i<4; i++)
		pthread_join(t[i],NULL);

	sem_destroy(&sem);

}
