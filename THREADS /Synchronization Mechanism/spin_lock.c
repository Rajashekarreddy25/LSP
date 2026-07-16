#include<stdio.h>
#include<pthread.h>

pthread_spinlock_t spin;

int cnt = 0;

void *worker1(void *arg){

	for(int i=0 ;i<5 ; i++){
		pthread_spin_lock(&spin);

		cnt++;

		printf("worker1's counter=%d\n",cnt);

		pthread_spin_unlock(&spin);
	}
	return NULL;
}

void *worker2(void *arg){

        for(int i=0 ;i<5 ; i++){
                pthread_spin_lock(&spin);

                cnt++;

                printf("worker2's counter=%d\n",cnt);

                pthread_spin_unlock(&spin);
        }
        return NULL;
}

int main(){

	pthread_t t1,t2;

	pthread_spin_init(&spin,PTHREAD_PROCESS_PRIVATE);

	pthread_create(&t1,NULL,worker1,NULL);
	pthread_create(&t2,NULL,worker2,NULL);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

	pthread_spin_destroy(&spin);
}
