#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define size_stack 20

//semaphore
sem_t scr1;

//barrier
pthread_barrier_t bcr2;

//mutex init
pthread_mutex_t mcr1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_sig21 = PTHREAD_MUTEX_INITIALIZER;

//conditional signal
pthread_cond_t sig21 = PTHREAD_COND_INITIALIZER;

//flags for signals
int flag_sig21_P2 = 0;
int flag_sig21_P5 = 0;
int flag_sig21 = 0;

//stack values
int idx_stack = 0;
int stack[size_stack];

//control bufer
int empty_stack = 0;
int full_stack = 0;

//atomic values
int int_first_cr2 = 0;
int int_second_cr2 = 0;
unsigned uns_first_cr2 = 0;
unsigned uns_second_cr2 = 0;
long long_first_cr2 = 0;
long long_second_cr2 = 0;
long unsigned long_uns_first_cr2 = 0;
long unsigned long_uns_second_cr2 = 0;

//log file
FILE *f = NULL;

//threads
pthread_t p1;
pthread_t p2;
pthread_t p3;
pthread_t p4;
pthread_t p5;
pthread_t p6;

//print to the console and to the file
void print(char *string, int p_num);

//atomic declarations
void AtomicModification();
void AtomicUsing();

//bufer declarations
void AddElemStack();
int GetElemStack();

//threads declarations
void *P1thread(void *p_number);
void *P2thread(void *p_number);
void *P3thread(void *p_number);
void *P4thread(void *p_number);
void *P5thread(void *p_number);
void *P6thread(void *p_number);

int main()
{
    srand(time(NULL));
    f = fopen("report.log", "w");
    if (f == NULL)
    {
        printf("\nfile broken\n");
        return(1);
    }

    sem_init(&scr1, 0, 0); //initialize semaphore
    pthread_barrier_init(&bcr2, NULL, 2); //initialize barrier

    for(int i = 0; i < size_stack / 2; i++)
    {
        AddElemStack();
        sem_post(&scr1);
    }
    int sem_value = 0;
    sem_getvalue(&scr1, &sem_value);
    fprintf(f, "Stack buffer with elements from 0-th to %d-th has been created !!!\n", (size_stack / 2));
    printf("Stack buffer with elements from 0-th to %d-th has been created !!!\n", (size_stack / 2));
    fprintf(f, "semaphore = %d\n\n", sem_value);
    printf("semaphore = %d\n\n", sem_value);

    int_first_cr2 = (rand() % 8) - 4;
    int_second_cr2 = (rand() % 8) - 4;
    uns_first_cr2 = (rand() % 8) - 4;
    uns_second_cr2 = (rand() % 8) - 4;
    long_first_cr2 = (rand() % 8) - 4;
    long_second_cr2 = (rand() % 8) - 4;
    long_uns_first_cr2 = (rand() % 8) - 4;
    long_uns_second_cr2 = (rand() % 8) - 4;

    int p1_numb = 1;
    int p2_numb = 2;
    int p3_numb = 3;
    int p4_numb = 4;
    int p5_numb = 5;
    int p6_numb = 6;

    pthread_create(&p1, NULL, &P1thread, (void*)&p1_numb);
    pthread_create(&p2, NULL, &P2thread, (void*)&p2_numb);
    pthread_create(&p3, NULL, &P3thread, (void*)&p3_numb);
    pthread_create(&p4, NULL, &P4thread, (void*)&p4_numb);
    pthread_create(&p5, NULL, &P5thread, (void*)&p5_numb);
    pthread_create(&p6, NULL, &P6thread, (void*)&p6_numb);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    pthread_join(p4, NULL);
    pthread_join(p5, NULL);
    pthread_join(p6, NULL);

    pthread_barrier_destroy(&bcr2); //destroy barrier
    print("\nAll actions of threads were written to the report file\n", 0);
    fclose(f);
    return 0;
}

void print(char *str, int p_num)
{
    if (p_num == 0)
    {
        fprintf(f, "%s", str);
        printf("%s", str);
    }
    else if (p_num > 0)
    {
        fprintf(f, "Thread P[%d]: %s", p_num, str);
        printf("Thread P[%d]: %s", p_num, str);
    }
}

void AddElemStack()
{
    stack[idx_stack] = idx_stack;
    idx_stack++;
    //stack[idx_stack] = rand() % 100;
}

int GetElemStack()
{
    idx_stack--;
    return stack[idx_stack];
}

AtomicModification()
{
    __sync_fetch_and_sub(&int_first_cr2, (rand() % 8) - 4);
    __sync_sub_and_fetch(&int_second_cr2, (rand() % 8) - 4);
    __sync_fetch_and_or(&uns_first_cr2, (rand() % 8) - 4);
    __sync_fetch_and_and(&uns_second_cr2, (rand() % 8) - 4);
    __sync_xor_and_fetch(&long_first_cr2, (rand() % 8) - 4);
    __sync_nand_and_fetch(&long_second_cr2, (rand() % 8) - 4);
    __sync_bool_compare_and_swap(&long_uns_first_cr2, (rand() % 8) - 4, (rand() % 8) - 4);
    __sync_val_compare_and_swap(&long_uns_second_cr2, (rand() % 8) - 4, (rand() % 8) - 4);
}

AtomicUsing()
{
    __sync_fetch_and_sub(&int_first_cr2, 0);
    __sync_sub_and_fetch(&int_second_cr2, 0);
    __sync_fetch_and_or(&uns_first_cr2, 0);
    __sync_fetch_and_and(&uns_second_cr2, uns_second_cr2);
    __sync_xor_and_fetch(&long_first_cr2, 0);
    __sync_nand_and_fetch(&long_second_cr2, long_second_cr2);
    __sync_nand_and_fetch(&long_second_cr2, long_second_cr2);
    __sync_bool_compare_and_swap(&long_uns_first_cr2, 0, 0);
    __sync_val_compare_and_swap(&long_uns_second_cr2, 0, 0);
}

void *P1thread(void *p_number)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    print("begins\n", p_num);
    int prev_state = 0;
    int sem_value = 0;
    while(1)
    {
        sem_getvalue(&scr1, &sem_value);
        if (sem_value < size_stack)
        {
            if (pthread_mutex_trylock(&mcr1) == 0)
            {
                print("locked mcr1\n", p_num);
                if (sem_value == 0)
                {
                    print("stack is empty\n", p_num);
                    empty_stack++;
                    if (empty_stack == 2)
                        break;
                }
                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &prev_state);
                AddElemStack();
                sem_post(&scr1);
                pthread_setcancelstate(prev_state, NULL);
                print("increased semaphore scr1\n", p_num);
                sem_getvalue(&scr1, &sem_value);
                fprintf(f, "Thread P[%d]: element %d CREATED; current semaphore = %d;\n", p_num, stack[idx_stack - 1], sem_value);
                printf("Thread P[%d]: element %d CREATED; current semaphore = %d;\n", p_num, stack[idx_stack - 1], sem_value);
                print("unlocked mcr1\n", p_num);
                pthread_mutex_unlock(&mcr1);
                usleep(1);
            }
            else
            {
                usleep(20);
            }
        }
    }
    pthread_cancel(p2);
    pthread_cancel(p3);
    pthread_cancel(p4);
    pthread_cancel(p5);
    pthread_cancel(p6);
    print("stopped all threads and was stopped too\n", p_num);
    return NULL;
}

void *P2thread(void *p_number)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    print("begins\n", p_num);
    int prev_state = 0;
    int sem_value = 0;
    while(1)
    {
        sem_getvalue(&scr1, &sem_value);
        if (sem_value < size_stack)
        {
            if (pthread_mutex_trylock(&mcr1) == 0)
            {
                print("locked mcr1\n", p_num);
                if (sem_value == 0)
                {
                    print("stack is empty\n", p_num);
                    empty_stack++;
                    if (empty_stack == 2)
                        break;
                }
                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &prev_state);
                AddElemStack();
                sem_post(&scr1);
                pthread_setcancelstate(prev_state, NULL);
                print("increased semaphore scr1\n", p_num);
                sem_getvalue(&scr1, &sem_value);
                fprintf(f, "Thread P[%d]: element %d CREATED; current semaphore = %d;\n", p_num, stack[idx_stack - 1], sem_value);
                printf("Thread P[%d]: element %d CREATED; current semaphore = %d;\n", p_num, stack[idx_stack - 1], sem_value);
                print("unlocked mcr1\n", p_num);
                pthread_mutex_unlock(&mcr1);
            }
            else
            {
            //
            }
        }
        pthread_mutex_lock(&mut_sig21);
        while (flag_sig21_P2 == 0 || flag_sig21 == 0)
        {
            print("is waiting sig21\n", p_num);
            pthread_cond_wait(&sig21, &mut_sig21);
        }
        flag_sig21_P2 = 0;
        flag_sig21 = 0;
        print("already received sig21\n", p_num);
        pthread_mutex_unlock(&mut_sig21);
        AtomicUsing();
        print("used atomic values\n", p_num);
        AtomicModification();
        print("modificated atomic values\n", p_num);
    }
    pthread_cancel(p1);
    pthread_cancel(p3);
    pthread_cancel(p4);
    pthread_cancel(p5);
    pthread_cancel(p6);
    print("stopped all threads and was stopped too\n", p_num);
    return NULL;
}

void *P3thread(void *p_number)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    print("begins\n", p_num);
    while (1)
    {
        AtomicModification();
        print("modificated atomic values\n", p_num);
        print("waits for the thread P[6] at the barrier bcr2\n", p_num);
        pthread_barrier_wait(&bcr2);
        print("works after barrier bcr2\n", p_num);
        pthread_mutex_lock(&mut_sig21);
        pthread_cond_signal(&sig21);
        flag_sig21 = 1;
        print("sent sig21 for thread P[2]\n", p_num);
        pthread_mutex_unlock(&mut_sig21);
        AtomicUsing();
        print("used atomic values\n", p_num);
        usleep(20);
    }
    print("stopped all threads and was stopped too\n", p_num);
    return NULL;
}

void *P4thread(void *p_number)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    print("begins\n", p_num);
    int prev_state = 0;
    int sem_value = 0;
    int temp = 0;
    while(1)
    {
        //print("is waiting the semaphore scr1\n", p_num);
        sem_wait(&scr1);
        if (pthread_mutex_trylock(&mcr1) == 0)
        {
            print("already received and reduced semaphore scr1\n", p_num);
            print("locked mcr1\n", p_num);
            sem_getvalue(&scr1, &sem_value);
            if (sem_value + 1 == size_stack)
            {
                print("stack is full\n", p_num);
                full_stack++;
                if (full_stack == 2)
                    break;
            }
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &prev_state);
            temp = GetElemStack();
            pthread_setcancelstate(prev_state, NULL);
            sem_getvalue(&scr1, &sem_value);
            fprintf(f, "Thread P[%d]: element %d TAKEN; current semaphore = %d;\n", p_num, temp, sem_value);
            printf("Thread P[%d]: element %d TAKEN; current semaphore = %d;\n", p_num, temp, sem_value);
            print("unlocked mcr1\n", p_num);
            pthread_mutex_unlock(&mcr1);
            usleep(1);
        }
        else
        {
            sem_post(&scr1);
            usleep(20);
        }
    }
    pthread_cancel(p1);
    pthread_cancel(p2);
    pthread_cancel(p3);
    pthread_cancel(p5);
    pthread_cancel(p6);
    print("stopped all threads and was stopped too\n", p_num);
    return NULL;
}

void *P5thread(void *p_number)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    print("begins\n", p_num);
    int prev_state = 0;
    int sem_value = 0;
    int temp = 0;
    while(1)
    {
        //print("is waiting the semaphore scr1\n", p_num);
        sem_wait(&scr1);
        if (pthread_mutex_trylock(&mcr1) == 0)
        {
            print("already received and reduced semaphore scr1\n", p_num);
            print("locked mcr1\n", p_num);
            sem_getvalue(&scr1, &sem_value);
            if (sem_value + 1 == size_stack)
            {
                print("stack is full\n", p_num);
                full_stack++;
                if (full_stack == 2)
                    break;
            }
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &prev_state);
            temp = GetElemStack();
            pthread_setcancelstate(prev_state, NULL);
            sem_getvalue(&scr1, &sem_value);
            fprintf(f, "Thread P[%d]: element %d TAKEN; current semaphore = %d;\n", p_num, temp, sem_value);
            printf("Thread P[%d]: element %d TAKEN; current semaphore = %d;\n", p_num, temp, sem_value);
            print("unlocked mcr1\n", p_num);
            pthread_mutex_unlock(&mcr1);
        }
        else
        {
            sem_post(&scr1);
        }
        pthread_mutex_lock(&mut_sig21);
        while (flag_sig21_P5 == 0)
        {
            print("is waiting sig21\n", p_num);
            pthread_cond_wait(&sig21, &mut_sig21);
        }
        flag_sig21_P5 = 0;
        print("already received sig21\n", p_num);
        pthread_mutex_unlock(&mut_sig21);
        AtomicUsing();
        print("used atomic values\n", p_num);
    }
    pthread_cancel(p1);
    pthread_cancel(p2);
    pthread_cancel(p3);
    pthread_cancel(p4);
    pthread_cancel(p6);
    print("stopped all threads and was stopped too\n", p_num);
    return NULL;
}

void *P6thread(void *p_number)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int p_num = *((int*)p_number);
    print("begins\n", p_num);
    AtomicModification();
    print("modificated atomic values\n", p_num);
    while (1)
    {
        print("waits for the thread P[3] at the barrier bcr2\n", p_num);
        pthread_barrier_wait(&bcr2);
        print("works after barrier bcr2\n", p_num);
        pthread_mutex_lock(&mut_sig21);
        pthread_cond_broadcast(&sig21);
        flag_sig21_P2 = 1;
        flag_sig21_P5 = 1;
        print("sent sig21 for thread P[2] and thread P[5]\n", p_num);
        pthread_mutex_unlock(&mut_sig21);
        AtomicUsing();
        print("used atomic values\n", p_num);
        usleep(20);
    }
    print("stopped all threads and was stopped too\n", p_num);
    return NULL;
}
