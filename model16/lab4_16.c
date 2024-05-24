#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

FILE* f; //log file

struct elem //dsd queue
{
    struct elem *next;
    int number;
};

struct elem *beg_queue = NULL;
struct elem *end_queue = NULL;

pthread_cond_t sig1 = PTHREAD_COND_INITIALIZER; //signal not empty
pthread_cond_t sig2 = PTHREAD_COND_INITIALIZER; //signal not full
pthread_cond_t sig21 = PTHREAD_COND_INITIALIZER; //signal (broadcast)
pthread_cond_t sig22 = PTHREAD_COND_INITIALIZER; //signal (signal)

pthread_mutex_t mcr1 = PTHREAD_MUTEX_INITIALIZER; //mutex dsd
pthread_mutex_t mcr_sig21 = PTHREAD_MUTEX_INITIALIZER; //mutex for signal
pthread_mutex_t mcr_sig22 = PTHREAD_MUTEX_INITIALIZER; //mutex for signal

sem_t scr21; //semaphore scr21

pthread_t P1; //threads
pthread_t P2;
pthread_t P3;
pthread_t P4;
pthread_t P5;
pthread_t P6;

int max_size = 40; //dsd length
int curr_size = 0; //current dsd length
int full_queue = 0;
int empty_queue = 0;

int atom_int1 = 0; //atomic values cr2
int atom_int2 = 0;
unsigned atom_unsigned1 = 0;
unsigned atom_unsigned2 = 0;
long atom_long1 = 0;
long atom_long2 = 0;
long unsigned atom_long_unsigned1 = 0;
long unsigned atom_long_unsigned2 = 0;

int flag_sig21_P2 = 0; //flags for signals
int flag_sig21_P4 = 0;
int flag_sig22 = 0;

int is_full(); //dsd queue functions
int is_empty();
void add_elem(int atom_value_flag);
struct elem *get_elem();

void add_atom_elem(int atom_value_flag, struct elem *p);

void atom_modificate(); //atomic operations
void atom_use();

void *Pthread1(void *arg); //threads functions
void *Pthread2(void *arg);
void *Pthread3(void *arg);
void *Pthread4(void *arg);
void *Pthread5(void *arg);
void *Pthread6(void *arg);

int main()
{
    srand(time(NULL));
    f = fopen("laba4.log", "w");
    if (f == NULL)
    {
        printf("\n\nerror open file\n\n");
        return 1;
    }
    for(int i = 0; i < max_size / 2; i++)
    {
        add_elem(0);
        curr_size++;
    }
    fprintf(f, "Queue with elements from 0-th to %d-th has been created !!!\n\n", max_size / 2);
    printf("Queue with elements from 0-th to %d-th has been created !!!\n\n", max_size / 2);

    atom_int1 = rand() % 10;
    atom_int2 = rand() % 10;
    atom_unsigned1 = rand() % 10;
    atom_unsigned2 = rand() % 10;
    atom_long1 = rand() % 10;
    atom_long2 = rand() % 10;
    atom_long_unsigned1 = rand() % 10;
    atom_long_unsigned2 = rand() % 10;

    sem_init(&scr21, 0, 0);

    int thread_numb1 = 1;
    int thread_numb2 = 2;
    int thread_numb3 = 3;
    int thread_numb4 = 4;
    int thread_numb5 = 5;
    int thread_numb6 = 6;

    pthread_create(&P1, NULL, &Pthread1, (void*)&thread_numb1);
    pthread_create(&P2, NULL, &Pthread2, (void*)&thread_numb2);
    pthread_create(&P3, NULL, &Pthread3, (void*)&thread_numb3);
    pthread_create(&P4, NULL, &Pthread4, (void*)&thread_numb4);
    pthread_create(&P5, NULL, &Pthread5, (void*)&thread_numb5);
    pthread_create(&P6, NULL, &Pthread6, (void*)&thread_numb6);

    pthread_join(P1, NULL);
    pthread_join(P2, NULL);
    pthread_join(P3, NULL);
    pthread_join(P4, NULL);
    pthread_join(P5, NULL);
    pthread_join(P6, NULL);

    fprintf(f, "\nOperations with threads were written to the laba4 log file\n");
    printf("\nOperations with threads were written to the laba4 log file\n");
    fclose(f);
    return 0;
}

int is_empty()
{
    return curr_size <= 0;
}

int is_full()
{
    return curr_size >= max_size;
}

void add_elem(int atom_value_flag)
{
    struct elem *p = (struct elem*)malloc(sizeof(struct elem));
    p->next = NULL;
    if (beg_queue == NULL)
    {
        if (atom_value_flag == 0)
            p->number = 0;
        else
            add_atom_elem(atom_value_flag, p);
        beg_queue = p;
    }
    else
    {
        if (atom_value_flag == 0)
            p->number = end_queue->number + 1;
        else
            add_atom_elem(atom_value_flag, p);
        end_queue->next = p;
    }
    end_queue = p;
}

void add_atom_elem(int atom_value_flag, struct elem *p)
{
    if (atom_value_flag == 1)
        p->number = atom_int1;
    else if (atom_value_flag == 2)
        p->number = atom_int2;
    else if (atom_value_flag == 3)
        p->number = atom_unsigned1;
    else if (atom_value_flag == 4)
        p->number = atom_unsigned2;
    else if (atom_value_flag == 5)
        p->number = atom_long1;
    else if (atom_value_flag == 6)
        p->number = atom_long2;
    else if (atom_value_flag == 7)
        p->number = atom_long_unsigned1;
    else if (atom_value_flag == 8)
        p->number = atom_long_unsigned2;
}

struct elem *get_elem()
{
    struct elem *p = NULL;
    p = beg_queue;
    beg_queue = beg_queue->next;
    return p;
}

void atom_modificate()
{
    __sync_fetch_and_add(&atom_int1, rand() % 5);
    __sync_sub_and_fetch(&atom_int2, rand() % 5);
    __sync_fetch_and_and(&atom_unsigned1, rand() % 5);
    __sync_fetch_and_xor(&atom_unsigned2, rand() % 5);
    __sync_or_and_fetch(&atom_long1, rand() % 5);
    __sync_nand_and_fetch(&atom_long2, rand() % 5);
    __sync_bool_compare_and_swap(&atom_long_unsigned1, rand() % 5, rand() % 5);
    __sync_val_compare_and_swap(&atom_long_unsigned1, rand() % 5, rand() % 5);
}

void atom_use()
{
    __sync_fetch_and_add(&atom_int1, 0);
    __sync_sub_and_fetch(&atom_int2, 0);
    __sync_fetch_and_and(&atom_unsigned1, atom_unsigned1);
    __sync_fetch_and_xor(&atom_unsigned2, 0);
    __sync_or_and_fetch(&atom_long1, 0);
    __sync_nand_and_fetch(&atom_long2, atom_long2);
    __sync_nand_and_fetch(&atom_long2, atom_long2);
    __sync_bool_compare_and_swap(&atom_long_unsigned1, 0, 0);
    __sync_val_compare_and_swap(&atom_long_unsigned2, 0, 0);
}

void *Pthread1(void *p_number)
{
    int numb_p = *((int*)p_number);
    fprintf(f, "P[%d]: begins\n", numb_p);
    printf("P[%d]: begins\n", numb_p);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while(1)
    {
        atom_modificate();
        fprintf(f, "P[%d]: modificated all atomic variables\n", numb_p);
        printf("P[%d]: modificated all atomic variables\n", numb_p);
        sem_post(&scr21);
        fprintf(f, "P[%d]: opened semaphore scr21 for P4\n", numb_p);
        printf("P[%d]: opened semaphore scr21 for P4\n", numb_p);
        pthread_mutex_lock(&mcr_sig21);
        fprintf(f, "P[%d]: locked mcr_sig21\n", numb_p);
        printf("P[%d]: locked mcr_sig21\n", numb_p);
        pthread_cond_broadcast(&sig21);
        flag_sig21_P2 = 1;
        flag_sig21_P4 = 1;
        fprintf(f, "P[%d]: sent sig21\n", numb_p);
        printf("P[%d]: sent sig21\n", numb_p);
        fprintf(f, "P[%d]: unlocks mcr_sig21\n", numb_p);
        printf("P[%d]: unlocks mcr_sig21\n", numb_p);
        pthread_mutex_unlock(&mcr_sig21);
        atom_use();
        fprintf(f, "P[%d]: used all atomic variables\n", numb_p);
        printf("P[%d]: used all atomic variables\n", numb_p);
        atom_modificate();
        fprintf(f, "P[%d]: modificated all atomic variables\n", numb_p);
        printf("P[%d]: modificated all atomic variables\n", numb_p);
        usleep(10);
    }
    //fprintf(f, "P[%d]: was stopped and canceled other threads\n", numb_p);
    //printf("P[%d]: was stopped and canceled other threads\n", numb_p);
	return NULL;
}

void *Pthread2(void *p_number)
{
    int numb_p = *((int*)p_number);
    fprintf(f, "P[%d]: begins\n", numb_p);
    printf("P[%d]: begins\n", numb_p);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int old_state = 0;
    int atom_value_flag = 1;
    while(1)
    {
        pthread_mutex_lock(&mcr_sig21);
        fprintf(f, "P[%d]: locked mcr_sig21\n", numb_p);
        printf("P[%d]: locked mcr_sig21\n", numb_p);
        while(flag_sig21_P2 == 0)
        {
            fprintf(f, "P[%d]: is waiting sig21\n", numb_p);
            printf("P[%d]: is waiting sig21\n", numb_p);
            pthread_cond_wait(&sig21, &mcr_sig21);
        }
        flag_sig21_P2 = 0;
        fprintf(f, "P[%d]: received sig21\n", numb_p);
        printf("P[%d]: received sig21\n", numb_p);
        fprintf(f, "P[%d]: unlocks mcr_sig21\n", numb_p);
        printf("P[%d]: unlocks mcr_sig21\n", numb_p);
        pthread_mutex_unlock(&mcr_sig21);
        atom_use();
        fprintf(f, "P[%d]: used all atomic variables\n", numb_p);
        printf("P[%d]: used all atomic variables\n", numb_p);
        pthread_mutex_lock(&mcr1);
        fprintf(f, "P[%d]: locked mcr1\n", numb_p);
        printf("P[%d]: locked mcr1\n", numb_p);
        if (curr_size == 0) {
            fprintf(f, "P[%d]: queue is empty\n", numb_p);
            printf("P[%d]: queue is empty\n", numb_p);
            empty_queue++;
            if (empty_queue == 2)
                break;
        }
        while(is_full())
        {
            fprintf(f, "P[%d]: is waiting sig2\n", numb_p);
            printf("P[%d]: is waiting sig2\n", numb_p);
            pthread_cond_wait(&sig2, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
        add_elem(atom_value_flag);
        curr_size++;
        atom_value_flag++;
        fprintf(f, "P[%d]: element %d CREATED; current queue size = %d;\n", numb_p, end_queue->number, curr_size);
        printf("P[%d]: element %d CREATED; current queue size = %d;\n", numb_p, end_queue->number, curr_size);
        if (atom_value_flag == 9)
            atom_value_flag = 1;
        pthread_setcancelstate(old_state, NULL);
        fprintf(f, "P[%d]: unlocks mcr1\n", numb_p);
        printf("P[%d]: unlocks mcr1\n", numb_p);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_signal(&sig1);
        fprintf(f, "P[%d]: sent sig1\n", numb_p);
        printf("P[%d]: sent sig1\n", numb_p);
    }
    pthread_cancel(P1);
    pthread_cancel(P3);
    pthread_cancel(P4);
    pthread_cancel(P5);
    pthread_cancel(P6);
    pthread_mutex_unlock(&mcr1);
    fprintf(f, "P[%d]: was stopped and canceled other threads\n", numb_p);
    printf("P[%d]: was stopped and canceled other threads\n", numb_p);
	return NULL;
}

void *Pthread3(void *p_number)
{
    int numb_p = *((int*)p_number);
    fprintf(f, "P[%d]: begins\n", numb_p);
    printf("P[%d]: begins\n", numb_p);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int old_state = 0;
    struct elem *temp = 0;
    while(1)
    {
        pthread_mutex_lock(&mcr1);
        fprintf(f, "P[%d]: locked mcr1\n", numb_p);
        printf("P[%d]: locked mcr1\n", numb_p);
        if (curr_size == max_size) {
            fprintf(f, "P[%d]: queue is full\n", numb_p);
            printf("P[%d]: queue is full\n", numb_p);
            full_queue++;
            if (full_queue == 2)
                break;
        }
        while(is_empty())
        {
            fprintf(f, "P[%d]: is waiting sig1\n", numb_p);
            printf("P[%d]: is waiting sig1\n", numb_p);
            pthread_cond_wait(&sig1, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
        temp = get_elem();
        curr_size--;
        fprintf(f, "P[%d]: element %d TAKEN; current queue size = %d;\n", numb_p, temp->number, curr_size);
        printf("P[%d]: element %d TAKEN; current queue size = %d;\n", numb_p, temp->number, curr_size);
        pthread_setcancelstate(old_state, NULL);
        fprintf(f, "P[%d]: unlocks mcr1\n", numb_p);
        printf("P[%d]: unlocks mcr1\n", numb_p);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_broadcast(&sig2);
        fprintf(f, "P[%d]: sent sig2\n", numb_p);
        printf("P[%d]: sent sig2\n", numb_p);
        free(temp);
        usleep(1);
    }
    pthread_cancel(P1);
    pthread_cancel(P2);
    pthread_cancel(P4);
    pthread_cancel(P5);
    pthread_cancel(P6);
    pthread_mutex_unlock(&mcr1);
    fprintf(f, "P[%d]: was stopped and canceled other threads\n", numb_p);
    printf("P[%d]: was stopped and canceled other threads\n", numb_p);
	return NULL;
}

void *Pthread4(void *p_number)
{
    int numb_p = *((int*)p_number);
    fprintf(f, "P[%d]: begins\n", numb_p);
    printf("P[%d]: begins\n", numb_p);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while(1)
    {
        pthread_mutex_lock(&mcr_sig21);
        fprintf(f, "P[%d]: locked mcr_sig21\n", numb_p);
        printf("P[%d]: locked mcr_sig21\n", numb_p);
        while(flag_sig21_P4 == 0)
        {
            fprintf(f, "P[%d]: is waiting sig21\n", numb_p);
            printf("P[%d]: is waiting sig21\n", numb_p);
            pthread_cond_wait(&sig21, &mcr_sig21);
        }
        flag_sig21_P4 = 0;
        fprintf(f, "P[%d]: received sig21\n", numb_p);
        printf("P[%d]: received sig21\n", numb_p);
        fprintf(f, "P[%d]: unlocks mcr_sig21\n", numb_p);
        printf("P[%d]: unlocks mcr_sig21\n", numb_p);
        pthread_mutex_unlock(&mcr_sig21);
        atom_modificate();
        fprintf(f, "P[%d]: modificated all atomic variables\n", numb_p);
        printf("P[%d]: modificated all atomic variables\n", numb_p);
        pthread_mutex_lock(&mcr_sig22);
        fprintf(f, "P[%d]: locked mcr_sig22\n", numb_p);
        printf("P[%d]: locked mcr_sig22\n", numb_p);
        pthread_cond_signal(&sig22);
        flag_sig22 = 1;
        fprintf(f, "P[%d]: sent sig22\n", numb_p);
        printf("P[%d]: sent sig22\n", numb_p);
        fprintf(f, "P[%d]: unlocks mcr_sig22\n", numb_p);
        printf("P[%d]: unlocks mcr_sig22\n", numb_p);
        pthread_mutex_unlock(&mcr_sig22);
        fprintf(f, "P[%d]: waits for the opening of the semaphore scr21 from P1\n", numb_p);
        printf("P[%d]: waits for the opening of the semaphore scr21 from P1\n", numb_p);
        sem_wait(&scr21);
        fprintf(f, "P[%d]: semaphore scr21 was opened\n", numb_p);
        printf("P[%d]: semaphore scr21 was opened\n", numb_p);
        sem_close(&scr21);
        sem_init(&scr21, 0, 0);
    }
    //fprintf(f, "P[%d]: was stopped and canceled other threads\n", numb_p);
    //printf("P[%d]: was stopped and canceled other threads\n", numb_p);
	return NULL;
}

void *Pthread5(void *p_number)
{
    int numb_p = *((int*)p_number);
    fprintf(f, "P[%d]: begins\n", numb_p);
    printf("P[%d]: begins\n", numb_p);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int old_state = 0;
    int atom_value_flag = 1;
    while(1)
    {
        pthread_mutex_lock(&mcr_sig22);
        fprintf(f, "P[%d]: locked mcr_sig22\n", numb_p);
        printf("P[%d]: locked mcr_sig22\n", numb_p);
        while(flag_sig22 == 0)
        {
            fprintf(f, "P[%d]: is waiting sig22\n", numb_p);
            printf("P[%d]: is waiting sig22\n", numb_p);
            pthread_cond_wait(&sig22, &mcr_sig22);
        }
        flag_sig22 = 0;
        fprintf(f, "P[%d]: received sig22\n", numb_p);
        printf("P[%d]: received sig22\n", numb_p);
        fprintf(f, "P[%d]: unlocks mcr_sig22\n", numb_p);
        printf("P[%d]: unlocks mcr_sig22\n", numb_p);
        pthread_mutex_unlock(&mcr_sig22);
        atom_use();
        fprintf(f, "P[%d]: used all atomic variables\n", numb_p);
        printf("P[%d]: used all atomic variables\n", numb_p);
        pthread_mutex_lock(&mcr1);
        fprintf(f, "P[%d]: locked mcr1\n", numb_p);
        printf("P[%d]: locked mcr1\n", numb_p);
        if (curr_size == 0) {
            fprintf(f, "P[%d]: queue is empty\n", numb_p);
            printf("P[%d]: queue is empty\n", numb_p);
            empty_queue++;
            if (empty_queue == 2)
                break;
        }
        while(is_full())
        {
            fprintf(f, "P[%d]: is waiting sig2\n", numb_p);
            printf("P[%d]: is waiting sig2\n", numb_p);
            pthread_cond_wait(&sig2, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
        add_elem(atom_value_flag);
        curr_size++;
        atom_value_flag++;
        fprintf(f, "P[%d]: element %d CREATED; current queue size = %d;\n", numb_p, end_queue->number, curr_size);
        printf("P[%d]: element %d CREATED; current queue size = %d;\n", numb_p, end_queue->number, curr_size);
        if (atom_value_flag == 9)
            atom_value_flag = 1;
        pthread_setcancelstate(old_state, NULL);
        fprintf(f, "P[%d]: unlocks mcr1\n", numb_p);
        printf("P[%d]: unlocks mcr1\n", numb_p);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_signal(&sig1);
        fprintf(f, "P[%d]: sent sig1\n", numb_p);
        printf("P[%d]: sent sig1\n", numb_p);
    }
    pthread_cancel(P1);
    pthread_cancel(P2);
    pthread_cancel(P3);
    pthread_cancel(P4);
    pthread_cancel(P6);
    pthread_mutex_unlock(&mcr1);
    fprintf(f, "P[%d]: was stopped and canceled other threads\n", numb_p);
    printf("P[%d]: was stopped and canceled other threads\n", numb_p);
	return NULL;
}

void *Pthread6(void *p_number)
{
    int numb_p = *((int*)p_number);
    fprintf(f, "P[%d]: begins\n", numb_p);
    printf("P[%d]: begins\n", numb_p);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int old_state = 0;
    while(1)
    {
        pthread_mutex_lock(&mcr1);
        fprintf(f, "P[%d]: locked mcr1\n", numb_p);
        printf("P[%d]: locked mcr1\n", numb_p);
        if (curr_size == 0) {
            fprintf(f, "P[%d]: queue is empty\n", numb_p);
            printf("P[%d]: queue is empty\n", numb_p);
            empty_queue++;
            if (empty_queue == 2)
                break;
        }
        while(is_full())
        {
            fprintf(f, "P[%d]: is waiting sig2\n", numb_p);
            printf("P[%d]: is waiting sig2\n", numb_p);
            pthread_cond_wait(&sig2, &mcr1);
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
        add_elem(0);
        curr_size++;
        fprintf(f, "P[%d]: element %d CREATED; current queue size = %d;\n", numb_p, end_queue->number, curr_size);
        printf("P[%d]: element %d CREATED; current queue size = %d;\n", numb_p, end_queue->number, curr_size);
        pthread_setcancelstate(old_state, NULL);
        fprintf(f, "P[%d]: unlocks mcr1\n", numb_p);
        printf("P[%d]: unlocks mcr1\n", numb_p);
        pthread_mutex_unlock(&mcr1);
        pthread_cond_signal(&sig1);
        fprintf(f, "P[%d]: sent sig1\n", numb_p);
        printf("P[%d]: sent sig1\n", numb_p);
        usleep(1);
    }
    pthread_cancel(P1);
    pthread_cancel(P2);
    pthread_cancel(P3);
    pthread_cancel(P4);
    pthread_cancel(P5);
    pthread_mutex_unlock(&mcr1);
    fprintf(f, "P[%d]: was stopped and canceled other threads\n", numb_p);
    printf("P[%d]: was stopped and canceled other threads\n", numb_p);
	return NULL;
}
