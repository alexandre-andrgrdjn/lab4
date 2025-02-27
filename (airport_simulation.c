#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_PLANES 100

// Global variables
int N, T, P, R; // Number of planes, terminals, planes per terminal, and runways
sem_t terminal_slots; // Semaphore for available terminal slots
pthread_mutex_t runway_mutex; // Mutex for runway access
pthread_mutex_t print_mutex; // Mutex for synchronized printing

void* plane_routine(void* arg) {
    int id = *(int*)arg;
    usleep(rand() % 500000); // Simulate random arrival time

    // Arrival message
    pthread_mutex_lock(&print_mutex);
    printf("Plane %d is arriving at the airport...\n", id);
    pthread_mutex_unlock(&print_mutex);

    // Wait for a terminal slot
    pthread_mutex_lock(&print_mutex);
    printf("Plane %d is waiting for a terminal slot...\n", id);
    pthread_mutex_unlock(&print_mutex);

    sem_wait(&terminal_slots);

    // Park at terminal
    pthread_mutex_lock(&print_mutex);
    printf("Plane %d has parked at a terminal slot.\n", id);
    pthread_mutex_unlock(&print_mutex);

    usleep(rand() % 500000); // Simulate time spent at terminal

    // Wait for runway access
    pthread_mutex_lock(&print_mutex);
    printf("Plane %d is waiting for a runway...\n", id);
    pthread_mutex_unlock(&print_mutex);

    pthread_mutex_lock(&runway_mutex);

    // Take off
    pthread_mutex_lock(&print_mutex);
    printf("Plane %d is taking off!\n", id);
    pthread_mutex_unlock(&print_mutex);

    usleep(rand() % 500000); // Simulate takeoff time

    pthread_mutex_lock(&print_mutex);
    printf("Plane %d has taken off!\n", id);
    pthread_mutex_unlock(&print_mutex);

    pthread_mutex_unlock(&runway_mutex);
    sem_post(&terminal_slots); // Free terminal slot

    free(arg);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage: %s <N> <T> <P> <R>\n", argv[0]);
        return 1;
    }

    // Parse arguments
    N = atoi(argv[1]);
    T = atoi(argv[2]);
    P = atoi(argv[3]);
    R = atoi(argv[4]);

    if (N <= 0 || T <= 0 || P <= 0 || R <= 0) {
        printf("All values must be positive integers.\n");
        return 1;
    }

    // Initialize synchronization mechanisms
    sem_init(&terminal_slots, 0, T * P);
    pthread_mutex_init(&runway_mutex, NULL);
    pthread_mutex_init(&print_mutex, NULL);

    // Print initial configuration
    pthread_mutex_lock(&print_mutex);
    printf("Airport simulation with %d planes, %d terminals (%d planes per terminal), and %d runways started.\n", N, T, P, R);
    pthread_mutex_unlock(&print_mutex);

    pthread_t planes[MAX_PLANES];

    // Create plane threads
    for (int i = 0; i < N; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&planes[i], NULL, plane_routine, id);
    }

    // Wait for all planes to take off
    for (int i = 0; i < N; i++) {
        pthread_join(planes[i], NULL);
    }

    // Print final message
    pthread_mutex_lock(&print_mutex);
    printf("All planes have successfully taken off.\n");
    pthread_mutex_unlock(&print_mutex);

    // Cleanup
    sem_destroy(&terminal_slots);
    pthread_mutex_destroy(&runway_mutex);
    pthread_mutex_destroy(&print_mutex);

    return 0;
}
