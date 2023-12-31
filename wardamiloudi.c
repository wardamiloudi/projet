#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_SIZE 100

void printMatrix(int matrix[MAX_SIZE][MAX_SIZE], int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// Matrices B, C, A
int B[MAX_SIZE][MAX_SIZE];
int C[MAX_SIZE][MAX_SIZE];
int A[MAX_SIZE][MAX_SIZE];

// Buffer pour les résultats intermédiaires
int T[MAX_SIZE][MAX_SIZE];

// Tailles des matrices
int n1, m1, n2, m2;

// Variables de synchronisation
pthread_mutex_t mutex;
sem_t empty, full;

// Fonction pour produire des résultats intermédiaires
void produce(int row) {
    int i, j;
    for (i = 0; i < m2; ++i) {
        T[row][i] = 0;
        for (j = 0; j < n2; ++j) {
            T[row][i] += B[row][j] * C[j][i];
        }
    }
}

// Fonction pour insérer un résultat intermédiaire dans la matrice A
void insert_item(int row) {
    int i;
    for (i = 0; i < m2; ++i) {
        A[row][i] = T[row][i];
    }
}

// Fonction pour le producteur
void *producer(void *arg) {
    int row = *(int *)arg;
    produce(row);
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    insert_item(row);
     // Affichage de la matrice T après avoir produit les résultats intermédiaires
    printf("Matrix T (Produced by Thread %d):\n", row);
    for (int i = 0; i < m2; ++i) {
        printf("%d ", T[row][i]);
    }
    printf("\n");
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    pthread_exit(NULL);
}

// Fonction pour le consommateur
void *consumer(void *arg) {
    int row = *(int *)arg;
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    pthread_exit(NULL);
}

int main() {
    // Initialisation des dimensions des matrices
    n1 = 3;
    m1 = 2;
    n2 = 2;
    m2 = 4;

    // Initialisation des matrices B et C avec des valeurs aléatoires
    srand(time(NULL));
    printf("Matrix B:\n");
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < m1; ++j) {
            B[i][j] = rand() % 10; // Valeurs aléatoires entre 0 et 9
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("\n");
    
    printf("Matrix C:\n");
    for (int i = 0; i < n2; ++i) {
        for (int j = 0; j < m2; ++j) {
            C[i][j] = rand() % 10;// Valeurs aléatoires entre 0 et 9
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("\n");
    // Initialisation des matrices B et C avec des valeurs aléatoires
    // (Assurez-vous d'initialiser les dimensions n1, m1, n2, m2)
    // ...

    // Initialisation des variables de synchronisation
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, n1);  // Buffer vide au départ
    sem_init(&full, 0, 0);    // Buffer plein au départ

    // Création des threads producteurs
    pthread_t producers[n1];
    int producer_args[n1];
    for (int i = 0; i < n1; ++i) {
        producer_args[i] = i;
        pthread_create(&producers[i], NULL, producer, &producer_args[i]);
    }

    // Attente des threads producteurs
    for (int i = 0; i < n1; ++i) {
        pthread_join(producers[i], NULL);
    }

    // Création des threads consommateurs
    pthread_t consumers[n1];
    int consumer_args[n1];
    for (int i = 0; i < n1; ++i) {
        consumer_args[i] = i;
        pthread_create(&consumers[i], NULL, consumer, &consumer_args[i]);
    }

    // Attente des threads consommateurs
    for (int i = 0; i < n1; ++i) {
        pthread_join(consumers[i], NULL);
    }

    // Destruction des variables de synchronisation
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    // Affichage de la matrice résultante A
    printf("\n");
    printf("\n");
    printf("Matrix A:\n");
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < m2; ++j) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }

    return 0;
}
