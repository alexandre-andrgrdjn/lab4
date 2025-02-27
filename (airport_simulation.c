#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Global semaphore and mutex for synchronization
sem_t terminal_semaphore;
pthread_mutex_t runway_mutex;

// Structure to store data for each plane
typedef struct {
    int id;  // Plane ID
    sem_t* terminal_semaphore;  // Semaphore for terminal slot management
    pthread_mutex_t* runway_mutex;  // Mutex for runway management
} plane_t;

// Function to initialize semaphores and mutexes
void initialize_resources(int total_terminals, int planes_per_terminal, int runways) {
    sem_init(&terminal_semaphore, 0, total_terminals * planes_per_terminal);  // Semaphore for terminal slots
    pthread_mutex_init(&runway_mutex, NULL);  // Mutex for runway
}

// Function to clean up semaphores and mutexes after use
void cleanup_resources() {
    sem_destroy(&terminal_semaphore);
    pthread_mutex_destroy(&runway_mutex);
}

// Thread function for each plane's routine
void* plane_routine(void* arg) {
    plane_t* plane = (plane_t*) arg;

    // Step 1: Plane arrives at the airport
    printf("Plane %d arrives at the airport.\n", plane->id);
    usleep(rand() % 500000);  // Simulate random arrival time

    // Step 2: Plane waits for an available terminal slot
    printf("Plane %d is waiting for a terminal slot.\n", plane->id);
    sem_wait(plane->terminal_semaphore);  // Wait for terminal space
    printf("Plane %d parks at a terminal.\n", plane->id);

    // Step 3: Plane stays at the terminal for a random duration
    usleep(rand() % 500000);  // Simulate random parking time

    // Step 4: Plane waits for an available runway
    printf("Plane %d is waiting for a runway.\n", plane->id);
    pthread_mutex_lock(plane->runway_mutex);  // Wait for exclusive access to the runway
    printf("Plane %d takes off.\n", plane->id);

    // Step 5: Plane takes off and releases the runway
    usleep(rand() % 500000);  // Simulate random takeoff time
    pthread_mutex_unlock(plane->runway_mutex);  // Release the runway

    // Step 6: Plane leaves the terminal, freeing the space
    sem_post(plane->terminal_semaphore);  // Release the terminal space

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <N> <T> <P> <R>\n", argv[0]);
        return 1;
    }

    // Initialize resources and arguments
    int total_planes = atoi(argv[1]);
    int total_terminals = atoi(argv[2]);
    int planes_per_terminal = atoi(argv[3]);
    int runways = atoi(argv[4]);

    // Initialize semaphores and mutexes
    initialize_resources(total_terminals, planes_per_terminal, runways);

    // Create and run threads for each plane
    pthread_t planes[total_planes];
    plane_t plane_data[total_planes];

    for (int i = 0; i < total_planes; i++) {
        plane_data[i].id = i + 1;
        plane_data[i].terminal_semaphore = &terminal_semaphore;
        plane_data[i].runway_mutex = &runway_mutex;
        pthread_create(&planes[i], NULL, plane_routine, (void*)&plane_data[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < total_planes; i++) {
        pthread_join(planes[i], NULL);
    }

    // Clean up resources
    cleanup_resources();

    printf("All planes have taken off. Airport operations complete.\n");

    return 0;
}
