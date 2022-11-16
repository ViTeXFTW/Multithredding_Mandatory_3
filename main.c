//
// Created by Mads on 02/11/2022.
//

#include "stdio.h"
#include<pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys\timeb.h>

#include <Windows.h>

typedef struct {
    pthread_rwlock_t *lock;
    pthread_t *tid;
    int *stuckArr[9];
    int *stuck;
    int i;
} arg_struct;

pthread_rwlock_t lock_rw = PTHREAD_RWLOCK_INITIALIZER;

void wrlock(pthread_rwlock_t *lock_rw) {
    if (pthread_rwlock_wrlock(lock_rw) != 0) {
        perror("master thread: pthread_rwlock_wrlock error");
        exit(__LINE__);
    }
}

void rdlock(pthread_rwlock_t *lock_rw) {
    if (pthread_rwlock_rdlock(lock_rw) != 0) {
        perror("master thread: pthread_rwlock_rdlock error");
        exit(__LINE__);
    }
}

void unlock(pthread_rwlock_t *lock_rw) {
    if (pthread_rwlock_unlock(lock_rw) != 0) {
        perror("master thread: pthread_unlock_wrlock error");
        exit(__LINE__);
    }
}

char sudoku[9][9];

char sudokus[1][9][9] = {{{1, 9, 3, 0, 4, 2, 7, 6, 0},
                          {7, 0, 0, 0, 0, 0, 0, 9, 0},
                          {8, 0, 0, 0, 7, 6, 0, 5, 3},
                          {9, 4, 0, 6, 1, 3, 0, 8, 0},
                          {6, 3, 7, 2, 8, 0, 5, 1, 9},
                          {0, 8, 1, 7, 0, 0, 0, 4, 6},
                          {5, 1, 0, 8, 0, 0, 0, 0, 0},
                          {0, 6, 0, 0, 2, 0, 0, 7, 1},
                          {3, 7, 0, 4, 9, 1, 6, 0, 0},
                         },};

/*
char sudokus[6][9][9] = {{{0, 3, 0, 0, 0, 1, 9, 0, 0},
                                 {1, 0, 9, 5, 0, 0, 2, 0, 4},
                                 {2, 0, 0, 9, 0, 7, 6, 0, 5},
                                 {0, 0, 0, 0, 0, 0, 7, 0, 0},
                                 {0, 5, 0, 3, 1, 9, 4, 6, 2},
                                 {3, 2, 0, 6, 0, 0, 0, 5, 0},
                                 {7, 0, 0, 0, 0, 3, 0, 0, 0},
                                 {6, 9, 0, 1, 5, 4, 8, 0, 0},
                                 {0, 8, 0, 0, 0, 0, 0, 9, 0},
                         },
                         {{4, 0, 0, 0, 0, 3, 0, 0, 1},
                                 {0, 1, 8, 0, 0, 0, 0, 7, 0},
                                 {0, 0, 0, 1, 6, 8, 0, 2, 0},
                                 {0, 9, 2, 8, 0, 0, 0, 4, 0},
                                 {5, 0, 0, 2, 7, 9, 0, 1, 8},
                                 {0, 6, 0, 0, 3, 5, 0, 9, 0},
                                 {9, 8, 6, 0, 0, 0, 0, 3, 2},
                                 {0, 7, 0, 6, 8, 0, 4, 5, 9},
                                 {0, 0, 5, 3, 0, 0, 0, 0, 6},
                         },
                         {{0, 0, 0, 9, 0, 0, 5, 0, 7},
                                 {0, 0, 0, 6, 0, 0, 0, 0, 0},
                                 {9, 8, 1, 0, 0, 7, 0, 6, 0},
                                 {0, 0, 9, 1, 0, 3, 2, 5, 0},
                                 {1, 0, 6, 5, 0, 2, 0, 7, 0},
                                 {0, 5, 2, 0, 6, 9, 0, 0, 4},
                                 {0, 0, 0, 3, 1, 0, 7, 0, 0},
                                 {0, 1, 0, 4, 2, 0, 0, 3, 9},
                                 {0, 0, 0, 0, 0, 6, 1, 8, 5},
                         },
                         {{1, 9, 3, 0, 4, 2, 7, 6, 0},
                                 {7, 0, 0, 0, 0, 0, 0, 9, 0},
                                 {8, 0, 0, 0, 7, 6, 0, 5, 3},
                                 {9, 4, 0, 6, 1, 3, 0, 8, 0},
                                 {6, 3, 7, 2, 8, 0, 5, 1, 9},
                                 {0, 8, 1, 7, 0, 0, 0, 4, 6},
                                 {5, 1, 0, 8, 0, 0, 0, 0, 0},
                                 {0, 6, 0, 0, 2, 0, 0, 7, 1},
                                 {3, 7, 0, 4, 9, 1, 6, 0, 0},
                         },
                         {{0, 0, 2, 5, 3, 0, 1, 7, 0},
                                 {0, 3, 8, 0, 0, 0, 2, 0, 0},
                                 {0, 1, 0, 0, 9, 0, 4, 0, 0},
                                 {2, 6, 0, 0, 0, 0, 0, 8, 0},
                                 {0, 8, 0, 6, 0, 5, 0, 4, 2},
                                 {0, 0, 0, 0, 8, 0, 0, 1, 0},
                                 {0, 7, 0, 9, 0, 0, 5, 0, 0},
                                 {1, 5, 9, 0, 0, 0, 0, 6, 4},
                                 {0, 2, 0, 7, 0, 1, 0, 0, 9},
                         },
                         {{0, 0, 0, 0, 0, 0, 1, 0, 0},
                                 {0, 0, 0, 1, 0, 2, 0, 0, 7},
                                 {9, 8, 1, 0, 0, 0, 4, 2, 0},
                                 {1, 0, 0, 0, 0, 0, 6, 0, 0},
                                 {3, 0, 0, 9, 0, 7, 0, 0, 1},
                                 {0, 0, 2, 0, 0, 0, 0, 0, 4},
                                 {0, 7, 3, 0, 0, 0, 8, 5, 6},
                                 {5, 0, 0, 2, 0, 8, 0, 0, 0},
                                 {0, 0, 4, 0, 0, 0, 0, 0, 0},},
};
*/

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

void printMySudoku(char sudoku[9][9]) {
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
    printf("\n");
}

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
    printf("\n");
}

int oldCheck() {
    for (int i = 0; i < 9; ++i) {
        char found[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

        for (int j = 0; j < 9; ++j) {
            if (sudoku[i][j] == 0) {
                return 0;
            }
            if (found[sudoku[i][j] - 1]) {
                return 0;
            }
            found[sudoku[i][j] - 1]++;
        }
    }

    return 1;
}

int checkSudoku(void *vargp) {
    arg_struct *arg = vargp;
    pthread_rwlock_t *p = arg->lock;
    //char localSudoku[9][9];

    for (int i = 0; i < 9; ++i) {
        char found[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

        for (int j = 0; j < 9; ++j) {
            //rdlock(p);
            int val = sudoku[i][j];
            //unlock(p);
            if (val == 0) {
                return 0;
            }
            if (found[val - 1]) {
                return 0;
            }
            found[val - 1]++;
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
    memcpy(&localSudoku, &sudoku, sizeof(char) * 9 * 9);
    if (pthread_rwlock_unlock(p) != 0) {
        perror("reader threadB: pthred_rwlock_unlock error");
        exit(__LINE__);
    }

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
            //Sleep(1);
            if (sudoku[i][j]) continue;

            if (checkBoxFromPos(number, i, j, localSudoku)) {
                j = 2 + (j / 3) * 3;
                continue; //TODO: do more here?
            }
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

            //printf("Thread %d, Put in number: %d, at V: %d, H: %d\n", myid, number, i + 1, position + 1);
            //printSudoku();
            return 1;

            //Sleep(50);
        }

    }
    return 0;
}


int oldBrute(char sudoku[9][9]) {

    int found = 0;
    for (int number = 1; number <= 9; ++number) {

        for (int i = 0; i < 9; ++i) {
            if (checkColumn(number, i, sudoku)) {
                continue;
            }
            int potentialPositions = 0;
            int position = 0;


            for (int j = 0; j < 9; ++j) {
                //Sleep(25);
                if (sudoku[i][j]) continue;

                if (checkBoxFromPos(number, i, j, sudoku)) {
                    j = 2 + (j / 3) * 3;
                    continue; //TODO: do more here?
                }
                if (!checkLine(number, j, sudoku)) {
                    potentialPositions++;
                    position = j;
                }
            }

            if (potentialPositions == 1) {

                if (!sudoku[i][position])
                    sudoku[i][position] = number;
                found++;

            }
        }
    }
    return found;
}

int emptyCounter(char sudoku[9][9]) {
    int counter = 0;

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (!sudoku[j][i]) counter++;
        }
    }

    return counter;
}

int leastEmptyRow(char sudoku[9][9]) {
    int row = -1;
    int counter = 9;

    for (int i = 0; i < 9; ++i) {
        int rowCounter = 9;
        for (int j = 0; j < 9; ++j) {
            if (sudoku[i][j]) rowCounter--;
        }
        if (rowCounter) {
            if (rowCounter < counter) {
                counter = rowCounter;
                row = i;
            }
        }
    }
    return row;
}


struct list {
    int i;
    struct list *next;
} list_t;

int smarterSolve(char sudoku[9][9]) {
    int empty = emptyCounter(sudoku);
    if (empty > 10 || empty == 0) {
        return empty == 0;
    }

    int least = leastEmptyRow(sudoku);

    char original[9][9];
    memcpy(&original, &sudoku, sizeof(char) * 9 * 9);

    for (int i = 0; i < 9; ++i) {
        if (sudoku[least][i]) continue;

        for (int number = 0; number < 9; ++number) {
            if (checkLine(number, least, sudoku)) continue;
            sudoku[least][i] = number;
            for (int j = 0; j < empty; ++j) {
                if (oldBrute(sudoku) == 0) break;
            }
            if (oldCheck(sudoku)) {
                printf("Solved by smart solve!\n");
                printMySudoku(sudoku);
                return 1;
            }
            memcpy(&sudoku, &original, sizeof(char) * 9 * 9);
        }
    }
    return 0;
}

void smartSolve(char sudoku[9][9]) {
    int least;
    if (emptyCounter(sudoku) > 10 || emptyCounter(sudoku) == 0) {
        return;
    }

    least = leastEmptyRow(sudoku);


    char found[9];
    for (int i = 0; i < 9; ++i) {
        if (sudoku[least][i]) {
            found[sudoku[least][i] - 1] = 1;
        }
    }
    struct list *next;
    next = malloc(sizeof(list_t));

    struct list *head = next;
    next->i = 0;

    for (int i = 0; i < 9; ++i) {
        if (found[i]) continue;

        if (next->i == 0) {
            next->i = i + 1;
        } else {
            next->next = malloc(sizeof(list_t));
            next = next->next;
            next->i = i + 1;
        }
    }

    next = head;

    if (head->i == 0) return;
    while (next->next != NULL) {
        if (next->i <= 0) break;
        printf("Missing %d\n", next->i);
        next = next->next;
    }

    char original[9][9];
    memcpy(&original, &sudoku, sizeof(char) * 9 * 9);


    next = head;
    while (next->next != NULL) {
        for (int i = 0; i < 9; ++i) {
            if (sudoku[least][i]) {
                continue;
            }
            printf("trying %d at j %d, i %d\n", next->i, least, i);
            fflush(stdout);
            sudoku[least][i] = next->i;
            int fail = 0;
            while (fail < 2) {
                if (!oldBrute(sudoku)) {
                    fail++;
                } else {
                    fail = 0;
                }
            }
            printSudoku();
            break;
        }
        if (oldCheck(sudoku)) break;

        memcpy(&sudoku, &original, sizeof(char) * 9 * 9);
        next = next->next;
    }

    next = head;
    while (next->next != NULL) {
        void *temp = next;
        next = next->next;
        free(temp);
    }

}

void *runBrute(void *vargp) {
    arg_struct *arg = vargp;
    int *myid = (int *) arg->tid;

    //printf("Thread ID: %d, Start: %d\n", *myid, arg->i);
    //Sleep(2);

    while (1) {
        *(arg->stuck) = brute(arg->i, *myid, arg->lock);
    }
}

void *runCheck(void *vargp) {
    int i = 0;
    while (!checkSudoku(vargp)) {
        i++;
        if (i > 20000) {
            i = 0;
            return 0;
        }
    }
    //printf("Check complete!\n");
    return 0;
}

void *runStuck(void *vargp) {
    arg_struct *arg = vargp;

    while (1) {
        int total = 0;
        while (arg->i) {
            arg->i = !checkSudoku(vargp);


            for (int j = 0; j < 9; ++j) {
                //printf("s %d, %d\n", j, *arg->stuckArr[j]);
                if (*arg->stuckArr[j]) {
                    total++;
                }
            }
            //printf("Total: %d\n", total);
            if (total < 9) {
                total = 0;
            }
            if (total >= 18) {
                //printf("Stuck?\n");
                arg->i = 0;
            }
        }
    }


}

void runSingle() {
    int successCounter = 0;

    int length = 1000;//= sizeof(sudokus) / sizeof(sudokus[0]);
    for (int s = 0; s < length; ++s) {

        //printf("Starting new sudoku %d\n\n", s + 1);

        memcpy(&sudoku, &sudokus[0], sizeof(char) * 9 * 9);

        //printSudoku();

        while (oldBrute(sudoku));

        /*
        if (!oldCheck(sudoku)) {
            printf("\n");
            printSudoku();
            printf("\n");

            printf("Solved by smartsolve: %s", smarterSolve(sudoku) ? "Yes" : "No");
        }
        */

        //printf("\n\n");
        //printSudoku();


        arg_struct arg4;
        arg4.lock = &lock_rw;
        int success = checkSudoku(&arg4);
        //printf("Done? %s\n\n", success ? "Yes" : "No");
        if (success) {
            successCounter++;
        }
    }
    printf("\nSingle solved %d/%d\n", successCounter, length);
}

void runMulti() {
    int successCounter = 0;

    pthread_t checkTid;
    pthread_t stuckTid;
    pthread_t tid[9];
    int stuck[9];

    void *vp;

    int length = 1000; //sizeof(sudokus) / sizeof(sudokus[0]);
    for (int s = 0; s < length; ++s) {

        //printf("Starting new sudoku %d\n\n", s + 1);

        wrlock(&lock_rw);
        memcpy(&sudoku, &sudokus[0], sizeof(char) * 9 * 9);
        unlock(&lock_rw);

        //printSudoku();


        /*
        arg_struct arg1;
        arg1.lock = &lock_rw;
        arg1.tid = &checkTid;
        if (pthread_create(&checkTid, NULL, runCheck, &arg1) != 0) {
            perror("pthread_create error");
            exit(__LINE__);
        }
        */

        arg_struct arg2;
        arg2.i = 1;

        if (s == 0) {
            arg2.lock = &lock_rw;
            arg2.tid = &checkTid;
            for (int i = 0; i < 9; ++i) {
                stuck[i] = 0;
                arg2.stuckArr[i] = &stuck[i];
            }
            if (pthread_create(&stuckTid, NULL, runStuck, &arg2) != 0) {
                perror("pthread_create error");
                exit(__LINE__);
            }
            arg_struct arg[9];
            for (int j = 0; j < 9; ++j) {
                arg[j].lock = &lock_rw;
                arg[j].i = j + 1;
                arg[j].tid = &tid[j];
                arg[j].stuck = &stuck[j];
                stuck[j] = 0;
                if (pthread_create(&tid[j], NULL, runBrute, &arg[j]) != 0) {
                    perror("pthread_create error");
                    exit(__LINE__);
                }
            }

            //Sleep(1);
        }
        //printf("Waiting...\n");

        /*if (pthread_rwlock_unlock(&lock_rw) != 0) {
            perror("writer thread: pthread_rwlock_wrlock error");
            exit(__LINE__);
        }*/


        //wait for the solvers to be stuck
        /*if (pthread_join(stuckTid, &vp) != 0) {
            perror("pthread_join error");
            exit(__LINE__);
        }*/

        arg2.i = 1;
        while (arg2.i);

        //wait for the thread to complete
        /*if (pthread_join(checkTid, &vp) != 0) {
            perror("pthread_join error");
            exit(__LINE__);
        }
         */

        // Kill all worker threads if last sudoku
        if (s == length - 1) {
            for (int i = 0; i < 9; ++i) {
                pthread_cancel(tid[i]);
            }
            pthread_cancel(stuckTid);
        }

        //printf("\n\n");
        //printSudoku();


        arg_struct arg4;
        arg4.lock = &lock_rw;
        int success = checkSudoku(&arg4);
        if (success) {
            successCounter++;
        }
        //printf("Done? %s\n\n", success ? "Yes" : "No");


    }
    printf("\nMulti solved %d/%d\n", successCounter, length);
}

int main() {
    struct timeb start, mid, end;
    int diff;


    ftime(&start);
    runSingle();
    ftime(&mid);
    runMulti();
    ftime(&end);

    diff = (int) (1000.0 * (mid.time - start.time)
                  + (mid.millitm - start.millitm));

    printf("\nSingle took %u milliseconds\n", diff);

    diff = (int) (1000.0 * (end.time - mid.time)
                  + (end.millitm - mid.millitm));

    printf("\nMulti took %u milliseconds\n", diff);
    // pthread_exit(NULL);
    return 0;
}

