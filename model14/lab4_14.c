#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX_SIZE_OF_STACK 24

//threads
pthread_t p1;
pthread_t p2;
pthread_t p3;
pthread_t p4;
pthread_t p5;
pthread_t p6;

//semaphores
sem_t scr21;
sem_t scr22;

//mutex init
pthread_mutex_t mcr1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_for_sig21 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_for_sig22 = PTHREAD_MUTEX_INITIALIZER;

//conditional signals
pthread_cond_t sig1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t sig2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t sig21 = PTHREAD_COND_INITIALIZER;
pthread_cond_t sig22 = PTHREAD_COND_INITIALIZER;

//file
FILE *f = NULL;

//stack values
int curr_size_of_stack = 0;
int stack[MAX_SIZE_OF_STACK];

//atomic values
int cr2_int1 = 0;
int cr2_int2 = 0;
unsigned cr2_uns1 = 0;
unsigned cr2_uns2 = 0;
long cr2_long1 = 0;
long cr2_long2 = 0;
long unsigned cr2_luns1 = 0;
long unsigned cr2_luns2 = 0;

//flags for signals
int flag21_P1 = 0;
int flag21_P2 = 0;
int flag21_P4 = 0;
int flag22 = 0;

//control bufer
int empty_buf = 0;
int full_buf = 0;

//bufer function declarations
int IsFull();
int IsEmpty();
void AddElem();
int GetElem();

//atomic function declarations
void AtomicInput();
void AtomicModificationFirst();
void AtomicModificationSecond();
void AtomicUsingFirst();
void AtomicUsingSecond();

//threads function declarations
void *P1thread(void *p_number);
void *P2thread(void *p_number);
void *P3thread(void *p_number);
void *P4thread(void *p_number);
void *P5thread(void *p_number);
void *P6thread(void *p_number);

int main() {
    f = fopen("file.log", "w");
    if (f == NULL) {
        printf("\nerror file\n");
        exit(1);
    }
    srand(time(NULL));
    for(int i = 0; i < MAX_SIZE_OF_STACK / 2; i++) {
        AddElem();
        curr_size_of_stack++;
    }
    fprintf(f, "Stack buffer with elements from 0-th to %d-th has been created !!!\n", (MAX_SIZE_OF_STACK / 2));
    printf("Stack buffer with elements from 0-th to %d-th has been created !!!\n", (MAX_SIZE_OF_STACK / 2));
    AtomicInput();

    int p1_number = 1;
    int p2_number = 2;
    int p3_number = 3;
    int p4_number = 4;
    int p5_number = 5;
    int p6_number = 6;

    int p1_join = -1;
    int p2_join = -1;
    int p3_join = -1;
    int p4_join = -1;
    int p5_join = -1;
    int p6_join = -1;

    sem_init(&scr21, 0, 0);
    sem_init(&scr22, 0, 0);

    pthread_create(&p1, NULL, &P1thread, (void*)&p1_number);
    pthread_create(&p2, NULL, &P2thread, (void*)&p2_number);
    pthread_create(&p3, NULL, &P3thread, (void*)&p3_number);
    pthread_create(&p4, NULL, &P4thread, (void*)&p4_number);
    pthread_create(&p5, NULL, &P5thread, (void*)&p5_number);
    pthread_create(&p6, NULL, &P6thread, (void*)&p6_number);

    p1_join = pthread_join(p1, NULL);
    fprintf(f, "\npthread_join p%d = %d", p1_number, p1_join);
    printf("\npthread_join p%d = %d", p1_number, p1_join);
    p2_join = pthread_join(p2, NULL);
    fprintf(f, "\npthread_join p%d = %d", p2_number, p2_join);
    printf("\npthread_join p%d = %d", p2_number, p2_join);
    p3_join = pthread_join(p3, NULL);
    fprintf(f, "\npthread_join p%d = %d", p3_number, p3_join);
    printf("\npthread_join p%d = %d", p3_number, p3_join);
    p4_join = pthread_join(p4, NULL);
    fprintf(f, "\npthread_join p%d = %d", p4_number, p4_join);
    printf("\npthread_join p%d = %d", p4_number, p4_join);
    p5_join = pthread_join(p5, NULL);
    fprintf(f, "\npthread_join p%d = %d", p5_number, p5_join);
    printf("\npthread_join p%d = %d", p5_number, p5_join);
    p6_join = pthread_join(p6, NULL);
    fprintf(f, "\npthread_join p%d = %d\n", p6_number, p6_join);
    printf("\npthread_join p%d = %d\n", p6_number, p6_join);

    fprintf(f, "\nAll information was written to the log file\n");
    printf("\nAll information was written to the log file\n");
    fclose(f);

    return 0;
}

int IsFull() {
    return curr_size_of_stack >= MAX_SIZE_OF_STACK;
}

int IsEmpty() {
    return curr_size_of_stack <= 0;
}

void AddElem() {
    if (curr_size_of_stack == 0) {
        fprintf(f, "Stack bufer was created\n");
        printf("Stack bufer was created\n");
    }
    stack[curr_size_of_stack] = curr_size_of_stack;
}

int GetElem() {
    return stack[curr_size_of_stack];
}

void *P1thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    int old = 0;
    while(1) {
        pthread_mutex_lock(&mutex_for_sig21);
        while(flag21_P1 == 0) {
            fprintf(f, "thread p[%d]: is waiting for sig21\n", p_num);
            printf("thread p[%d]: is waiting for sig21\n", p_num);
            pthread_cond_wait(&sig21, &mutex_for_sig21);
        }
        pthread_mutex_unlock(&mutex_for_sig21);
        flag21_P1 = 0;
        fprintf(f, "thread p[%d]: received sig21\n", p_num);
        printf("thread p[%d]: received sig21\n", p_num);
        AtomicUsingFirst();
        fprintf(f, "thread p[%d]: used first atomic values\n", p_num);
        printf("thread p[%d]: used first atomic values\n", p_num);
        fprintf(f, "thread p[%d]: opens semaphore scr22 for the thread p[4]\n", p_num);
        printf("thread p[%d]: opens semaphore scr22 for the thread p[4]\n", p_num);
        sem_post(&scr22);
        fprintf(f, "\nthread p[%d]: semaphore scr22 is opened\n", p_num);
        printf("\nthread p[%d]: semaphore scr22 is opened\n", p_num);
        fprintf(f, "\nthread p[%d]: waits for the opening of the semaphore scr21\n", p_num);
        printf("\nthread p[%d]: waits for the opening of the semaphore scr21\n", p_num);
        if (sem_trywait(&scr21) == 0) {
            sem_close(&scr21);
            sem_open("scr21", O_CREAT, 0, 0);
            fprintf(f, "thread p[%d]: works after receiving semaphore scr21\n", p_num);
            printf("thread p[%d]: works after receiving semaphore scr21\n", p_num);
            pthread_mutex_lock(&mutex_for_sig22);
            while(flag22 == 0) {
                fprintf(f, "thread p[%d]: is waiting for sig22\n", p_num);
                printf("thread p[%d]: is waiting for sig22\n", p_num);
                pthread_cond_wait(&sig22, &mutex_for_sig22);
            }
            flag22 = 0;
            pthread_mutex_unlock(&mutex_for_sig22);
            fprintf(f, "thread p[%d]: received sig22\n", p_num);
            printf("thread p[%d]: received sig22\n", p_num);
            AtomicUsingSecond();
            fprintf(f, "thread p[%d]: used second atomic values\n", p_num);
            printf("thread p[%d]: used second atomic values\n", p_num);
        }
        else {
        //
        }
    }
    fprintf(f, "thread p[%d]: stopped\n", p_num);
    printf("thread p[%d]: stopped\n", p_num);
    return NULL;
}

void *P2thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    int old = 0;
    int tmp = -1;
    while(1) {
        pthread_mutex_lock(&mutex_for_sig21);
        while(flag21_P2 == 0) {
            fprintf(f, "thread p[%d]: is waiting for sig21\n", p_num);
            printf("thread p[%d]: is waiting for sig21\n", p_num);
            pthread_cond_wait(&sig21, &mutex_for_sig21);
        }
        pthread_mutex_unlock(&mutex_for_sig21);
        flag21_P2 = 0;
        fprintf(f, "thread p[%d]: received sig21\n", p_num);
        printf("thread p[%d]: received sig21\n", p_num);
        AtomicUsingFirst();
        fprintf(f, "thread p[%d]: used first atomic values\n", p_num);
        printf("thread p[%d]: used first atomic values\n", p_num);
        tmp = -1;
        if (curr_size_of_stack == MAX_SIZE_OF_STACK) {
            fprintf(f, "thread p[%d]: stack bufer is full\n", p_num);
            printf("thread p[%d]: stack bufer is full\n", p_num);
            full_buf++;
            if (full_buf == 2)
                break;
        }
        if (pthread_mutex_trylock(&mcr1) == 0) {
            while(IsEmpty()) {
                fprintf(f, "thread p[%d]: is waiting for sig2\n", p_num);
                printf("thread p[%d]: is waiting for sig2\n", p_num);
                pthread_cond_wait(&sig1, &mcr1);
            }
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old);
            curr_size_of_stack--;
            tmp = GetElem();
            pthread_setcancelstate(old, NULL);
            fprintf(f, "\nthread p[%d]: element %d TAKEN; current_size = %d;\n", p_num, tmp, curr_size_of_stack);
            printf("\nthread p[%d]: element %d TAKEN; current_size = %d;\n", p_num, tmp, curr_size_of_stack);
            pthread_mutex_unlock(&mcr1);
            pthread_cond_broadcast(&sig2);
            fprintf(f, "thread p[%d]: sent sig2\n", p_num);
            printf("thread p[%d]: sent sig2\n", p_num);
        }
        else {
            //printf("\nthread p[%d]: does some useful work\n", p_num);
        }
    }
    pthread_cancel(p1);
    pthread_cancel(p3);
    pthread_cancel(p4);
    pthread_cancel(p5);
    pthread_cancel(p6);
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", p_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", p_num);
	return NULL;
}

void *P3thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    int old = 0;
    int tmp = -1;
    while(1) {
        tmp = -1;
        if (curr_size_of_stack == MAX_SIZE_OF_STACK) {
            fprintf(f, "thread p[%d]: stack bufer is full\n", p_num);
            printf("thread p[%d]: stack bufer is full\n", p_num);
            full_buf++;
            if (full_buf == 2)
                break;
        }
        if (pthread_mutex_trylock(&mcr1) == 0) {
            while(IsEmpty()) {
                fprintf(f, "thread p[%d]: is waiting for sig2\n", p_num);
                printf("thread p[%d]: is waiting for sig2\n", p_num);
                pthread_cond_wait(&sig1, &mcr1);
            }
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old);
            curr_size_of_stack--;
            tmp = GetElem();
            pthread_setcancelstate(old, NULL);
            fprintf(f, "\nthread p[%d]: element %d TAKEN; current_size = %d;\n", p_num, tmp, curr_size_of_stack);
            printf("\nthread p[%d]: element %d TAKEN; current_size = %d;\n", p_num, tmp, curr_size_of_stack);
            pthread_mutex_unlock(&mcr1);
            pthread_cond_broadcast(&sig2);
            fprintf(f, "thread p[%d]: sent sig2\n", p_num);
            printf("thread p[%d]: sent sig2\n", p_num);
        }
        else {
            //printf("\nthread p[%d]: does some useful work\n", p_num);
        }
    }
    pthread_cancel(p1);
    pthread_cancel(p2);
    pthread_cancel(p4);
    pthread_cancel(p5);
    pthread_cancel(p6);
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", p_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", p_num);
	return NULL;
}

void *P4thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    int old = 0;
    while(1) {
        pthread_mutex_lock(&mutex_for_sig21);
        while(flag21_P4 == 0) {
            fprintf(f, "thread p[%d]: is waiting for sig21\n", p_num);
            printf("thread p[%d]: is waiting for sig21\n", p_num);
            pthread_cond_wait(&sig21, &mutex_for_sig21);
        }
        pthread_mutex_unlock(&mutex_for_sig21);
        flag21_P4 = 0;
        fprintf(f, "thread p[%d]: received sig21\n", p_num);
        printf("thread p[%d]: received sig21\n", p_num);
        pthread_mutex_lock(&mutex_for_sig22);
        while(flag22 == 0) {
            fprintf(f, "thread p[%d]: is waiting for sig22\n", p_num);
            printf("thread p[%d]: is waiting for sig22\n", p_num);
            pthread_cond_wait(&sig22, &mutex_for_sig22);
        }
        flag22 = 0;
        pthread_mutex_unlock(&mutex_for_sig22);
        fprintf(f, "thread p[%d]: received sig22\n", p_num);
        printf("thread p[%d]: received sig22\n", p_num);
        AtomicUsingFirst();
        AtomicUsingSecond();
        fprintf(f, "thread p[%d]: used all atomic values\n", p_num);
        printf("thread p[%d]: used all atomic values\n", p_num);
        AtomicModificationFirst();
        AtomicModificationSecond();
        fprintf(f, "\nthread p[%d]: modificated all atomic values\n", p_num);
        printf("\nthread p[%d]: modificated all atomic values\n", p_num);
        fprintf(f, "thread p[%d]: opens semaphore scr21 for the thread p[1]\n", p_num);
        printf("thread p[%d]: opens semaphore scr21 for the thread p[1]\n", p_num);
        sem_post(&scr21);
        fprintf(f, "\nthread p[%d]: semaphore scr21 is opened\n", p_num);
        printf("\nthread p[%d]: semaphore scr21 is opened\n", p_num);
        fprintf(f, "\nthread p[%d]: waits for the opening of the semaphore scr22\n", p_num);
        printf("\nthread p[%d]: waits for the opening of the semaphore scr22\n", p_num);
        if (sem_trywait(&scr22) == 0) {
            sem_close(&scr22);
            sem_open("scr22", O_CREAT, 0, 0);
            fprintf(f, "thread p[%d]: works after receiving semaphore scr22\n", p_num);
            printf("thread p[%d]: works after receiving semaphore scr22\n", p_num);
            AtomicUsingFirst();
            AtomicUsingSecond();
            fprintf(f, "thread p[%d]: used all atomic values\n", p_num);
            printf("thread p[%d]: used all atomic values\n", p_num);
            AtomicModificationFirst();
            AtomicModificationSecond();
            fprintf(f, "\nthread p[%d]: modificated all atomic values\n", p_num);
            printf("\nthread p[%d]: modificated all atomic values\n", p_num);
        }
        else {
        //
        }
    }
    fprintf(f, "thread p[%d]: stopped\n", p_num);
    printf("thread p[%d]: stopped\n", p_num);
    return NULL;
}

void *P5thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    int old = 0;
    while(1) {
        AtomicModificationFirst();
        fprintf(f, "\nthread p[%d]: modificated first atomic values\n", p_num);
        printf("\nthread p[%d]: modificated first atomic values\n", p_num);
        pthread_mutex_lock(&mutex_for_sig21);
        pthread_cond_broadcast(&sig21);
        flag21_P1 = 1;
        flag21_P2 = 1;
        flag21_P4 = 1;
        pthread_mutex_unlock(&mutex_for_sig21);
        fprintf(f, "thread p[%d]: sent sig21\n", p_num);
        printf("thread p[%d]: sent sig21\n", p_num);
        AtomicModificationSecond();
        fprintf(f, "\nthread p[%d]: modificated second atomic values\n", p_num);
        printf("\nthread p[%d]: modificated second atomic values\n", p_num);
        pthread_mutex_lock(&mutex_for_sig22);
        pthread_cond_signal(&sig22);
        flag22 = 1;
        pthread_mutex_unlock(&mutex_for_sig22);
        fprintf(f, "thread p[%d]: sent sig22\n", p_num);
        printf("thread p[%d]: sent sig22\n", p_num);
        if (curr_size_of_stack == 0) {
            fprintf(f, "thread p[%d]: stack bufer is empty\n", p_num);
            printf("thread p[%d]: stack bufer is empty\n", p_num);
            empty_buf++;
            if (empty_buf == 2)
                break;
        }
        if (pthread_mutex_trylock(&mcr1) == 0) {
            while(IsFull()) {
                fprintf(f, "thread p[%d]: is waiting for sig2\n", p_num);
                printf("thread p[%d]: is waiting for sig2\n", p_num);
                pthread_cond_wait(&sig2, &mcr1);
            }
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old);
            AddElem();
            curr_size_of_stack++;
            pthread_setcancelstate(old, NULL);
            fprintf(f, "\nthread p[%d]: element %d CREATED; current_size = %d;\n", p_num, stack[curr_size_of_stack - 1], curr_size_of_stack);
            printf("\nthread p[%d]: element %d CREATED; current_size = %d;\n", p_num, stack[curr_size_of_stack - 1], curr_size_of_stack);
            pthread_mutex_unlock(&mcr1);
            pthread_cond_broadcast(&sig1);
            fprintf(f, "thread p[%d]: sent sig1\n", p_num);
            printf("thread p[%d]: sent sig1\n", p_num);
        }
        else {
            //printf("\nthread p[%d]: does some useful work\n", p_num);
            usleep(20);
        }
    }
    pthread_cancel(p1);
    pthread_cancel(p2);
    pthread_cancel(p3);
    pthread_cancel(p4);
    pthread_cancel(p6);
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", p_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", p_num);
	return NULL;
}

void *P6thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    int old = 0;
    while(1) {
        if (curr_size_of_stack == 0) {
            fprintf(f, "thread p[%d]: stack bufer is empty\n", p_num);
            printf("thread p[%d]: stack bufer is empty\n", p_num);
            empty_buf++;
            if (empty_buf == 2)
                break;
        }
        if (pthread_mutex_trylock(&mcr1) == 0) {
            while(IsFull()) {
                fprintf(f, "thread p[%d]: is waiting for sig2\n", p_num);
                printf("thread p[%d]: is waiting for sig2\n", p_num);
                pthread_cond_wait(&sig2, &mcr1);
            }
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old);
            AddElem();
            curr_size_of_stack++;
            pthread_setcancelstate(old, NULL);
            fprintf(f, "\nthread p[%d]: element %d CREATED; current_size = %d;\n", p_num, stack[curr_size_of_stack - 1], curr_size_of_stack);
            printf("\nthread p[%d]: element %d CREATED; current_size = %d;\n", p_num, stack[curr_size_of_stack - 1], curr_size_of_stack);
            pthread_mutex_unlock(&mcr1);
            pthread_cond_broadcast(&sig1);
            fprintf(f, "thread p[%d]: sent sig1\n", p_num);
            printf("thread p[%d]: sent sig1\n", p_num);
        }
        else {
            //printf("\nthread p[%d]: does some useful work\n", p_num);
        }
    }
    pthread_cancel(p1);
    pthread_cancel(p2);
    pthread_cancel(p3);
    pthread_cancel(p4);
    pthread_cancel(p5);
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", p_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", p_num);
	return NULL;
}

void AtomicInput() {
    cr2_int1 = (rand() % 8) - 4;
    cr2_int2 = (rand() % 8) - 4;
    cr2_uns1 = (rand() % 8) - 4;
    cr2_uns2 = (rand() % 8) - 4;
    cr2_long1 = (rand() % 8) - 4;
    cr2_long2 = (rand() % 8) - 4;
    cr2_luns1 = (rand() % 8) - 4;
    cr2_luns2 = (rand() % 8) - 4;
}

void AtomicModificationFirst() {
    __sync_fetch_and_sub(&cr2_int1, (rand() % 8) - 4);
    __sync_sub_and_fetch(&cr2_int2, (rand() % 8) - 4);
    __sync_fetch_and_or(&cr2_uns1, (rand() % 8) - 4);
    __sync_fetch_and_xor(&cr2_uns2, (rand() % 8) - 4);
}

void AtomicModificationSecond() {
    __sync_and_and_fetch(&cr2_long1, (rand() % 8) - 4);
    __sync_nand_and_fetch(&cr2_long2, (rand() % 8) - 4);
    __sync_bool_compare_and_swap (&cr2_luns1, (rand() % 8) - 4, (rand() % 8) - 4);
    __sync_val_compare_and_swap (&cr2_luns2, (rand() % 8) - 4, (rand() % 8) - 4);
}

void AtomicUsingFirst() {
    __sync_fetch_and_sub(&cr2_int1, 0);
    __sync_sub_and_fetch(&cr2_int2, 0);
    __sync_fetch_and_or(&cr2_uns1, 0);
    __sync_fetch_and_xor(&cr2_uns2, 0);
}

void AtomicUsingSecond() {
    __sync_and_and_fetch(&cr2_long1, cr2_long1);
    __sync_nand_and_fetch(&cr2_long2, cr2_long2);
    __sync_nand_and_fetch(&cr2_long2, cr2_long2);
    __sync_bool_compare_and_swap (&cr2_luns1, 0, 0);
    __sync_val_compare_and_swap (&cr2_luns2, 0, 0);
}
