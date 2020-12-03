

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>


void creatPhilosophers(int nthreads);
void *philospherThread(void *arg);
void thinking(int threadIndex);
void pickUpChopsticks(int threadIndex);
void eating(int threadIndex);
void putDownChopsticks(int threadIndex);
int getRandomNumber(int range);
int nthreads;
int nextIndex;
pthread_mutex_t lock; 
pthread_cond_t cond;

// main entry point into program
int main(int argc, char *argv[]) {
	// check for bad arguments
	if (argc != 2) {
		printf("ERROR: Invalid number of arguments. Received %d, expected 1", argc -1);
		return -1;
	}
	
	int i;
	// determine number of threads
	nthreads = atoi(argv[1]);
	nextIndex = 0;
	printf("Assignment 4: # of threads = %d\n", nthreads);
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&cond, NULL);
	creatPhilosophers(nthreads);
	
	return 0;
}

// create all of the threads and join them
void creatPhilosophers(int nthreads) {
	int i;
	int error;
	pthread_t *tids;
	int threadIndex[nthreads];
	
	tids = (pthread_t *)calloc(nthreads, sizeof(pthread_t));
	if (tids == NULL) {
		perror("Failed to initialize semaphore");
		return;
	}
	
	for (i = 0; i < nthreads; i++) {
		threadIndex[i] = i+1;
		if (error = pthread_create(tids + i, NULL, philospherThread, threadIndex + i)) {
			printf("failed to create thread %d\n", i);
			return;
		}
	}
	
	for (i = 0; i < nthreads; i++) {
		if (error = pthread_join(tids[i], NULL)) {
			printf("failed to join thread %d\n", i);
		}
	}
	
	printf("%d threads have been completed/joined successfully!\n", nthreads);
}

// thread task, runs through various functions ordering the threads completion of them
void *philospherThread(void *arg) {
	int numThread = *((int *) arg);
	int error;
	
	thinking(numThread);
	
	// lock resource
	error = pthread_mutex_lock(&lock);
	if (error) {
		printf("error locking left chopstick for philosopher %d: %d\n", numThread, error);
	}
	
	// if in violation of thread ordering, wait on conditional variable
	while (nextIndex != (numThread - 1)) {
		error = pthread_cond_wait(&cond, &lock);
		if (error) {
			printf("failed to wait on condition: %d\n", error);
		}
	} 
	
	pickUpChopsticks(numThread);
	eating(numThread);
	putDownChopsticks(numThread);
	nextIndex = nextIndex + 1;
	
	// wake up all threads in queue
	error = pthread_cond_broadcast(&cond);
	if (error) {
		printf("Failed to signal: %d\n", error);
	}
	
	error = pthread_mutex_unlock(&lock);
	if (error) {
		printf("error unlocking left chopstick for philosopher %d: %d\n", numThread, error);
	}
	
	pthread_exit(NULL);
}

// sleeping function
void thinking(int threadIndex) {
	printf("Philosopher #%d: start thinking\n", threadIndex);
	usleep(getRandomNumber(500));
	printf("Philosopher #%d: end thinking\n", threadIndex);
}

// function placeholder
void pickUpChopsticks(int threadIndex) {
	return;
}

// sleeping function
void eating(int threadIndex) {
	printf("Philosopher #%d: start eating\n", threadIndex);
	usleep(getRandomNumber(500));
	printf("Philosopher #%d: end eating\n", threadIndex);
}

// function placeholder
void putDownChopsticks(int threadIndex) {
	return;
}

// helper function
int getRandomNumber(int range) {
	srand(time(0));
	return (rand() % range) + 1;
}
