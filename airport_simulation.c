#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int num_planes, num_terminals, planes_per_terminal, num_runways;
sem_t terminal_slots;
sem_t runway_slots;
pthread_mutex_t print_mutex;

void* plane_activity(void* plane_num) {
    int plane = *((int*) plane_num);
    
    // Arrival at the airport
    usleep(rand() % 500000); // Simulate random arrival time
    pthread_mutex_lock(&print_mutex);
    printf("Plane %d is arriving at the airport...\n", plane);
    pthread_mutex_unlock(&print_mutex);

    // Waiting for a terminal slot
    sem_wait(&terminal_slots);
    pthread_mutex_lock(&print_mutex);
    printf("Plane %d is waiting for a terminal slot...\n", plane);
    pthread_mutex_unlock(&print_mutex);
    
    // Park at the terminal
    usleep(rand() % 500000); // Simulate random parking time
    pthread_mutex_lock(&print_mutex);
    printf("Plane %d has parked at a terminal slot.\n", plane);
    pthread_mutex_unlock(&print_mutex);

    // Waiting for a runway
    sem_wait(&runway_slots);
    pthread_mutex_lock(&print_mutex);
    printf("Plane %d is waiting for a runway...\n", plane);
    pthread_mutex_unlock(&print_mutex);
    
    // Take off (without emojis or additional symbols)
    usleep(rand() % 500000); // Simulate random takeoff time
    pthread_mutex_lock(&print_mutex);
    printf("Plane %d is taking off!\n", plane);
    pthread_mutex_unlock(&print_mutex);
    
    // Release terminal slot and runway
    sem_post(&terminal_slots);
    sem_post(&runway_slots);
    
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s N T P R\n", argv[0]);
        return 1;
    }

    // Read input values
    num_planes = atoi(argv[1]);
    num_terminals = atoi(argv[2]);
    planes_per_terminal = atoi(argv[3]);
    num_runways = atoi(argv[4]);

    // Initialize semaphores and mutex
    sem_init(&terminal_slots, 0, num_terminals * planes_per_terminal);
    sem_init(&runway_slots, 0, num_runways);
    pthread_mutex_init(&print_mutex, NULL);

    printf("Airport simulation with %d planes, %d terminals (%d planes per terminal), and %d runways started.\n",
           num_planes, num_terminals, planes_per_terminal, num_runways);

    pthread_t threads[num_planes];
    int plane_nums[num_planes];

    // Create threads for each plane
    for (int i = 0; i < num_planes; i++) {
        plane_nums[i] = i + 1;
        pthread_create(&threads[i], NULL, plane_activity, &plane_nums[i]);
    }

    // Wait for all planes to complete
    for (int i = 0; i < num_planes; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All planes have successfully taken off.\n");

    // Clean up
    sem_destroy(&terminal_slots);
    sem_destroy(&runway_slots);
    pthread_mutex_destroy(&print_mutex);

    return 0;
}
