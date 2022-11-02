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





int main() {
    printSudoku();
    return 0;
}