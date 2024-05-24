#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>

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

//conditional signals
pthread_cond_t sig1_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t sig2_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t sig21 = PTHREAD_COND_INITIALIZER;

//file
FILE *f = NULL;

//dsd bufer values
struct cr1 {
    struct cr1 *next;
    int numb;
};

struct cr1 *next_in = NULL;
struct cr1 *next_out = NULL;
struct cr1 *begin_buf = NULL;
struct cr1 *end_buf = NULL;

unsigned int n = 200;
unsigned int el_numb = 0;
unsigned int new_buf = 0;

//atomic values
int int1_cr2 = 0;
int int2_cr2 = 0;
unsigned uns1_cr2 = 0;
unsigned uns2_cr2 = 0;
long long1_cr2 = 0;
long long2_cr2 = 0;
long unsigned luns1_cr2 = 0;
long unsigned luns2_cr2 = 0;

//control bufer
int empty_bufer = 0;
int full_bufer = 0;

//bufer function declarations
void DeleteBuf();
void OutputBuf();
int IsBufEmpty();
int IsBufFull();
void AddElemToBuf();
struct cr1 *GetElemFromBuf();

//atomic function declarations
void AddAtomicToBuf(int value);
void AtomicInput();
void AtomicModification();
void AtomicUsing();
void AtomicOutput();

//threads function declarations
void *P1thread(void *p_number);
void *P2thread(void *p_number);
void *P3thread(void *p_number);
void *P4thread(void *p_number);
void *P5thread(void *p_number);
void *P6thread(void *p_number);

int main() {
    f = fopen("log.txt", "w");
    if (f == NULL) {
        printf("\nerror file\n");
        exit(1);
    }

    srand(time(NULL));

    for(unsigned int i = 0; i < n / 2; i++)
        AddElemToBuf();

    fprintf(f, "Cycle buffer with elements from 0-th to %d-th has been created !!!\n", (n / 2));
    printf("Cycle buffer with elements from 0-th to %d-th has been created !!!\n", (n / 2));

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

void DeleteBuf() {
    struct cr1 *temp = begin_buf;
    struct cr1 *temp_free = NULL;
    if (temp == NULL)
        return;
    while(temp != end_buf) {
        temp_free = temp;
        temp = temp->next;
        free(temp_free);
    }
    if (temp != NULL) {
        free(temp);
    }
    else {
        printf("\nCycle buffer error delete\n\n");
        fprintf(f, "\nCycle buffer error delete\n\n");
    }
    fprintf(f, "\nCycle buffer was deleted\n\n");
    printf("\nCycle buffer was deleted\n\n");
}

int IsBufEmpty() {
    return el_numb <= 0;
}

int IsBufFull() {
    return el_numb >= n;
}

void AddElemToBuf() {
    if (new_buf < n) {
        struct cr1 *temp = (struct cr1 *)malloc(sizeof(struct cr1));
        temp->next = NULL;
        if (next_in == NULL) {
            //temp->numb = rand() % 50;
            temp->numb = new_buf;
            next_in = temp;
            next_out = temp;
            begin_buf = temp;
            end_buf = temp;
            new_buf++;
            fprintf(f, "Cycle bufer was created\n");
            printf("Cycle bufer was created\n");
        }
        else {
            if (el_numb > n) {
                fprintf(f, "\nerror el_numb > %d\n", n);
                printf("\nerror el_numb > %d\n", n);
                free(temp);
                return;
            }
            //temp->numb = rand() % 50;
            temp->numb = new_buf;
            next_in->next = temp;
            next_in = temp;
            new_buf++;
            if (new_buf == n)
                temp->next = begin_buf;
            end_buf = temp;
        }
        el_numb++;
    }
    else {
        next_in->next->numb = rand() % 50;
        if (next_in == end_buf)
            next_in = begin_buf;
        else
            next_in = next_in->next;
        el_numb++;
    }
}

struct cr1 *GetElemFromBuf() {
    struct cr1 *temp = NULL;
    if (el_numb < 0) {
        fprintf(f, "\nerror el_numb < 0\n");
        printf("\nerror el_numb < 0\n");
        return NULL;
    }
    temp = next_out;
    if (temp == end_buf)
        next_out = begin_buf;
    else
        next_out = next_out->next;
    el_numb--;
    return temp;
}

void OutputBuf() {
    struct cr1 *temp = begin_buf;
    if (temp == NULL)
        return;
    while (temp != end_buf) {
        fprintf(f, "%d ", temp->numb);
        printf("%d ", temp->numb);
        temp = temp->next;
    }
    if (temp != NULL) {
        fprintf(f, "%d ", temp->numb);
        printf("%d ", temp->numb);
        fprintf(f, "\n");
        printf("\n");
    }
    else {
        printf("\nCycle buffer error output\n\n");
        fprintf(f, "\nCycle buffer error output\n\n");
    }
}

void *P1thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    AtomicModification();
    fprintf(f, "\nthread p[%d]: modificated atomic values\n", p_num);
    printf("\nthread p[%d]: modificated atomic values\n", p_num);
    while (1) {
        fprintf(f, "thread p[%d]: opens semaphore scr22 for the thread p[4]\n", p_num);
        printf("thread p[%d]: opens semaphore scr22 for the thread p[4]\n", p_num);
        sem_post(&scr22);
        fprintf(f, "\nthread p[%d]: semaphore scr22 is opened\n", p_num);
        printf("\nthread p[%d]: semaphore scr22 is opened\n", p_num);
        fprintf(f, "\nthread p[%d]: waits for the opening of the semaphore scr21\n", p_num);
        printf("\nthread p[%d]: waits for the opening of the semaphore scr21\n", p_num);
        trywait:
        if (sem_trywait(&scr21) == 0) {
            sem_close(&scr21);
            sem_open("scr21", O_CREAT, 0, 0);
            fprintf(f, "thread p[%d]: works after receiving semaphore scr21\n", p_num);
            printf("thread p[%d]: works after receiving semaphore scr21\n", p_num);
            pthread_mutex_lock(&mutex_for_sig21);
            pthread_cond_signal(&sig21);
            pthread_mutex_unlock(&mutex_for_sig21);
            fprintf(f, "thread p[%d]: sent sig21\n", p_num);
            printf("thread p[%d]: sent sig21\n", p_num);
            AtomicUsing();
            fprintf(f, "thread p[%d]: used atomic values\n", p_num);
            printf("thread p[%d]: used atomic values\n", p_num);
            AtomicModification();
            fprintf(f, "\nthread p[%d]: modificated atomic values\n", p_num);
            printf("\nthread p[%d]: modificated atomic values\n", p_num);
        }
        else {
            //printf("\nthread p[%d]: does some useful work\n", p_num);
            goto trywait;
        }
    }
    fprintf(f, "thread p[%d]: stopped\n", p_num);
    printf("thread p[%d]: stopped\n", p_num);
    return NULL;
}

void *P2thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    int flag = 0;
    int old_state = 0;
    while(1) {
        fprintf(f, "thread p[%d]: is waiting for sig21\n", p_num);
        printf("thread p[%d]: is waiting for sig21\n", p_num);
        pthread_mutex_lock(&mutex_for_sig21);
        pthread_cond_wait(&sig21, &mutex_for_sig21);
        pthread_mutex_unlock(&mutex_for_sig21);
        fprintf(f, "thread p[%d]: received sig21\n", p_num);
        printf("thread p[%d]: received sig21\n", p_num);
        AtomicUsing();
        fprintf(f, "thread p[%d]: used atomic values\n", p_num);
        printf("thread p[%d]: used atomic values\n", p_num);
        if (el_numb == 0) {
            fprintf(f, "thread p[%d]: bufer is empty\n", p_num);
            printf("thread p[%d]: bufer is empty\n", p_num);
            empty_bufer++;
            if (empty_bufer == 2)
                break;
        }
        if (pthread_mutex_trylock(&mcr1) == 0) {
            while(IsBufFull())
                pthread_cond_wait(&sig2_not_full, &mcr1);
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
            if (flag == 0)
                AddAtomicToBuf((int)int1_cr2);
            else if (flag == 1)
                AddAtomicToBuf((int)int2_cr2);
            else if (flag == 2)
                AddAtomicToBuf((int)uns1_cr2);
            else if (flag == 3)
                AddAtomicToBuf((int)uns2_cr2);
            else if (flag == 4)
                AddAtomicToBuf((int)long1_cr2);
            else if (flag == 5)
                AddAtomicToBuf((int)long2_cr2);
            else if (flag == 6)
                AddAtomicToBuf((int)luns1_cr2);
            else if (flag == 7)
                AddAtomicToBuf((int)luns2_cr2);
            pthread_setcancelstate(old_state, NULL);
            flag++;
            if (flag == 8)
                flag = 0;
            fprintf(f, "\nthread p[%d]: element %d ATOMIC CREATED; current el_numb = %d;\n", p_num, next_in->numb, el_numb);
            printf("\nthread p[%d]: element %d ATOMIC CREATED; current el_numb = %d;\n", p_num, next_in->numb, el_numb);
            pthread_mutex_unlock(&mcr1);
            pthread_cond_signal(&sig1_not_empty);
            fprintf(f, "thread p[%d]: sent sig1_not_empty\n", p_num);
            printf("thread p[%d]: sent sig1_not_empty\n", p_num);
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
    DeleteBuf();
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", p_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", p_num);
    return NULL;
}

void *P3thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    struct cr1 *temp = NULL;
    int old_state = 0;
    while(1) {
        temp = 0;
        if (el_numb == n) {
            fprintf(f, "thread p[%d]: bufer is full\n", p_num);
            printf("thread p[%d]: bufer is full\n", p_num);
            full_bufer++;
            if (full_bufer == 2)
                break;
        }
        if (pthread_mutex_trylock(&mcr1) == 0) {
            while(IsBufEmpty()) {
                fprintf(f, "thread p[%d]: is waiting for sig2_not_empty\n", p_num);
                printf("thread p[%d]: is waiting for sig2_not_empty\n", p_num);
                pthread_cond_wait(&sig1_not_empty, &mcr1);
            }
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
            temp = GetElemFromBuf();
            pthread_setcancelstate(old_state, NULL);
            fprintf(f, "\nthread p[%d]: element %d TAKEN; current el_numb = %d;\n", p_num, temp->numb, el_numb);
            printf("\nthread p[%d]: element %d TAKEN; current el_numb = %d;\n", p_num, temp->numb, el_numb);
            pthread_mutex_unlock(&mcr1);
            pthread_cond_broadcast(&sig2_not_full);
            fprintf(f, "thread p[%d]: sent sig2_not_full\n", p_num);
            printf("thread p[%d]: sent sig2_not_full\n", p_num);
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
    DeleteBuf();
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", p_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", p_num);
	return NULL;
}

void *P4thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    AtomicModification();
    fprintf(f, "\nthread p[%d]: modificated atomic values\n", p_num);
    printf("\nthread p[%d]: modificated atomic values\n", p_num);
    while (1) {
        fprintf(f, "thread p[%d]: opens semaphore scr21 for the thread p[1]\n", p_num);
        printf("thread p[%d]: opens semaphore scr21 for the thread p[1]\n", p_num);
        sem_post(&scr21);
        fprintf(f, "\nthread p[%d]: semaphore scr21 is opened\n", p_num);
        printf("\nthread p[%d]: semaphore scr21 is opened\n", p_num);
        fprintf(f, "\nthread p[%d]: waits for the opening of the semaphore scr22\n", p_num);
        printf("\nthread p[%d]: waits for the opening of the semaphore scr22\n", p_num);
        trywait:
        if (sem_trywait(&scr22) == 0) {
            sem_close(&scr22);
            sem_open("scr22", O_CREAT, 0, 0);
            fprintf(f, "thread p[%d]: works after receiving semaphore scr22\n", p_num);
            printf("thread p[%d]: works after receiving semaphore scr22\n", p_num);
            pthread_mutex_lock(&mutex_for_sig21);
            pthread_cond_signal(&sig21);
            pthread_mutex_unlock(&mutex_for_sig21);
            fprintf(f, "thread p[%d]: sent sig21\n", p_num);
            printf("thread p[%d]: sent sig21\n", p_num);
            AtomicUsing();
            fprintf(f, "thread p[%d]: used atomic values\n", p_num);
            printf("thread p[%d]: used atomic values\n", p_num);
            AtomicModification();
            fprintf(f, "\nthread p[%d]: modificated atomic values\n", p_num);
            printf("\nthread p[%d]: modificated atomic values\n", p_num);
        }
        else {
            //printf("\nthread p[%d]: does some useful work\n", p_num);
            goto trywait;
        }
    }
    fprintf(f, "thread p[%d]: stopped\n", p_num);
    printf("thread p[%d]: stopped\n", p_num);
    return NULL;
}

void *P5thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    int flag = 0;
    int old_state = 0;
    while(1) {
        fprintf(f, "thread p[%d]: is waiting for sig21\n", p_num);
        printf("thread p[%d]: is waiting for sig21\n", p_num);
        pthread_mutex_lock(&mutex_for_sig21);
        pthread_cond_wait(&sig21, &mutex_for_sig21);
        pthread_mutex_unlock(&mutex_for_sig21);
        fprintf(f, "thread p[%d]: received sig21\n", p_num);
        printf("thread p[%d]: received sig21\n", p_num);
        AtomicUsing();
        fprintf(f, "thread p[%d]: used atomic values\n", p_num);
        printf("thread p[%d]: used atomic values\n", p_num);
        if (el_numb == 0) {
            fprintf(f, "thread p[%d]: bufer is empty\n", p_num);
            printf("thread p[%d]: bufer is empty\n", p_num);
            empty_bufer++;
            if (empty_bufer == 2)
                break;
        }
        if (pthread_mutex_trylock(&mcr1) == 0) {
            while(IsBufFull())
                pthread_cond_wait(&sig2_not_full, &mcr1);
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
            if (flag == 0)
                AddAtomicToBuf((int)int1_cr2);
            else if (flag == 1)
                AddAtomicToBuf((int)int2_cr2);
            else if (flag == 2)
                AddAtomicToBuf((int)uns1_cr2);
            else if (flag == 3)
                AddAtomicToBuf((int)uns2_cr2);
            else if (flag == 4)
                AddAtomicToBuf((int)long1_cr2);
            else if (flag == 5)
                AddAtomicToBuf((int)long2_cr2);
            else if (flag == 6)
                AddAtomicToBuf((int)luns1_cr2);
            else if (flag == 7)
                AddAtomicToBuf((int)luns2_cr2);
            pthread_setcancelstate(old_state, NULL);
            flag++;
            if (flag == 8)
                flag = 0;
            fprintf(f, "\nthread p[%d]: element %d ATOMIC CREATED; current el_numb = %d;\n", p_num, next_in->numb, el_numb);
            printf("\nthread p[%d]: element %d ATOMIC CREATED; current el_numb = %d;\n", p_num, next_in->numb, el_numb);
            pthread_mutex_unlock(&mcr1);
            pthread_cond_signal(&sig1_not_empty);
            fprintf(f, "thread p[%d]: sent sig1_not_empty\n", p_num);
            printf("thread p[%d]: sent sig1_not_empty\n", p_num);
        }
        else {
            //printf("\nthread p[%d]: does some useful work\n", p_num);
        }
    }
    pthread_cancel(p1);
    pthread_cancel(p2);
    pthread_cancel(p3);
    pthread_cancel(p4);
    pthread_cancel(p6);
    DeleteBuf();
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", p_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", p_num);
    return NULL;
}

void *P6thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    int old_state = 0;
    while(1) {
        if (el_numb == 0) {
            fprintf(f, "thread p[%d]: bufer is empty\n", p_num);
            printf("thread p[%d]: bufer is empty\n", p_num);
            empty_bufer++;
            if (empty_bufer == 2)
                break;
        }
        if (pthread_mutex_trylock(&mcr1) == 0) {
            while(IsBufFull()) {
                fprintf(f, "thread p[%d]: is waiting for sig2_not_full\n", p_num);
                printf("thread p[%d]: is waiting for sig2_not_full\n", p_num);
                pthread_cond_wait(&sig2_not_full, &mcr1);
            }
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
            AddElemToBuf();
            pthread_setcancelstate(old_state, NULL);
            fprintf(f, "\nthread p[%d]: element %d CREATED; current el_numb = %d;\n", p_num, next_in->numb, el_numb);
            printf("\nthread p[%d]: element %d CREATED; current el_numb = %d;\n", p_num, next_in->numb, el_numb);
            pthread_mutex_unlock(&mcr1);
            pthread_cond_signal(&sig1_not_empty);
            fprintf(f, "thread p[%d]: sent sig1_not_empty\n", p_num);
            printf("thread p[%d]: sent sig1_not_empty\n", p_num);
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
    DeleteBuf();
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", p_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", p_num);
	return NULL;
}

void AtomicInput() {
    int1_cr2 = (rand() % 6) - 3;
    int2_cr2 = (rand() % 6) - 3;
    uns1_cr2 = (rand() % 6) - 3;
    uns2_cr2 = (rand() % 6) - 3;
    long1_cr2 = (rand() % 6) - 3;
    long2_cr2 = (rand() % 6) - 3;
    luns1_cr2 = (rand() % 6) - 3;
    luns2_cr2 = (rand() % 6) - 3;
}

void AtomicOutput() {
    fprintf(f, "\n");
    fprintf(f, "cr2: (1) int = %d\n", int1_cr2);
    fprintf(f, "cr2: (2) int = %d\n", int2_cr2);
    fprintf(f, "cr2: (1) unsigned = %d\n", uns1_cr2);
    fprintf(f, "cr2: (2) unsigned = %d\n", uns2_cr2);
    fprintf(f, "cr2: (1) long = %ld\n", long1_cr2);
    fprintf(f, "cr2: (2) long = %ld\n", long2_cr2);
    fprintf(f, "cr2: (1) long unsigned = %lu\n", luns1_cr2);
    fprintf(f, "cr2: (2) long unsigned = %lu\n", luns2_cr2);
    fprintf(f, "\n");

    printf("\n");
    printf("cr2: (1) int = %d\n", int1_cr2);
    printf("cr2: (2) int = %d\n", int2_cr2);
    printf("cr2: (1) unsigned = %d\n", uns1_cr2);
    printf("cr2: (2) unsigned = %d\n", uns2_cr2);
    printf("cr2: (1) long = %ld\n", long1_cr2);
    printf("cr2: (2) long = %ld\n", long2_cr2);
    printf("cr2: (1) long unsigned = %lu\n", luns1_cr2);
    printf("cr2: (2) long unsigned = %lu\n", luns2_cr2);
    printf("\n");
}

void AtomicModification() {
    __sync_fetch_and_add(&int1_cr2, (rand() % 6) - 3);
    __sync_sub_and_fetch(&int2_cr2, (rand() % 6) - 3);
    __sync_fetch_and_xor(&uns1_cr2, (rand() % 6) - 3);
    __sync_fetch_and_nand(&uns2_cr2, (rand() % 6) - 3);
    __sync_or_and_fetch(&long1_cr2, (rand() % 6) - 3);
    __sync_and_and_fetch(&long2_cr2, (rand() % 6) - 3);
    __sync_bool_compare_and_swap (&luns1_cr2, (rand() % 6) - 3, (rand() % 6) - 3);
    __sync_val_compare_and_swap (&luns2_cr2, (rand() % 6) - 3, (rand() % 6) - 3);
}

void AtomicUsing() {
    __sync_fetch_and_add(&int1_cr2, 0);
    __sync_sub_and_fetch(&int2_cr2, 0);
    __sync_fetch_and_xor(&uns1_cr2, 0);
    __sync_fetch_and_nand(&uns2_cr2, uns2_cr2);
    __sync_fetch_and_nand(&uns2_cr2, uns2_cr2);
    __sync_or_and_fetch(&long1_cr2, 0);
    __sync_and_and_fetch(&long2_cr2, long2_cr2);
    __sync_bool_compare_and_swap (&luns1_cr2, 0, 0);
    __sync_val_compare_and_swap (&luns2_cr2, 0, 0);
}

void AddAtomicToBuf(int value) {
    if (new_buf < n) {
        struct cr1 *temp = (struct cr1 *)malloc(sizeof(struct cr1));
        temp->next = NULL;
        if (next_in == NULL) {
            temp->numb = value;
            next_in = temp;
            next_out = temp;
            begin_buf = temp;
            end_buf = temp;
            new_buf++;
            fprintf(f, "Cycle bufer was created\n");
            printf("Cycle bufer was created\n");
        }
        else {
            if (el_numb > n) {
                fprintf(f, "\nerror el_numb > %d\n", n);
                printf("\nerror el_numb > %d\n", n);
                free(temp);
                return;
            }
            temp->numb = value;
            next_in->next = temp;
            next_in = temp;
            new_buf++;
            if (new_buf == n)
                temp->next = begin_buf;
            end_buf = temp;
        }
        el_numb++;
    }
    else {
        next_in->next->numb = value;
        if (next_in == end_buf)
            next_in = begin_buf;
        else
            next_in = next_in->next;
        el_numb++;
    }
}
