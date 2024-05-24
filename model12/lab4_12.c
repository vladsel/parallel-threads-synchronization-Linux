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
pthread_cond_t sig1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t sig2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t sig21 = PTHREAD_COND_INITIALIZER;

//file
FILE *f = NULL;

//dsd bufer values
struct cr1 {
    struct cr1 *next;
    int numb;
};

struct cr1 *head = NULL;

unsigned int max_length = 64;
unsigned int cur_length = 0;

//atomic values
int cr2_int1 = 0;
int cr2_int2 = 0;
unsigned cr2_uns1 = 0;
unsigned cr2_uns2 = 0;
long cr2_long1 = 0;
long cr2_long2 = 0;
long unsigned cr2_luns1 = 0;
long unsigned cr2_luns2 = 0;

//control bufer
int empty_bufer = 0;
int full_bufer = 0;

//bufer function declarations
void DeleteBuf();
//void OutputBuf();
int BufEmpty();
int BufFull();
void AddElem();
struct cr1 *GetElem();

//atomic function declarations
void AtomicInput();
void AtomicModification();
void AtomicUsing();

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
    for(unsigned int i = 0; i < max_length / 2; i++) {
        AddElem();
        cur_length++;
    }

    fprintf(f, "Cycle buffer with elements from 0-th to %d-th has been created !!!\n", (max_length / 2));
    printf("Cycle buffer with elements from 0-th to %d-th has been created !!!\n", (max_length / 2));
    AtomicInput();

    int p1_numb = 1;
    int p2_numb = 2;
    int p3_numb = 3;
    int p4_numb = 4;
    int p5_numb = 5;
    int p6_numb = 6;

    int p1_join = -1;
    int p2_join = -1;
    int p3_join = -1;
    int p4_join = -1;
    int p5_join = -1;
    int p6_join = -1;

    sem_init(&scr21, 0, 0);
    sem_init(&scr22, 0, 0);

    pthread_create(&p1, NULL, &P1thread, (void*)&p1_numb);
    pthread_create(&p2, NULL, &P2thread, (void*)&p2_numb);
    pthread_create(&p3, NULL, &P3thread, (void*)&p3_numb);
    pthread_create(&p4, NULL, &P4thread, (void*)&p4_numb);
    pthread_create(&p5, NULL, &P5thread, (void*)&p5_numb);
    pthread_create(&p6, NULL, &P6thread, (void*)&p6_numb);

    p1_join = pthread_join(p1, NULL);
    fprintf(f, "\npthread_join p%d = %d", p1_numb, p1_join);
    printf("\npthread_join p%d = %d", p1_numb, p1_join);
    p2_join = pthread_join(p2, NULL);
    fprintf(f, "\npthread_join p%d = %d", p2_numb, p2_join);
    printf("\npthread_join p%d = %d", p2_numb, p2_join);
    p3_join = pthread_join(p3, NULL);
    fprintf(f, "\npthread_join p%d = %d", p3_numb, p3_join);
    printf("\npthread_join p%d = %d", p3_numb, p3_join);
    p4_join = pthread_join(p4, NULL);
    fprintf(f, "\npthread_join p%d = %d", p4_numb, p4_join);
    printf("\npthread_join p%d = %d", p4_numb, p4_join);
    p5_join = pthread_join(p5, NULL);
    fprintf(f, "\npthread_join p%d = %d", p5_numb, p5_join);
    printf("\npthread_join p%d = %d", p5_numb, p5_join);
    p6_join = pthread_join(p6, NULL);
    fprintf(f, "\npthread_join p%d = %d\n", p6_numb, p6_join);
    printf("\npthread_join p%d = %d\n", p6_numb, p6_join);

    fprintf(f, "\nAll information was written to the log file\n");
    printf("\nAll information was written to the log file\n");
    fclose(f);
    return 0;
}

void DeleteBuf() {
    struct cr1 *temp = head;
    if (temp == NULL)
        return;
    while(head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
    head = NULL;
    fprintf(f, "\nStack buffer was deleted\n\n");
    printf("\nStack buffer was deleted\n\n");
}

int BufEmpty() {
    return cur_length <= 0;
}

int BufFull() {
    return cur_length >= max_length;
}

void AddElem() {
    struct cr1 *temp = malloc(sizeof(struct cr1));
    if (temp == NULL)
        return;
    if (head == NULL) {
        fprintf(f, "Stack bufer was created\n");
        printf("Stack bufer was created\n");
    }
    temp->next = head;
    temp->numb = cur_length;
    head = temp;
}

struct cr1 *GetElem() {
    struct cr1 *temp = NULL;
    if (head == NULL) {
        fprintf(f, "Stack bufer is empty\n");
        printf("Stack bufer is empty\n");
        return NULL;
    }
    temp = head;
    head = head->next;
    return temp;
}

/*void OutputBuf() {
    struct cr1 *temp = head;
    if (temp == NULL)
        return;
    while (temp != NULL) {
        fprintf(f, "%d ", temp->numb);
        printf("%d ", temp->numb);
        temp = temp->next;
    }
}*/

void *P1thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    int old_state = 0;
    while(1) {
        if (cur_length == 0) {
            fprintf(f, "thread p[%d]: stack bufer is empty\n", p_num);
            printf("thread p[%d]: stack bufer is empty\n", p_num);
            empty_bufer++;
            if (empty_bufer == 2)
                break;
        }
        pthread_mutex_lock(&mcr1);
        while(BufFull()) {
            fprintf(f, "thread p[%d]: is waiting for sig2\n", p_num);
            printf("thread p[%d]: is waiting for sig2\n", p_num);
            pthread_cond_wait(&sig2, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
        AddElem();
        cur_length++;
        pthread_setcancelstate(old_state, NULL);
        fprintf(f, "\nthread p[%d]: element %d CREATED; current cur_length = %d;\n", p_num, head->numb, cur_length);
        printf("\nthread p[%d]: element %d CREATED; current cur_length = %d;\n", p_num, head->numb, cur_length);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_broadcast(&sig1);
        fprintf(f, "thread p[%d]: sent sig1\n", p_num);
        printf("thread p[%d]: sent sig1\n", p_num);
    }
    pthread_cancel(p2);
    pthread_cancel(p3);
    pthread_cancel(p4);
    pthread_cancel(p5);
    pthread_cancel(p6);
    DeleteBuf();
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", p_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", p_num);
	return NULL;
}

void *P2thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    int old_state = 0;
    while(1) {
        if (cur_length == 0) {
            fprintf(f, "thread p[%d]: stack bufer is empty\n", p_num);
            printf("thread p[%d]: stack bufer is empty\n", p_num);
            empty_bufer++;
            if (empty_bufer == 2)
                break;
        }
        pthread_mutex_lock(&mcr1);
        while(BufFull()) {
            fprintf(f, "thread p[%d]: is waiting for sig2\n", p_num);
            printf("thread p[%d]: is waiting for sig2\n", p_num);
            pthread_cond_wait(&sig2, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
        AddElem();
        cur_length++;
        pthread_setcancelstate(old_state, NULL);
        fprintf(f, "\nthread p[%d]: element %d CREATED; current cur_length = %d;\n", p_num, head->numb, cur_length);
        printf("\nthread p[%d]: element %d CREATED; current cur_length = %d;\n", p_num, head->numb, cur_length);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_broadcast(&sig1);
        fprintf(f, "thread p[%d]: sent sig1\n", p_num);
        printf("thread p[%d]: sent sig1\n", p_num);
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
        AtomicModification();
        fprintf(f, "\nthread p[%d]: modificated atomic values\n", p_num);
        printf("\nthread p[%d]: modificated atomic values\n", p_num);
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
    while (1) {
        AtomicModification();
        fprintf(f, "\nthread p[%d]: modificated atomic values\n", p_num);
        printf("\nthread p[%d]: modificated atomic values\n", p_num);
        fprintf(f, "thread p[%d]: opens semaphore scr22 for the thread p[6]\n", p_num);
        printf("thread p[%d]: opens semaphore scr22 for the thread p[6]\n", p_num);
        sem_post(&scr22);
        fprintf(f, "\nthread p[%d]: semaphore scr22 is opened\n", p_num);
        printf("\nthread p[%d]: semaphore scr22 is opened\n", p_num);
        fprintf(f, "\nthread p[%d]: waits for the opening of the semaphore scr21\n", p_num);
        printf("\nthread p[%d]: waits for the opening of the semaphore scr21\n", p_num);
        wait:
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
        }
        else
            goto wait;
    }
    fprintf(f, "thread p[%d]: stopped\n", p_num);
    printf("thread p[%d]: stopped\n", p_num);
    return NULL;
}

void *P4thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    struct cr1 *temp = NULL;
    int old_state = 0;
    while(1) {
        temp = NULL;
        if (cur_length == max_length) {
            fprintf(f, "thread p[%d]: stack bufer is full\n", p_num);
            printf("thread p[%d]: stack bufer is full\n", p_num);
            full_bufer++;
            if (full_bufer == 2)
                break;
        }
        pthread_mutex_lock(&mcr1);
        while(BufEmpty()) {
            fprintf(f, "thread p[%d]: is waiting for sig2\n", p_num);
            printf("thread p[%d]: is waiting for sig2\n", p_num);
            pthread_cond_wait(&sig1, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
        temp = GetElem();
        cur_length--;
        pthread_setcancelstate(old_state, NULL);
        fprintf(f, "\nthread p[%d]: element %d TAKEN; current cur_length = %d;\n", p_num, temp->numb, cur_length);
        printf("\nthread p[%d]: element %d TAKEN; current cur_length = %d;\n", p_num, temp->numb, cur_length);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_broadcast(&sig2);
        free(temp);
        fprintf(f, "thread p[%d]: sent sig2\n", p_num);
        printf("thread p[%d]: sent sig2\n", p_num);
    }
    pthread_cancel(p1);
    pthread_cancel(p2);
    pthread_cancel(p3);
    pthread_cancel(p5);
    pthread_cancel(p6);
    fprintf(f, "thread p[%d]: stopped other threads and was stopped too\n", p_num);
    printf("thread p[%d]: stopped other threads and was stopped too\n", p_num);
	return NULL;
}

void *P5thread(void *p_number) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    struct cr1 *temp = NULL;
    int old_state = 0;
    while(1) {
        temp = NULL;
        if (cur_length == max_length) {
            fprintf(f, "thread p[%d]: stack buffer is full\n", p_num);
            printf("thread p[%d]: stack bufer is full\n", p_num);
            full_bufer++;
            if (full_bufer == 2)
                break;
        }
        pthread_mutex_lock(&mcr1);
        while(BufEmpty()) {
            fprintf(f, "thread p[%d]: is waiting for sig2\n", p_num);
            printf("thread p[%d]: is waiting for sig2\n", p_num);
            pthread_cond_wait(&sig1, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
        temp = GetElem();
        cur_length--;
        pthread_setcancelstate(old_state, NULL);
        fprintf(f, "\nthread p[%d]: element %d TAKEN; current cur_length = %d;\n", p_num, temp->numb, cur_length);
        printf("\nthread p[%d]: element %d TAKEN; current cur_length = %d;\n", p_num, temp->numb, cur_length);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_broadcast(&sig2);
        free(temp);
        fprintf(f, "thread p[%d]: sent sig2\n", p_num);
        printf("thread p[%d]: sent sig2\n", p_num);
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
    AtomicModification();
    fprintf(f, "\nthread p[%d]: modificated atomic values\n", p_num);
    printf("\nthread p[%d]: modificated atomic values\n", p_num);
    while (1) {
        fprintf(f, "thread p[%d]: opens semaphore scr21 for the thread p[3]\n", p_num);
        printf("thread p[%d]: opens semaphore scr21 for the thread p[3]\n", p_num);
        sem_post(&scr21);
        fprintf(f, "\nthread p[%d]: semaphore scr21 is opened\n", p_num);
        printf("\nthread p[%d]: semaphore scr21 is opened\n", p_num);
        fprintf(f, "\nthread p[%d]: waits for the opening of the semaphore scr22\n", p_num);
        printf("\nthread p[%d]: waits for the opening of the semaphore scr22\n", p_num);
        sem_wait(&scr22);
        sem_close(&scr22);
        sem_open("scr22", O_CREAT, 0, 0);
        fprintf(f, "thread p[%d]: works after receiving semaphore scr22\n", p_num);
        printf("thread p[%d]: works after receiving semaphore scr22\n", p_num);
        pthread_mutex_lock(&mutex_for_sig21);
        pthread_cond_broadcast(&sig21);
        pthread_mutex_unlock(&mutex_for_sig21);
        fprintf(f, "thread p[%d]: sent sig21\n", p_num);
        printf("thread p[%d]: sent sig21\n", p_num);
        AtomicUsing();
        fprintf(f, "thread p[%d]: used atomic values\n", p_num);
        printf("thread p[%d]: used atomic values\n", p_num);
    }
    fprintf(f, "thread p[%d]: stopped\n", p_num);
    printf("thread p[%d]: stopped\n", p_num);
    return NULL;
}

void AtomicInput() {
    cr2_int1 = (rand() % 6) - 3;
    cr2_int2 = (rand() % 6) - 3;
    cr2_uns1 = (rand() % 6) - 3;
    cr2_uns2 = (rand() % 6) - 3;
    cr2_long1 = (rand() % 6) - 3;
    cr2_long2 = (rand() % 6) - 3;
    cr2_luns1 = (rand() % 6) - 3;
    cr2_luns2 = (rand() % 6) - 3;
}

void AtomicModification() {
    __sync_fetch_and_add(&cr2_int1, (rand() % 6) - 3);
    __sync_add_and_fetch(&cr2_int2, (rand() % 6) - 3);
    __sync_fetch_and_xor(&cr2_uns1, (rand() % 6) - 3);
    __sync_fetch_and_nand(&cr2_uns2, (rand() % 6) - 3);
    __sync_or_and_fetch(&cr2_long1, (rand() % 6) - 3);
    __sync_and_and_fetch(&cr2_long2, (rand() % 6) - 3);
    __sync_bool_compare_and_swap (&cr2_luns1, (rand() % 6) - 3, (rand() % 6) - 3);
    __sync_val_compare_and_swap (&cr2_luns2, (rand() % 6) - 3, (rand() % 6) - 3);
}

void AtomicUsing() {
    __sync_fetch_and_add(&cr2_int1, 0);
    __sync_add_and_fetch(&cr2_int2, 0);
    __sync_fetch_and_xor(&cr2_uns1, 0);
    __sync_fetch_and_nand(&cr2_uns2, cr2_uns2);
    __sync_fetch_and_nand(&cr2_uns2, cr2_uns2);
    __sync_or_and_fetch(&cr2_long1, 0);
    __sync_and_and_fetch(&cr2_long2, cr2_long2);
    __sync_bool_compare_and_swap (&cr2_luns1, 0, 0);
    __sync_val_compare_and_swap (&cr2_luns2, 0, 0);
}
