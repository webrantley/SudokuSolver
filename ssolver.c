#include "ssolver.h" 
#include <string.h>

cell board[9][9];
char test;
char* filename;

//Writes the solved puzzle with the same format as print_board to 
//the a file with the original name with .solved appened to it.
void write_back(cell board[][9]){
    char str[255];
    sprintf(str, "%s.solved", filename);
    FILE* output = fopen(str, "w");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++)
            fprintf(output, "%d", board[i][j].elem);
        fprintf(output, "\n");
    }
}

//Will print the board to stdout as 9 numbers per line, with no special
//formatting to separate blocks.
void print_board(){
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            printf("%d ", board[i][j].elem);
        }
        printf("\n");
    }
}

//Change the pointers to x and y to the x, y coordinates of the previous
//cell of the Sudoku board that was not giving at the beginning. x and y
//will become -1 and -1 if the position of x and y was the top left most
//unassigned cell.
void previous_blank(int* x, int* y) {
    //printf("Searching for next blank from [%d, %d]\n", *x, *y);
    for(int i = (9*(*x) + *y) - 1; i >= 0; i--)
            if (!board[i / 9][i % 9].given) {
                *x = i / 9;
                *y = i % 9;
                return;
            }
}

//Change the pointers to x and y to the x, y coordinates of the next
//cell of the Sudoku board that was not giving at the beginning. x and y
//will become -1 and -1 if the position of x and y was the bottom right most
//unassigned cell.
void next_blank(int* x, int* y) {
    for(int i = *x; i < 9; i++)
        for(int j = 0; j < 9; j++) {
            //printf("Checking [%d,%d]\n", i, j);
            if (!board[i][j].given && ((9*(*x) + *y) < (9*i + j))) {
                *x = i;
                *y = j;
                return;
            }
        }
    *x = -1;
    *y = -1;
}

void solver(int i, int j) {
    int backtracking;
    int prev_i, prev_j;
    int next_i, next_j;

    prev_i = next_i = i;
    prev_j = next_j = j;

    while(1) {
        backtracking = 0;
        previous_blank(&prev_i, &prev_j);
        next_blank(&next_i, &next_j);

        if (i == -1 && j == -1) {
            if (board_checker(board))
                printf("board looks right\n");
            return;
        }

        while(!valid_checker(board, ++board[i][j].elem, i, j)) {
            if(board[i][j].elem == 10) {
                board[i][j].elem = 0;
                i = prev_i;
                j = prev_j;
                prev_i = next_i = i;
                prev_j = next_j = j;
                backtracking = 1;
                break;
            }
        }
        if (backtracking){
            continue;
        }
        else {
            //this means we're moving forward. We found a valid position
            //for a cell.
            i = next_i;
            j = next_j;
            prev_i = next_i = i;
            prev_j = next_j = j;
        }
    }
}


int main(int argc, char** argv) {
    FILE* input_f = fopen(argv[1], "r");

    int start_i = 0;
    int start_j = 0;

    if (!input_f) {
        printf("Error opening file %s\n", argv[1]);
        exit(1);
    }

    //read input from file
    int num;
    int count = 0;
    while((num = fgetc(input_f)) != EOF){
        if (num == '\n') continue;
        else {
            board[count / 9][count % 9].elem = num - '0';
            board[count / 9][count % 9].given = (num - '0' > 0) ? 1 : 0;
            count++;
        }
    }
    char* last_dot;
    last_dot = strrchr(argv[1], '.');

    if (last_dot == NULL){
        printf("Filename malformed. Should have a .in at the end\n");
        exit(1);
    }
    *last_dot = '\0';
    filename = argv[1];

    if(board[0][0].given)
        next_blank(&start_i, &start_j);

    solver(start_i, start_j);
    write_back(board);

    return 0;
}
