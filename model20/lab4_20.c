#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

//threads
pthread_t p1;
pthread_t p2;
pthread_t p3;
pthread_t p4;
pthread_t p5;
pthread_t p6;

//mutex
pthread_mutex_t mcr1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_sig21 = PTHREAD_MUTEX_INITIALIZER;

//conditional signals
pthread_cond_t sig1_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t sig2_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t sig21 = PTHREAD_COND_INITIALIZER;

//flags for sig21
int flag_sig21 = 0;

//semaphores
sem_t scr21;
sem_t scr22;

//bufer values
int n = 20;
unsigned next_in = -1;
unsigned next_out = -1;
unsigned cur_numb = 0;
int *cr1 = NULL;

//atomic values
int int1 = 0;
int int2 = 0;
unsigned uns1 = 0;
unsigned uns2 = 0;
long long1 = 0;
long long2 = 0;
long unsigned luns1 = 0;
long unsigned luns2 = 0;

//control bufer
int empty_bufer = 0;
int full_bufer = 0;

//file
FILE *f = NULL;

//bufer function declarations
void create_buf();
void delete_buf();
void output_buf();
int is_buf_empty();
int is_buf_full();
void add_elem_to_buf();
int get_elem_from_buf();

//atomic function declarations
void atomic_input();
void atomic_modification();
void atomic_using();

//threads function declarations
void *P1_thread(void *p_number);
void *P2_thread(void *p_number);
void *P3_thread(void *p_number);
void *P4_thread(void *p_number);
void *P5_thread(void *p_number);
void *P6_thread(void *p_number);

int main()
{
    f = fopen("file.log", "w");
    if (f == NULL)
    {
        printf("\nerror file\n");
        exit(1);
    }
    srand(time(NULL));

    create_buf();
    for(unsigned i = 0; i < n / 2; i++)
    {
        add_elem_to_buf();
        cur_numb++;
    }
    fprintf(f, "Cycle buffer with elements from 0-th to %d-th has beencreated !!!\n", (n / 2));
    printf("Cycle buffer with elements from 0-th to %d-th has been created!!!\n", (n / 2));

    atomic_input();

    int p1_number = 1;
    int p2_number = 2;
    int p3_number = 3;
    int p4_number = 4;
    int p5_number = 5;
    int p6_number = 6;

    sem_init(&scr21, 0, 0);
    sem_init(&scr22, 0, 0);

    pthread_create(&p1, NULL, &P1_thread, (void*)&p1_number);
    pthread_create(&p2, NULL, &P2_thread, (void*)&p2_number);
    pthread_create(&p3, NULL, &P3_thread, (void*)&p3_number);
    pthread_create(&p4, NULL, &P4_thread, (void*)&p4_number);
    pthread_create(&p5, NULL, &P5_thread, (void*)&p5_number);
    pthread_create(&p6, NULL, &P6_thread, (void*)&p6_number);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    pthread_join(p4, NULL);
    pthread_join(p5, NULL);
    pthread_join(p6, NULL);


    fprintf(f, "\nAll information was written to the log file\n");
    printf("\nAll information was written to the log file\n");
    fclose(f);

    return 0;
}

void create_buf()
{
    cr1 = (int*)malloc(sizeof(int) * n);
    fprintf(f, "Cycle buffer was created\n");
    printf("Cycle buffer was created\n");
}

void delete_buf()
{
    free(cr1);
    fprintf(f, "\nCycle buffer was deleted\n\n");
    printf("\nCycle buffer was deleted\n\n");
}

int is_buf_empty()
{
    return cur_numb <= 0;
}

int is_buf_full()
{
    return cur_numb >= n;
}

void add_elem_to_buf()
{
    //cr1[next_in] = rand() % 50;
    next_in = (next_in + 1) % n;
    cr1[next_in] = cur_numb;
}

int get_elem_from_buf()
{
    next_out = (next_out + 1) % n;
    return cr1[next_out];
}

void output_buf()
{
    for (unsigned i = 0; i < n; i++)
    {
        fprintf(f, "%d ", cr1[i]);
        printf("%d ", cr1[i]);
    }
    fprintf(f, "\n");
    printf("\n");
}

void *P1_thread(void *p_number)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int thread_num = *((int*)p_number);
    fprintf(f, "thread p[%d]: started\n", thread_num);
    printf("thread p[%d]: started\n", thread_num);
    int prev_state = 0;
    while(1)
    {
        if (cur_numb == 0)
        {
            fprintf(f, "thread p[%d]: cycle bufer is empty\n", thread_num);
            printf("thread p[%d]: cycle bufer is empty\n", thread_num);
            empty_bufer++;
            if (empty_bufer == 2)
                break;
        }
        pthread_mutex_lock(&mcr1);
        while(is_buf_full())
        {
            fprintf(f, "thread p[%d]: is waiting for sig2_not_full\n", thread_num);
            printf("thread p[%d]: is waiting for sig2_not_full\n", thread_num);
            pthread_cond_wait(&sig2_not_full, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &prev_state);
        add_elem_to_buf();
        cur_numb++;
        pthread_setcancelstate(prev_state, NULL);
        fprintf(f, "\nthread p[%d]: element %d CREATED; current length = %d;\n", thread_num, cr1[next_in], cur_numb);
        printf("\nthread p[%d]: element %d CREATED; current length = %d;\n", thread_num, cr1[next_in], cur_numb);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_broadcast(&sig1_not_empty);
        fprintf(f, "thread p[%d]: sent sig1_not_empty\n", thread_num);
        printf("thread p[%d]: sent sig1_not_empty\n", thread_num);
        usleep(1);
    }
    pthread_cancel(p2);
    pthread_cancel(p3);
    pthread_cancel(p4);
    pthread_cancel(p5);
    pthread_cancel(p6);
    delete_buf();
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", thread_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", thread_num);
    return NULL;
}

void *P2_thread(void *p_number)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int thread_num = *((int*)p_number);
    fprintf(f, "thread p[%d]: started\n", thread_num);
    printf("thread p[%d]: started\n", thread_num);
    int prev_state = 0;
    int temp = 0;
    while(1)
    {
        pthread_mutex_lock(&mutex_sig21);
        while(flag_sig21 == 0)
        {
            fprintf(f, "thread p[%d]: is waiting for sig21\n", thread_num);
            printf("thread p[%d]: is waiting for sig21\n", thread_num);
            pthread_cond_wait(&sig21, &mutex_sig21);
        }
        flag_sig21 = 0;
        fprintf(f, "thread p[%d]: received sig21\n", thread_num);
        printf("thread p[%d]: received sig21\n", thread_num);
        pthread_mutex_unlock(&mutex_sig21);
        atomic_using();
        fprintf(f, "\nthread p[%d]: used atomic values\n", thread_num);
        printf("\nthread p[%d]: used atomic values\n", thread_num);
        atomic_modification();
        fprintf(f, "\nthread p[%d]: modificated atomic values\n", thread_num);
        printf("\nthread p[%d]: modificated atomic values\n", thread_num);
        if (cur_numb == n)
        {
            fprintf(f, "thread p[%d]: cycle bufer is full\n", thread_num);
            printf("thread p[%d]: cycle bufer is full\n", thread_num);
            full_bufer++;
            if (full_bufer == 2)
                break;
        }
        pthread_mutex_lock(&mcr1);
        while(is_buf_empty())
        {
            fprintf(f, "thread p[%d]: is waiting for sig1_not_empty\n", thread_num);
            printf("thread p[%d]: is waiting for sig1_not_empty\n", thread_num);
            pthread_cond_wait(&sig1_not_empty, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &prev_state);
        temp = get_elem_from_buf();
        cur_numb--;
        pthread_setcancelstate(prev_state, NULL);
        fprintf(f, "\nthread p[%d]: element %d TAKEN; current length = %d;\n", thread_num, temp, cur_numb);
        printf("\nthread p[%d]: element %d TAKEN; current length = %d;\n", thread_num, temp, cur_numb);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_signal(&sig2_not_full);
        fprintf(f, "thread p[%d]: sent sig2_not_full\n", thread_num);
        printf("thread p[%d]: sent sig2_not_full\n", thread_num);
    }
    pthread_cancel(p1);
    pthread_cancel(p3);
    pthread_cancel(p4);
    pthread_cancel(p5);
    pthread_cancel(p6);
    delete_buf();
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", thread_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", thread_num);
    return NULL;
}

void *P3_thread(void *p_number)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int thread_num = *((int*)p_number);
    fprintf(f, "thread p[%d]: started\n", thread_num);
    printf("thread p[%d]: started\n", thread_num);
    while(1)
    {
        pthread_mutex_lock(&mutex_sig21);
        while(flag_sig21 == 0)
        {
            fprintf(f, "thread p[%d]: is waiting for sig21\n", thread_num);
            printf("thread p[%d]: is waiting for sig21\n", thread_num);
            pthread_cond_wait(&sig21, &mutex_sig21);
        }
        flag_sig21 = 0;
        fprintf(f, "thread p[%d]: received sig21\n", thread_num);
        printf("thread p[%d]: received sig21\n", thread_num);
        pthread_mutex_unlock(&mutex_sig21);
        atomic_using();
        fprintf(f, "\nthread p[%d]: used atomic values\n", thread_num);
        printf("\nthread p[%d]: used atomic values\n", thread_num);
        fprintf(f, "thread p[%d]: opens semaphore scr22 for the thread p[6]\n", thread_num);
        printf("thread p[%d]: opens semaphore scr22 for the thread p[6]\n", thread_num);
        sem_post(&scr22);
        fprintf(f, "\nthread p[%d]: semaphore scr22 is opened\n", thread_num);
        printf("\nthread p[%d]: semaphore scr22 is opened\n", thread_num);
        fprintf(f, "\nthread p[%d]: is waiting the opening of the semaphore scr21\n", thread_num);
        printf("\nthread p[%d]: is waiting the opening of the semaphore scr21\n", thread_num);
        if (sem_trywait(&scr21) == 0) {
            sem_close(&scr21);
            sem_open("scr21", O_CREAT, 0, 0);
            fprintf(f, "thread p[%d]: is working after receiving the semaphore scr21\n", thread_num);
            printf("thread p[%d]: is working after receiving the semaphore scr21\n", thread_num);
            atomic_using();
            fprintf(f, "\nthread p[%d]: used atomic values\n", thread_num);
            printf("\nthread p[%d]: used atomic values\n", thread_num);
        }
        else {
        //
        }
    }
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", thread_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", thread_num);
    return NULL;
}

void *P4_thread(void *p_number)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int thread_num = *((int*)p_number);
    fprintf(f, "thread p[%d]: started\n", thread_num);
    printf("thread p[%d]: started\n", thread_num);
    int prev_state = 0;
    int temp = 0;
    while(1)
    {
        pthread_mutex_lock(&mutex_sig21);
        while(flag_sig21 == 0)
        {
            fprintf(f, "thread p[%d]: is waiting for sig21\n", thread_num);
            printf("thread p[%d]: is waiting for sig21\n", thread_num);
            pthread_cond_wait(&sig21, &mutex_sig21);
        }
        flag_sig21 = 0;
        fprintf(f, "thread p[%d]: received sig21\n", thread_num);
        printf("thread p[%d]: received sig21\n", thread_num);
        pthread_mutex_unlock(&mutex_sig21);
        if (cur_numb == n)
        {
            fprintf(f, "thread p[%d]: cycle bufer is full\n", thread_num);
            printf("thread p[%d]: cycle bufer is full\n", thread_num);
            full_bufer++;
            if (full_bufer == 2)
                break;
        }
        pthread_mutex_lock(&mcr1);
        while(is_buf_empty())
        {
            fprintf(f, "thread p[%d]: is waiting for sig1_not_empty\n", thread_num);
            printf("thread p[%d]: is waiting for sig1_not_empty\n", thread_num);
            pthread_cond_wait(&sig1_not_empty, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &prev_state);
        temp = get_elem_from_buf();
        cur_numb--;
        pthread_setcancelstate(prev_state, NULL);
        fprintf(f, "\nthread p[%d]: element %d TAKEN; current length = %d;\n", thread_num, temp, cur_numb);
        printf("\nthread p[%d]: element %d TAKEN; current length = %d;\n", thread_num, temp, cur_numb);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_signal(&sig2_not_full);
        fprintf(f, "thread p[%d]: sent sig2_not_full\n", thread_num);
        printf("thread p[%d]: sent sig2_not_full\n", thread_num);
    }
    pthread_cancel(p1);
    pthread_cancel(p2);
    pthread_cancel(p3);
    pthread_cancel(p5);
    pthread_cancel(p6);
    delete_buf();
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", thread_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", thread_num);
    return NULL;
}

void *P5_thread(void *p_number)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int thread_num = *((int*)p_number);
    fprintf(f, "thread p[%d]: started\n", thread_num);
    printf("thread p[%d]: started\n", thread_num);
    int prev_state = 0;
    int temp = 0;
    while(1)
    {
        atomic_modification();
        fprintf(f, "\nthread p[%d]: modificated atomic values\n", thread_num);
        printf("\nthread p[%d]: modificated atomic values\n", thread_num);
        pthread_mutex_lock(&mutex_sig21);
        pthread_cond_signal(&sig21);
        flag_sig21 = 1;
        fprintf(f, "thread p[%d]: sent sig21\n", thread_num);
        printf("thread p[%d]: sent sig21\n", thread_num);
        pthread_mutex_unlock(&mutex_sig21);
        if (cur_numb == n)
        {
            fprintf(f, "thread p[%d]: cycle bufer is full\n", thread_num);
            printf("thread p[%d]: cycle bufer is full\n", thread_num);
            full_bufer++;
            if (full_bufer == 2)
                break;
        }
        pthread_mutex_lock(&mcr1);
        while(is_buf_empty())
        {
            fprintf(f, "thread p[%d]: is waiting for sig1_not_empty\n", thread_num);
            printf("thread p[%d]: is waiting for sig1_not_empty\n", thread_num);
            pthread_cond_wait(&sig1_not_empty, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &prev_state);
        temp = get_elem_from_buf();
        cur_numb--;
        pthread_setcancelstate(prev_state, NULL);
        fprintf(f, "\nthread p[%d]: element %d TAKEN; current length = %d;\n", thread_num, temp, cur_numb);
        printf("\nthread p[%d]: element %d TAKEN; current length = %d;\n", thread_num, temp, cur_numb);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_signal(&sig2_not_full);
        fprintf(f, "thread p[%d]: sent sig2_not_full\n", thread_num);
        printf("thread p[%d]: sent sig2_not_full\n", thread_num);
        usleep(1);
    }
    pthread_cancel(p1);
    pthread_cancel(p2);
    pthread_cancel(p3);
    pthread_cancel(p4);
    pthread_cancel(p6);
    delete_buf();
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", thread_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", thread_num);
    return NULL;
}

void *P6_thread(void *p_number)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int thread_num = *((int*)p_number);
    fprintf(f, "thread p[%d]: started\n", thread_num);
    printf("thread p[%d]: started\n", thread_num);
    while(1)
    {
        pthread_mutex_lock(&mutex_sig21);
        while(flag_sig21 == 0)
        {
            fprintf(f, "thread p[%d]: is waiting for sig21\n", thread_num);
            printf("thread p[%d]: is waiting for sig21\n", thread_num);
            pthread_cond_wait(&sig21, &mutex_sig21);
        }
        flag_sig21 = 0;
        fprintf(f, "thread p[%d]: received sig21\n", thread_num);
        printf("thread p[%d]: received sig21\n", thread_num);
        pthread_mutex_unlock(&mutex_sig21);
        atomic_using();
        fprintf(f, "\nthread p[%d]: used atomic values\n", thread_num);
        printf("\nthread p[%d]: used atomic values\n", thread_num);
        fprintf(f, "thread p[%d]: opens semaphore scr21 for the thread p[3]\n", thread_num);
        printf("thread p[%d]: opens semaphore scr21 for the thread p[3]\n", thread_num);
        sem_post(&scr21);
        fprintf(f, "\nthread p[%d]: semaphore scr21 is opened\n", thread_num);
        printf("\nthread p[%d]: semaphore scr21 is opened\n", thread_num);
        fprintf(f, "\nthread p[%d]: is waiting the opening of the semaphore scr22\n", thread_num);
        printf("\nthread p[%d]: is waiting the opening of the semaphore scr22\n", thread_num);
        sem_wait(&scr22);
        sem_close(&scr22);
        sem_open("scr22", O_CREAT, 0, 0);
        fprintf(f, "thread p[%d]: is working after receiving the semaphore scr22\n", thread_num);
        printf("thread p[%d]: is working after receiving the semaphore scr22\n", thread_num);
        atomic_modification();
        fprintf(f, "\nthread p[%d]: modificated atomic values\n", thread_num);
        printf("\nthread p[%d]: modificated atomic values\n", thread_num);
    }
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", thread_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", thread_num);
    return NULL;
}

void atomic_input()
{
    int1 = (rand() % 6) - 3;
    int2 = (rand() % 6) - 3;
    uns1 = (rand() % 6) - 3;
    uns2 = (rand() % 6) - 3;
    long1 = (rand() % 6) - 3;
    long2 = (rand() % 6) - 3;
    luns1 = (rand() % 6) - 3;
    luns2 = (rand() % 6) - 3;
}

void atomic_modification()
{
    __sync_fetch_and_add(&int1, (rand() % 4) - 2);
    __sync_add_and_fetch(&int2, (rand() % 4) - 2);
    __sync_fetch_and_or(&uns1, (rand() % 4) - 2);
    __sync_fetch_and_xor(&uns2, (rand() % 4) - 2);
    __sync_and_and_fetch(&long1, (rand() % 4) - 2);
    __sync_nand_and_fetch(&long2, (rand() % 4) - 2);
    __sync_bool_compare_and_swap (&luns1, (rand() % 4) - 2, (rand() % 4) - 2);
    __sync_val_compare_and_swap (&luns2, (rand() % 4) - 2, (rand() % 4) - 2);
}

void atomic_using()
{
    __sync_fetch_and_add(&int1, 0);
    __sync_add_and_fetch(&int2, 0);
    __sync_fetch_and_or(&uns1, 0);
    __sync_fetch_and_xor(&uns2, 0);
    __sync_and_and_fetch(&long1, long1);
    __sync_nand_and_fetch(&long2, long2);
    __sync_nand_and_fetch(&long2, long2);
    __sync_bool_compare_and_swap (&luns1, 0, 0);
    __sync_val_compare_and_swap (&luns2, 0, 0);
}
