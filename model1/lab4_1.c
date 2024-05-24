#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

pthread_t Pthread1;
pthread_t Pthread2;
pthread_t Pthread3;
pthread_t Pthread4;
pthread_t Pthread5;
pthread_t Pthread6;

pthread_mutex_t mcr1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mcr_sig21 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mcr_sig22 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t sig21 = PTHREAD_COND_INITIALIZER;
pthread_cond_t sig22 = PTHREAD_COND_INITIALIZER;
pthread_cond_t sig1_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t sig2_not_full = PTHREAD_COND_INITIALIZER;

int flag21_P2 = 0;
int flag21_P5 = 0;
int flag21_P6 = 0;
int flag22 = 0;

sem_t scr21;

int max_length = 40;
int curr_length = 0;

int int1 = 0;
int int2 = 0;
unsigned uns1 = 0;
unsigned uns2 = 0;
long long1 = 0;
long long2 = 0;
long unsigned uns_long1 = 0;
long unsigned uns_long2 = 0;

FILE* f;

struct elem
{
    struct elem* next;
    int number;
};

struct elem* beg_q = NULL;
struct elem* end_q = NULL;
int queue_full = 0;
int queue_empty = 0;

int full();
int empty();
void add_elem();
struct elem* get_elem();

void atomic_modification();
void atomic_using();

void* P1(void* arg);
void* P2(void* arg);
void* P3(void* arg);
void* P4(void* arg);
void* P5(void* arg);
void* P6(void* arg);

int main()
{
    srand(time(NULL));
    f = fopen("file.log", "w");
    if (f == NULL)
    {
        printf("\nerror file\n");
        exit(1);
    }
    for(int i = 0; i < max_length / 2; i++)
    {
        add_elem();
        curr_length++;
    }
    fprintf("Queue with elements from 0-th to %d-th has been created !!!\n", max_length / 2);
    printf("Queue with elements from 0-th to %d-th has been created !!!\n", max_length / 2);

    sem_init(&scr21, 0, 0);

    int1 = (rand() % 10) - 5;
    int2 = (rand() % 10) - 5;
    uns1 = (rand() % 10) - 5;
    uns2 = (rand() % 10) - 5;
    long1 = (rand() % 10) - 5;
    long2 = (rand() % 10) - 5;
    uns_long1 = (rand() % 10) - 5;
    uns_long2 = (rand() % 10) - 5;

    int thread1 = 1;
    int thread2 = 2;
    int thread3 = 3;
    int thread4 = 4;
    int thread5 = 5;
    int thread6 = 6;

    pthread_create(&Pthread1, NULL, &P1, (void*)&thread1);
    pthread_create(&Pthread2, NULL, &P2, (void*)&thread2);
    pthread_create(&Pthread3, NULL, &P3, (void*)&thread3);
    pthread_create(&Pthread4, NULL, &P4, (void*)&thread4);
    pthread_create(&Pthread5, NULL, &P5, (void*)&thread5);
    pthread_create(&Pthread6, NULL, &P6, (void*)&thread6);

    pthread_join(Pthread1, NULL);
    pthread_join(Pthread2, NULL);
    pthread_join(Pthread3, NULL);
    pthread_join(Pthread4, NULL);
    pthread_join(Pthread5, NULL);
    pthread_join(Pthread6, NULL);

    fprintf(f, "\nAll information was written to the log file\n");
    printf("\nAll information was written to the log file\n");
    fclose(f);
    return 0;
}

int full()
{
    return curr_length >= max_length;
}

int empty()
{
    return curr_length <= 0;
}

void add_elem()
{
    struct elem* p = (struct elem*)malloc(sizeof(struct elem));
    p->next = NULL;
    if (beg_q == NULL)
    {
        p->number = 0;
        beg_q = p;
    }
    else
    {
        p->number = end_q->number + 1;
        end_q->next = p;
    }
    end_q = p;
}

struct elem* get_elem()
{
    struct elem* p = NULL;
    p = beg_q;
    beg_q = beg_q->next;
    return p;
}

void atomic_modification()
{
    __sync_fetch_and_add(&int1, (rand() % 6) - 3);
    __sync_add_and_fetch(&int2, (rand() % 6) - 3);
    __sync_fetch_and_or(&uns1, (rand() % 6) - 3);
    __sync_fetch_and_and(&uns2, (rand() % 6) - 3);
    __sync_xor_and_fetch(&long1, (rand() % 6) - 3);
    __sync_nand_and_fetch(&long2, (rand() % 6) - 3);
    __sync_bool_compare_and_swap(&uns_long1, (rand() % 6) - 3, (rand() % 6) - 3);
    __sync_val_compare_and_swap(&uns_long2, (rand() % 6) - 3, (rand() % 6) - 3);
}

void atomic_using()
{
    __sync_fetch_and_add(&int1, 0);
    __sync_add_and_fetch(&int2, 0);
    __sync_fetch_and_or(&uns1, 0);
    __sync_fetch_and_and(&uns2, uns2);
    __sync_xor_and_fetch(&long1, 0);
    __sync_nand_and_fetch(&long2, long2);
    __sync_nand_and_fetch(&long2, long2);
    __sync_bool_compare_and_swap(&uns_long1, 0, 0);
    __sync_val_compare_and_swap(&uns_long2, 0, 0);
}

void* P1(void* arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int num = *(int*)arg;
    struct elem* temp = NULL;
    int old_thread_state = 0;
    fprintf(f, "thread p[%d]: is working\n", num);
    printf("thread p[%d]: is working\n", num);
    while (1)
    {
        temp = NULL;
        if (curr_length == max_length)
        {
            fprintf(f, "thread p[%d]: queue is full\n", num);
            printf("thread p[%d]: queue is full\n", num);
            queue_full++;
            if (queue_full == 2)
                break;
        }
        pthread_mutex_lock(&mcr1);
        while (empty())
        {
            fprintf(f, "thread p[%d]: is waiting for sig1_not_empty\n", num);
            printf("thread p[%d]: is waiting for sig1_not_empty\n", num);
            pthread_cond_wait(&sig1_not_empty, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_thread_state);
        temp = get_elem();
        curr_length--;
        pthread_setcancelstate(old_thread_state, NULL);
        fprintf(f, "\nthread p[%d]: element %d TAKEN; current queue length = %d;\n", num, temp->number, curr_length);
        printf("\nthread p[%d]: element %d TAKEN; current queue length = %d;\n", num, temp->number, curr_length);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_broadcast(&sig2_not_full);
        fprintf(f, "thread p[%d]: sends sig2_not_full\n", num);
        printf("thread p[%d]: sends sig2_not_full\n", num);
        free(temp);
        fprintf(f, "thread p[%d]: opens semaphore scr21 for P4\n", num);
        printf("thread p[%d]: opens semaphore scr21 for P4\n", num);
        sem_post(&scr21);
        //usleep(100);
    }
    pthread_cancel(Pthread2);
    pthread_cancel(Pthread3);
    pthread_cancel(Pthread4);
    pthread_cancel(Pthread5);
    pthread_cancel(Pthread6);
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", num);
    return NULL;
}

void* P2(void* arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int num = *(int*)arg;
    int old_thread_state = 0;
    fprintf(f, "thread p[%d]: is working\n", num);
    printf("thread p[%d]: is working\n", num);
    while (1)
    {
        pthread_mutex_lock(&mcr_sig21);
        while(flag21_P2 == 0)
        {
            fprintf(f, "thread p[%d]: is waiting for sig21\n", num);
            printf("thread p[%d]: is waiting for sig21\n", num);
            pthread_cond_wait(&sig21, &mcr_sig21);
        }
        pthread_mutex_unlock(&mcr_sig21);
        flag21_P2 = 0;
        fprintf(f, "thread p[%d]: received sig21\n", num);
        printf("thread p[%d]: received sig21\n", num);
        atomic_using();
        fprintf(f, "thread p[%d]: uses atomic values\n", num);
        printf("thread p[%d]: uses atomic values\n", num);
        atomic_modification();
        fprintf(f, "thread p[%d]: modificates atomic values\n", num);
        printf("thread p[%d]: modificates atomic values\n", num);
        if (curr_length == 0)
        {
            fprintf(f, "thread p[%d]: queue is empty\n", num);
            printf("thread p[%d]: queue is empty\n", num);
            queue_empty++;
            if (queue_empty == 2)
                break;
        }
        pthread_mutex_lock(&mcr1);
        while (full())
        {
            fprintf(f, "thread p[%d]: is waiting for sig2_not_full\n", num);
            printf("thread p[%d]: is waiting for sig2_not_full\n", num);
            pthread_cond_wait(&sig2_not_full, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_thread_state);
        add_elem();
        curr_length++;
        pthread_setcancelstate(old_thread_state, NULL);
        fprintf(f, "\nthread p[%d]: element %d CREATED; current queue length = %d;\n", num, end_q->number, curr_length);
        printf("\nthread p[%d]: element %d CREATED; current queue length = %d;\n", num, end_q->number, curr_length);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_broadcast(&sig1_not_empty);
        fprintf(f, "thread p[%d]: sends sig1_not_empty\n", num);
        printf("thread p[%d]: sends sig1_not_empty\n", num);
        pthread_mutex_lock(&mcr_sig22);
        pthread_cond_signal(&sig22);
        flag22 = 1;
        pthread_mutex_unlock(&mcr_sig22);
        fprintf(f, "thread p[%d]: sends sig22\n", num);
        printf("thread p[%d]: sends sig22\n", num);
        //usleep(100);
    }
    pthread_cancel(Pthread1);
    pthread_cancel(Pthread3);
    pthread_cancel(Pthread4);
    pthread_cancel(Pthread5);
    pthread_cancel(Pthread6);
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", num);
    return NULL;
}

void* P3(void* arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int num = *(int*)arg;
    fprintf(f, "thread p[%d]: is working\n", num);
    printf("thread p[%d]: is working\n", num);
    atomic_modification();
    fprintf(f, "thread p[%d]: modificates atomic values\n", num);
    printf("thread p[%d]: modificates atomic values\n", num);
    while (1)
    {
        pthread_mutex_lock(&mcr_sig21);
        pthread_cond_broadcast(&sig21);
        flag21_P2 = 1;
        flag21_P5 = 1;
        flag21_P6 = 1;
        fprintf(f, "thread p[%d]: sends sig21\n", num);
        printf("thread p[%d]: sends sig21\n", num);
        pthread_mutex_unlock(&mcr_sig21);
        atomic_modification();
        fprintf(f, "thread p[%d]: modificates atomic values\n", num);
        printf("thread p[%d]: modificates atomic values\n", num);
        usleep(50);
    }
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", num);
    return NULL;
}

void* P4(void* arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int num = *(int*)arg;
    int old_thread_state = 0;
    fprintf(f, "thread p[%d]: is working\n", num);
    printf("thread p[%d]: is working\n", num);
    while (1)
    {
        if (curr_length == 0)
        {
            fprintf(f, "thread p[%d]: queue is empty\n", num);
            printf("thread p[%d]: queue is empty\n", num);
            queue_empty++;
            if (queue_empty == 2)
                break;
        }
        pthread_mutex_lock(&mcr1);
        while (full())
        {
            fprintf(f, "thread p[%d]: is waiting for sig2_not_full\n", num);
            printf("thread p[%d]: is waiting for sig2_not_full\n", num);
            pthread_cond_wait(&sig2_not_full, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_thread_state);
        add_elem();
        curr_length++;
        pthread_setcancelstate(old_thread_state, NULL);
        fprintf(f, "\nthread p[%d]: element %d CREATED; current queue length = %d;\n", num, end_q->number, curr_length);
        printf("\nthread p[%d]: element %d CREATED; current queue length = %d;\n", num, end_q->number, curr_length);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_broadcast(&sig1_not_empty);
        fprintf(f, "thread p[%d]: sends sig1_not_empty\n", num);
        printf("thread p[%d]: sends sig1_not_empty\n", num);
        fprintf(f, "thread p[%d]: waits for the scr21 from P1\n", num);
        printf("thread p[%d]: waits for the scr21 from P1\n", num);
        sem_wait(&scr21);
        //usleep(100);
    }
    pthread_cancel(Pthread1);
    pthread_cancel(Pthread2);
    pthread_cancel(Pthread3);
    pthread_cancel(Pthread5);
    pthread_cancel(Pthread6);
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", num);
    return NULL;
}

void* P5(void* arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int num = *(int*)arg;
    struct elem* temp = NULL;
    int old_thread_state = 0;
    fprintf(f, "thread p[%d]: is working\n", num);
    printf("thread p[%d]: is working\n", num);
    while (1)
    {
        temp = NULL;
        pthread_mutex_lock(&mcr_sig21);
        while (flag21_P5 == 0)
        {
            fprintf(f, "thread p[%d]: is waiting for sig21\n", num);
            printf("thread p[%d]: is waiting for sig21\n", num);
            pthread_cond_wait(&sig21, &mcr_sig21);
        }
        pthread_mutex_unlock(&mcr_sig21);
        flag21_P5 = 0;
        fprintf(f, "thread p[%d]: received sig21\n", num);
        printf("thread p[%d]: received sig21\n", num);
        atomic_using();
        fprintf(f, "thread p[%d]: uses atomic values\n", num);
        printf("thread p[%d]: uses atomic values\n", num);
        atomic_modification();
        fprintf(f, "thread p[%d]: modificates atomic values\n", num);
        printf("thread p[%d]: modificates atomic values\n", num);
        if (curr_length == max_length)
        {
            fprintf(f, "thread p[%d]: queue is full\n", num);
            printf("thread p[%d]: queue is full\n", num);
            queue_full++;
            if (queue_full == 2)
                break;
        }
        pthread_mutex_lock(&mcr1);
        while (empty())
        {
            fprintf(f, "thread p[%d]: is waiting for sig1_not_empty\n", num);
            printf("thread p[%d]: is waiting for sig1_not_empty\n", num);
            pthread_cond_wait(&sig1_not_empty, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_thread_state);
        temp = get_elem();
        curr_length--;
        pthread_setcancelstate(old_thread_state, NULL);
        fprintf(f, "\nthread p[%d]: element %d TAKEN; current queue length = %d;\n", num, temp->number, curr_length);
        printf("\nthread p[%d]: element %d TAKEN; current queue length = %d;\n", num, temp->number, curr_length);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_broadcast(&sig2_not_full);
        fprintf(f, "thread p[%d]: sends sig2_not_full\n", num);
        printf("thread p[%d]: sends sig2_not_full\n", num);
        free(temp);
        //usleep(100);
    }
    pthread_cancel(Pthread1);
    pthread_cancel(Pthread2);
    pthread_cancel(Pthread3);
    pthread_cancel(Pthread4);
    pthread_cancel(Pthread6);
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", num);
    return NULL;
}

void* P6(void* arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int num = *(int*)arg;
    fprintf(f, "thread p[%d]: is working\n", num);
    printf("thread p[%d]: is working\n", num);
    while (1)
    {
        pthread_mutex_lock(&mcr_sig21);
        while (flag21_P6 == 0)
        {
            fprintf(f, "thread p[%d]: is waiting for sig21\n", num);
            printf("thread p[%d]: is waiting for sig21\n", num);
            pthread_cond_wait(&sig21, &mcr_sig21);
        }
        pthread_mutex_unlock(&mcr_sig21);
        flag21_P6 = 0;
        fprintf(f, "thread p[%d]: received sig21\n", num);
        printf("thread p[%d]: received sig21\n", num);
        atomic_using();
        fprintf(f, "thread p[%d]: uses atomic values\n", num);
        printf("thread p[%d]: uses atomic values\n", num);
        pthread_mutex_lock(&mcr_sig22);
        while (flag22 == 0)
        {
            fprintf(f, "thread p[%d]: is waiting for sig22\n", num);
            printf("thread p[%d]: is waiting for sig22\n", num);
            pthread_cond_wait(&sig22, &mcr_sig22);
        }
        pthread_mutex_unlock(&mcr_sig22);
        flag22 = 0;
        fprintf(f, "thread p[%d]: received sig22\n", num);
        printf("thread p[%d]: received sig22\n", num);
        atomic_using();
        fprintf(f, "thread p[%d]: uses atomic values\n", num);
        printf("thread p[%d]: uses atomic values\n", num);
        //usleep(100);
    }
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", num);
    return NULL;
}
