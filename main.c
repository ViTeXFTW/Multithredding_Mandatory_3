//
// Created by Mads on 02/11/2022.
//

#include "stdio.h"
#include<pthread.h>
#include <stdlib.h>
#include <string.h>

#include <Windows.h>

typedef struct {
    pthread_rwlock_t *lock;
    pthread_t *tid;
    int i;
} arg_struct;

pthread_rwlock_t lock_rw = PTHREAD_RWLOCK_INITIALIZER;

//13
char sudoku[9][9] = {{4, 0, 1, 8, 0, 0, 0, 0, 0},
                    {0, 2, 0, 3, 0, 4, 9, 0, 0},
                    {9, 0, 5, 7, 0, 0, 6, 0, 8},
                    {3, 7, 2, 0, 0, 5, 1, 8, 0},
                    {0, 0, 0, 0, 3, 0, 0, 9, 0},
                    {0, 1, 0, 6, 0, 7, 0, 2, 0},
                    {0, 8, 7, 9, 0, 0, 2, 6, 0},
                    {0, 0, 0, 5, 1, 6, 8, 0, 0},
                    {0, 0, 4, 0, 0, 0, 0, 0, 9},};

//277
char sudoku2[9][9] = {{0, 0, 0, 0, 0, 0, 1, 0, 0},
                     {0, 0, 0, 1, 0, 2, 0, 0, 7},
                     {9, 8, 1, 0, 0, 0, 4, 2, 0},
                     {1, 0, 0, 0, 0, 0, 6, 0, 0},
                     {3, 0, 0, 9, 0, 7, 0, 0, 1},
                     {0, 0, 2, 0, 0, 0, 0, 0, 4},
                     {0, 7, 3, 0, 0, 0, 8, 5, 6},
                     {5, 0, 0, 2, 0, 8, 0, 0, 0},
                     {0, 0, 4, 0, 0, 0, 0, 0, 0},};


char finalSudoku[9][9] = {{4, 6, 1, 8, 5, 9, 7, 3, 2},
                         {7, 2, 8, 3, 6, 4, 9, 5, 1},
                         {9, 3, 5, 7, 2, 1, 6, 4, 8},
                         {3, 7, 2, 4, 9, 5, 1, 8, 6},
                         {8, 4, 6, 1, 3, 2, 5, 9, 7},
                         {5, 1, 9, 6, 8, 7, 4, 2, 3},
                         {1, 8, 7, 9, 4, 3, 2, 6, 5},
                         {2, 9, 3, 5, 1, 6, 8, 7, 4},
                         {6, 5, 4, 2, 7, 8, 3, 1, 9},};


void printSudoku() {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (sudoku[i][j] == 0) {
                printf(" x ");
            } else {
                printf(" %d ", sudoku[i][j]);
            }
        }
        printf("\n");
    }
}

int checkSudoku(void *vargp) {
    arg_struct *arg = vargp;
    pthread_rwlock_t *p = arg->lock;
    char localSudoku[9][9];

        if (pthread_rwlock_rdlock(p) != 0) {
        perror("reader_threadC: pthread_rwlock_rdlock error");
        exit(__LINE__);
    }
    memcpy(&localSudoku, &sudoku, sizeof (char) * 9 * 9);
    if (pthread_rwlock_unlock(p) != 0) {
        perror("reader threadC: pthred_rwlock_unlock error");
        exit(__LINE__);
    }

    for (int i = 0; i < 9; ++i) {
        char found[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

        for (int j = 0; j < 9; ++j) {
            if (localSudoku[i][j] == 0) {
                return 0;
            }
            if (found[localSudoku[i][j] - 1]) {
                return 0;
            }
            found[localSudoku[i][j] - 1]++;
        }
    }


    return 1;
}

// checkLine goes through each line and checks if the number is present. Returns true if found
int checkLine(int number, int j, char sudoku[9][9]) {
    for (int i = 0; i < 9; ++i) {
        if (sudoku[i][j] == number) {
            return 1;
        }
    }
    return 0;
}

// checkColumn goes through each column and checks if the number is present. Returns true if found
int checkColumn(int number, int i, char sudoku[9][9]) {

    for (int j = 0; j < 9; ++j) {
        if (sudoku[i][j] == number) {
            return 1;
        }
    }
    return 0;
}

// checkBox goes through the box and checks if the number is present. Returns true if found. Box ranges from 1-9.
int checkBox(int number, int box, char sudoku[9][9]) {
    box = box - 1;
    int offsetV = box / 3;
    int offsetH = box % 3;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (sudoku[i + offsetH * 3][j + offsetV * 3] == number) {
                return 1;
            }
        }
    }
    return 0;
}

// checkBoxFromPos goes through the box and checks if the number is present. Returns true if found. Box ranges from 1-9.
int checkBoxFromPos(int number, int i, int j, char sudoku[9][9]) {
    int offsetV = j / 3;
    int offsetH = i / 3;

    //printf("i: %d, j: %d, offv: %d, offh: %d\n", i, j, offsetV, offsetH);

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {

            if (sudoku[i + offsetH * 3][j + offsetV * 3] == number) {
                return 1;
            }
        }
    }
    return 0;
}


int brute(int number, int myid, pthread_rwlock_t *p) {
    char localSudoku[9][9];

    if (pthread_rwlock_rdlock(p) != 0) {
        perror("reader_threadB: pthread_rwlock_rdlock error");
        exit(__LINE__);
    }
    memcpy(&localSudoku, &sudoku, sizeof (char) * 9 * 9);
    if (pthread_rwlock_unlock(p) != 0) {
        perror("reader threadB: pthred_rwlock_unlock error");
        exit(__LINE__);
    }

    int found = 0;
    /*for (int round = 0; round < 9; ++round) {
        int number = ((round+start-1)%9)+1;*/
        //printf("%d, ",number);

        for (int i = 0; i < 9; ++i) {
            if (checkColumn(number, i, localSudoku)) {
                continue;
            }
            int potentialPositions = 0;
            int position = 0;


            for (int j = 0; j < 9; ++j) {
                Sleep(25);
                if (sudoku[i][j]) continue;

                //if ((i%3 == 0 ) && (j%3 == 0 )) {
                    if(checkBoxFromPos(number, i, j, localSudoku)) {
                        continue; //TODO: do more here?
                    }
                //}
                if (!checkLine(number, j, localSudoku)) {
                    potentialPositions++;
                    position = j;
                }
            }

            if (potentialPositions == 1) {

                if (pthread_rwlock_wrlock(p) != 0) {
                    perror("writer thread: pthread_rwlock_wrlock error");
                    exit(__LINE__);
                }
                if (!sudoku[i][position])
                    sudoku[i][position] = number;
                if (pthread_rwlock_unlock(p) != 0) {
                    perror("writer thread: pthread_rwlock_unlock error");
                    exit(__LINE__);
                }

                printf("Thread %d, Put in number: %d, at i: %d, j: %d\n", myid, number, i, position);
                found++;

                Sleep(50);
            }

        }
    //}
    return found;
}

void *runBrute(void *vargp) {
    arg_struct *arg = vargp;
    int *myid = (int *)arg->tid;

    printf("Thread ID: %d, Start: %d\n", *myid, arg->i);

    while (1){
        brute(arg->i, *myid, arg->lock);
    };
}

void *runCheck(void *vargp) {
    while(!checkSudoku(vargp)) {

    }
    printf("Check complete!");
    return 0;
}

int main() {
    printSudoku();

    pthread_t checkTid;
    pthread_t tid[9];

    void *vp;

    arg_struct arg1;
    arg1.lock = &lock_rw;
    arg1.tid = &checkTid;
    if (pthread_create(&checkTid, NULL, runCheck, &arg1) != 0) {
        perror("pthread_create error");
        exit (__LINE__);
    }

    /*
    if (pthread_rwlock_wrlock(&lock_rw) != 0) {
        perror("writer thread: pthread_rwlock_wrlock error");
        exit(__LINE__);
    }
     */

    arg_struct arg[9];
    for (int j = 0; j < 9; ++j) {
        arg[j].lock = &lock_rw;
        arg[j].i = j+1;
        arg[j].tid = &tid[j];
        if (pthread_create(&tid[j], NULL, runBrute, &arg[j]) != 0) {
            perror("pthread_create error");
            exit (__LINE__);
        }
    }

    printf("Waiting...\n");

    /*if (pthread_rwlock_unlock(&lock_rw) != 0) {
        perror("writer thread: pthread_rwlock_wrlock error");
        exit(__LINE__);
    }*/

    //wait for the thread to complete
    if (pthread_join(checkTid, &vp) != 0) {
        perror("pthread_join error");
        exit (__LINE__);
    }
    for (int i = 0; i < 9; ++i) {
        pthread_cancel(tid[i]);
    }
    printf("Done!");

    //while (brute(4));

    printf("\n\n");
    printSudoku();


    arg_struct arg4;
    arg4.lock = &lock_rw;
    printf("Done? %s\n", checkSudoku(&arg4) ? "Yes" : "No");


   // pthread_exit(NULL);
    return 0;
}