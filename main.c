//
// Created by Mads on 02/11/2022.
//

#include "stdio.h"

int sudoku[9][9] ={{4,0,1,8,0,0,0,0,0},
                   {0,2,0,3,0,4,9,0,0},
                   {9,0,5,7,0,0,6,0,8},
                   {3,7,2,0,0,5,1,8,0},
                   {0,0,0,0,3,0,0,9,0},
                   {0,1,0,6,0,7,0,2,0},
                   {0,8,7,9,0,0,2,6,0},
                   {0,0,0,5,1,6,8,0,0},
                   {0,0,4,0,0,0,0,0,9},};


int finalSudoku[9][9] ={{4,6,1,8,5,9,7,3,2},
                        {7,2,8,3,6,4,9,5,1},
                        {9,3,5,7,2,1,6,4,8},
                        {3,7,2,4,9,5,1,8,6},
                        {8,4,6,1,3,2,5,9,7},
                        {5,1,9,6,8,7,4,2,3},
                        {1,8,7,9,4,3,2,6,5},
                        {2,9,3,5,1,6,8,7,4},
                        {6,5,4,2,7,8,3,1,9},};


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

int checkSudoku() {

    for (int i = 0; i < 9; ++i) {
        char found[9] = {0,0,0,0,0,0,0,0,0};

        for (int j = 0; j < 9; ++j) {
            if (sudoku[i][j] == 0) {
                return 0;
            }
            if (found[sudoku[i][j]-1]) {
                return 0;
            }
            found[sudoku[i][j]-1]++;
            }
        }


    return 1;
}

void checkBox(){


}



int main() {
    printSudoku();
    printf("Done? %s", checkSudoku() ? "Yes" : "No");
    return 0;
}