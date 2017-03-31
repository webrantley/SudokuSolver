#include <pthread.h>
#include <string.h>
#include "ssolver.h" 


cell board[3][9][9];
pthread_t* thread_handle;
char* filename; //filename without extension;

int finished = 0;
int first_i = 0;
int first_j = 0;

void write_back(cell board[][9]){
    /* Writes correct sudoku board to filename.out */
    char str[255];
    sprintf(str, "%s.out", filename);
    FILE* output = fopen(str, "w");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++)
            fprintf(output, "%d", board[i][j].elem);
        fprintf(output, "\n");
    }
}

void print_board(cell board[][9]){
    /* Debugging tool */
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            printf("%d ", board[i][j].elem);
        }
        printf("\n");
    }
}

void previous_blank(cell board[][9], int* x, int* y) {
    /* Finds the location of the prevoius nongiven cell on the board.
        The board is read line by line from left to right to look for empty
        spots. This finds the one most recently visited. */
    for(int i = (9*(*x) + *y) - 1; i >= 0; i--)
            if (!board[i / 9][i % 9].given) {
                *x = i / 9;
                *y = i % 9;
                return;
            }
}

void next_blank(cell board[][9], int* x, int* y) {
    /* Finds the location of the next nongiven spot. If the puzzle is completed,
        and thus solved, this function will set x and y to -1 and -1. */
    for(int i = *x; i < 9; i++)
        for(int j = 0; j < 9; j++) {
            if (!board[i][j].given && ((9*(*x) + *y) < (9*i + j))) {
                *x = i;
                *y = j;
                return;
            }
        }
    *x = -1;
    *y = -1;
}

void* solver(void* input) {
    int i = first_i; //starting positon
    int j = first_j; //starting position
    long rank = (long) input;
    int branch = rank + 1; //first branch we will take is the rank + 1

    int backtracking; //help keep track of relevant data if we need to backtrack
    int prev_i, prev_j;
    int next_i, next_j;

    prev_i = next_i = i;
    prev_j = next_j = j;

    while(1) {
        backtracking = 0;
        pthread_testcancel(); //set a cancellation point if we need to cancel a thread

        previous_blank(board[rank], &prev_i, &prev_j); //get previous blank in case we have to backtrack
        next_blank(board[rank], &next_i, &next_j); //get the next blank in case we move forward

        if (i == first_i && j == first_j){
            while(!valid_checker(board[rank], branch, i, j)){
                branch += 3;

                //if we have tried all the work that this process should try, let's terminate
                if (branch > 9){
                    //printf("hit a dead end, cancelling thread %ld\n", rank);
                    pthread_cancel(thread_handle[rank]);
                    pthread_testcancel();
                }
            }
            board[rank][i][j].elem = branch;
            i = next_i;
            j = next_j;
            prev_i = next_i = i;
            prev_j = next_j = j;
            branch += 3; //set branch to next position;
            continue;
        }

        if (i == -1 && j == -1 && !finished) {
            //Here the puzzle is solved
            finished = 1;
            for(int i = 0; i < 3; i++)
                if(i != rank){
                    //printf("we found the solution, cancelling thread %d\n", i);
                    pthread_cancel(thread_handle[i]);
                }
            if (board_checker(board[rank]))
                printf("board looks right\n");
            write_back(board[rank]);
            return NULL;
        }

        //try 1-9 in this position to see if we find a number that works
        while(!valid_checker(board[rank], ++board[rank][i][j].elem, i, j)) {
            if(board[rank][i][j].elem == 10) {
                //here we know this tree is a dead end and we must backtrack
                board[rank][i][j].elem = 0;
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
            //this means we're moving forward. We found a valid number for the cell
            i = next_i;
            j = next_j;
            prev_i = next_i = i;
            prev_j = next_j = j;
        }
    }
}


int main(int argc, char** argv) {
    FILE* input_f = fopen(argv[1], "r");

    if (!input_f) {
        printf("Error opening file %s\n", argv[1]);
        exit(1);
    }

    thread_handle = malloc(3 * sizeof(pthread_t));
    //read input from file
    int num;
    int count = 0;
    while((num = fgetc(input_f)) != EOF){
        if (num == '\n') continue;
        else {
            board[0][count / 9][count % 9].elem = num - '0';
            board[0][count / 9][count % 9].given = (num - '0' > 0) ? 1 : 0;
            count++;
        }
    }

    //get filename without extension
    char* last_dot;
    last_dot = strrchr(argv[1], '.');

    if (last_dot == NULL){
        printf("Filename malformed. Should have a .in at the end\n");
        exit(1);
    }
    *last_dot = '\0';
    filename = argv[1];

    //give each thread their own board
    for(int i = 0; i < 9; i++)
        for(int j = 0; j < 9; j++)
            board[1][i][j] = board[2][i][j] = board[0][i][j];

    //if the first position is given, we will start with the next unknown cell
    if (board[0][0][0].given)
        next_blank(board[0], &first_i, &first_j);

    for(long i = 0; i < 3; i++)
        pthread_create(&thread_handle[i], NULL, solver, (void*) i);
    
    for(int i = 0; i < 3; i++)
        pthread_join(thread_handle[i], NULL);
    
    free(thread_handle);
    return 0;
}