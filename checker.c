#include "ssolver.h"

//Checks whether an indivicual cell violates any invariant of the Sudoku game.
//Used as a debugging tool tool so I can know whether I have generated an incorrect output.
int valid_checker(cell board [9][9], int elem, int row, int col) {
    if (elem > 9 || elem < 1)
        return 0;

    //check if the element already exists in the row 
    for (int i = 0; i < 9; i++)
        if (board[row][i].elem == elem && i != col) {
            return 0;
        }
    //check if the element already exists in the column 
    for (int j = 0; j < 9; j++)
        if (board[j][col].elem == elem && j != row) {
            return 0;
        }

    int row_start = 3 * (row / 3);
    int col_start = 3 * (col / 3);

    for (int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            if (board[row_start + i][col_start + j].elem == elem
                                        && row != row_start + i && col != col_start + j) {
                return 0;
            }

    return 1;
}
/* Returns 1 if the board contains a valid solution. That is, that
   all the elements are filled and the constraints are met. We check this
   by summing the elements of each column, row, and 3x3 block. With the
   condition that every element, e, s.t. 1<=e<=9, these will all equal to 
   45 if the board is correct*/
int board_checker(cell board[9][9]) {
    int row;
    int col;
    for(int i = 0; i < 9; i++) {
        row = col = 0;
        for(int j = 0; j < 9; j++) {
            row += board[i][j].elem;
            col += board[j][i].elem;
        }
        if (!(row == 45 && col == 45)) {
            printf("row or column %d wrong\n", i);
            return 0;
        }
    }

    int xstart = 0;
    int ystart = 0;
    int block;
    for(int count = 0; count < 9; count++){
        block = 0;
        if (count % 3 == 0 && count) {
            xstart += 3;
            ystart = 0;
        }
        else if (count) 
            ystart += 3;

        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                block += board[xstart + i][ystart + j].elem;
            }
        }
        if (block != 45) 
            return 0;
    }
    return 1;
}
