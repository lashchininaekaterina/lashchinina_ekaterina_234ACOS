#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define THREADS 4

long long sum = 0;
pthread_mutex_t lock;

typedef struct {
    int *arr;
    int start;
    int end;
} Data;

void *worker(void *arg) {
    Data *d = arg;
    long long local = 0;
    for (int i = d->start; i < d->end; i++)
        local += d->arr[i];

    pthread_mutex_lock(&lock);
    sum += local;
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(void) {
    int N;
    printf("Enter N (>= %d): ", THREADS * 4);
    if (scanf("%d", &N) != 1 || N < THREADS * 4) {
        fprintf(stderr, "Invalid input. N must be >= %d\n", THREADS * 4);
        return EXIT_FAILURE;
    }

    int *arr = malloc(N * sizeof *arr);
    if (!arr) perror("malloc"), exit(EXIT_FAILURE);

    srand((unsigned)time(NULL));
    for (int i = 0; i < N; i++)
        arr[i] = rand() % 100;

    pthread_mutex_init(&lock, NULL);
    pthread_t threads[THREADS];
    Data d[THREADS];

    int base = N / THREADS;
    int rem = N % THREADS;
    int idx = 0;

    for (int i = 0; i < THREADS; i++) {
        d[i] = (Data){ arr, idx, idx + base + (i < rem) };
        idx = d[i].end;
        pthread_create(&threads[i], NULL, worker, &d[i]);
    }

    printf("Array: ");
    for (int i = 0; i < N; i++)
        printf("%d ", arr[i]);
    printf("\n");

    for (int i = 0; i < THREADS; i++)
        pthread_join(threads[i], NULL);

    printf("Total sum = %lld\n", sum);

    pthread_mutex_destroy(&lock);
    free(arr);
    return 0;
}
