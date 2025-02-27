#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

sem_t terminal_sem;  // Semaphore pour les places de terminal
pthread_mutex_t runway_mutex;  // Mutex pour les pistes

int num_planes, num_terminals, planes_per_terminal, num_runways;

// Fonction pour simuler un avion (thread)
void* plane_operations(void* plane_id) {
    int id = *(int*)plane_id;
    printf("Plane %d is arriving at the airport...\n", id);
    usleep(rand() % 500000);  // Simuler un délai d'arrivée

    // Attente d'une place dans un terminal
    printf("Plane %d is waiting for a terminal slot...\n", id);
    sem_wait(&terminal_sem);  // Attente d'une place de terminal
    printf("Plane %d has parked at a terminal slot.\n", id);

    usleep(rand() % 500000);  // Simuler un délai de stationnement

    // Attente d'une piste pour décoller
    printf("Plane %d is waiting for a runway...\n", id);
    pthread_mutex_lock(&runway_mutex);  // Attente d'une piste
    printf("Plane %d is taking off!\n", id);
    usleep(rand() % 500000);  // Simuler le décollage

    // Libération de la place de terminal et de la piste
    sem_post(&terminal_sem);
    pthread_mutex_unlock(&runway_mutex);
    
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage: ./airport_simulation <N> <T> <P> <R>\n");
        return -1;
    }

    num_planes = atoi(argv[1]);
    num_terminals = atoi(argv[2]);
    planes_per_terminal = atoi(argv[3]);
    num_runways = atoi(argv[4]);

    // Initialisation du sémaphore et du mutex
    sem_init(&terminal_sem, 0, num_terminals * planes_per_terminal);
    pthread_mutex_init(&runway_mutex, NULL);

    printf("Airport simulation with %d planes, %d terminals (%d planes per terminal), and %d runways started.\n", 
           num_planes, num_terminals, planes_per_terminal, num_runways);

    pthread_t planes[num_planes];
    int plane_ids[num_planes];

    // Création des threads pour les avions
    for (int i = 0; i < num_planes; i++) {
        plane_ids[i] = i + 1;  // ID de l'avion (1 à N)
        pthread_create(&planes[i], NULL, plane_operations, &plane_ids[i]);
    }

    // Attente de la fin de tous les threads
    for (int i = 0; i < num_planes; i++) {
        pthread_join(planes[i], NULL);
    }

    // Nettoyage des ressources
    sem_destroy(&terminal_sem);
    pthread_mutex_destroy(&runway_mutex);

    printf("All planes have successfully taken off.\n");
    return 0;
}
