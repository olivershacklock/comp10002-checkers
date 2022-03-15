/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: [ Oliver Henry Alfred Shacklock 1264212 ]
  Dated:     [ 5 / 10 / 2021 ]

*/

/* header files --------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <math.h>

/* given global constants ----------------------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth
#define COMP_ACTIONS        10      // number of computed actions

/* my global constants -------------------------------------------------------*/
#define ROWS                BOARD_SIZE  // number of rows
#define COLS                BOARD_SIZE  // number of columns
#define INITIAL_TEAM_PIECES 12          // number of pieces a begins with
#define ASCII_OFFSET        65          // int difference between A and 0
#define CAPTURE_JUMP        2           // distance moved when capturing
#define PLAY                'P'         // input command to play 10 moves
#define ACTION              'A'         // input command to play 1 move
#define MAX_PIECE_MOVES     2           // max possible moves of a piece
#define MAX_TOWER_MOVES     4           // max possible moves of a tower
#define QUAD1               1           // 1st quadrant (top right)
#define QUAD2               2           // 2nd quadrant (bottom right)
#define QUAD3               3           // 3rd quadrant (bottom left)
#define QUAD4               4           // 4th quadrant (top left)

/* given type definitions ----------------------------------------------------*/
typedef char board_t[BOARD_SIZE][BOARD_SIZE];  // board type

/* my type definitions -------------------------------------------------------*/

typedef struct {                    // coordinate of a cell
    int row, col;
    char cell;
} cell_t;

typedef struct {                    // move from source cell to target cell
    cell_t src, tgt;
    int num;
} move_t;

typedef struct {
    move_t *moves_arr;
    int n_moves, max_moves;
} moveset_t;

typedef struct node node_t;         // tree node prototype

struct node {                       // minimax tree node
    board_t board;
    move_t move;
    int cost, temp_cost, propagated_cost, max_depth;
    int num_children, best_child;
    node_t **children;
};


/* my function prototypes ----------------------------------------------------*/

    /* stage 0 */
void set_board(board_t);
void print_board(board_t, int first_print);
node_t* make_empty_node(void);
void fill_node(node_t *node, board_t, move_t*);
void process_input(board_t, move_t *main_move, char *command);
void free_tree(node_t*);

    /* stage 1 & 2 */
node_t* play_best_move(node_t*, board_t, move_t  *main_move);

    /* stage 0 helper functions */
int illegal_move(board_t, move_t*);
int misc_illegal_move(board_t, move_t*);
void print_error(int error_num);
void make_move(board_t, move_t*);
void check_tower(board_t, move_t*);
void print_move_info(board_t, move_t*, int cost, int minimax_move);
int node_cost(node_t*);
int cost(board_t);
void check_winner(node_t*);

    /* stage 1 & 2 helper functions */
void fill_tree(node_t*, int depth);
move_t* fill_moves_arr(node_t *node, moveset_t *moveset);
move_t* get_moves(node_t*, cell_t*, moveset_t *moveset);
move_t* get_move(board_t, move_t*, int quadrant, moveset_t *moveset);
move_t* realloc_moves_arr(moveset_t *moveset);
int minimax_cost(node_t*, int best_max, int best_min);

    /* miscellaneous helper functions */
void copy_board(board_t original, board_t new);
int a2n(char);
char n2a(int);

/* main code -----------------------------------------------------------------*/
int
main(int argc, char *argv[]) {

    /* Stage 0 - reading, analysing, and printing input data */

    /* initialise and print starting board */
    board_t main_board = {{0}};
    set_board(main_board);
    print_board(main_board, 1);

    /* initialise empty starting move */
    move_t main_move;
    main_move.num = 0;

    /* read and process input */
    char command = '\0';
    process_input(main_board, &main_move, &command);
    
    /* input command was given */
    if (command) {

        /* make tree root and node for best move */
        node_t *main_node;
        main_node = make_empty_node();
        fill_node(main_node, main_board, &main_move);

        /* Stage 1 - compute and print next action */ 
        if (command == ACTION) {

            /* temporary pointer for tree */
            node_t *temp_root;
            temp_root = make_empty_node();
            *temp_root = *main_node;

            /* find best move and copy to main node */
            *main_node = *play_best_move(temp_root, main_board, &main_move);

            /* free temporary tree */
            free_tree(temp_root);
            temp_root = NULL;
        }  

        /* Stage 2 - machines game */
        else if (command == PLAY) {
            int i;
            for (i = 0; i < COMP_ACTIONS; i++) {        

                /* temporary pointer for tree */
                node_t *temp_root;
                temp_root = make_empty_node();
                *temp_root = *main_node;

                /* find best move and copy to main node */
                *main_node = *play_best_move(temp_root, main_board, &main_move); 

                /* free temporary tree */
                free_tree(temp_root);  
                temp_root = NULL;          
            }
        }
        free(main_node);
        main_node = NULL;
    }
    return EXIT_SUCCESS;
}

/* STAGE 0 -------------------------------------------------------------------*/

/* sets board to starting position 
*/
void
set_board(board_t board) {
    int row, col, i, j;
    for (i = 0; i < ROWS; i++) {
        row = i;
        for (j = 0; j < COLS; j++) {
            col = j;

            /* row is white */
            if (row < ROWS_WITH_PIECES) {

                /* alternate empty cells and row orientation */
                if ((row % 2 == 0) && (col % 2 == 0)) {
                    board[row][col] = CELL_EMPTY;                    
                }
                else if ((row % 2 == 0) && (col % 2 != 0)) {
                    board[row][col] = CELL_WPIECE;  
                }
                else if ((row % 2 != 0) && (col % 2 == 0)) {
                    board[row][col] = CELL_WPIECE;
                }
                else {
                    board[row][col] = CELL_EMPTY;
                }   
            }

            /* row is empty */
            else if ((ROWS_WITH_PIECES <= row) && 
                (row < ROWS - ROWS_WITH_PIECES)) {
                board[row][col] = CELL_EMPTY;
            }
            
            /* row is black */
            else {

                /* alternate empty cells and row orientation */
                if ((row % 2 == 0) && (col % 2 == 0)) {
                    board[row][col] = CELL_EMPTY;                    
                }
                else if ((row % 2 == 0) && (col % 2 != 0)) {
                    board[row][col] = CELL_BPIECE;  
                }
                else if ((row % 2 != 0) && (col % 2 == 0)) {
                    board[row][col] = CELL_BPIECE;
                }
                else {
                    board[row][col] = CELL_EMPTY;
                }   
            }
        }
    }
}

/* prints board in a nicer format
*/
void
print_board(board_t board, int first_print) {
    int i, j;
    
    /* initial board info */ 
    if (first_print) {
        printf("BOARD SIZE: %dx%d\n", ROWS, COLS);
        printf("#BLACK PIECES: %d\n", INITIAL_TEAM_PIECES);
        printf("#WHITE PIECES: %d\n", INITIAL_TEAM_PIECES);
    }
   
    /* column header */
    printf("     A   B   C   D   E   F   G   H\n");
    printf("   +---+---+---+---+---+---+---+---+\n");

    /* board rows */
    int row, col;
    for (i = 0; i < ROWS; i++) {
        row = i;

        /* row number */
        printf(" %d |", row + 1);

        /* row elements */
        for (j = 0; j < COLS; j++) {
            col = j;
            printf(" %c |", board[row][col]);
        }
        printf("\n   +---+---+---+---+---+---+---+---+\n");
    }
}

/* makes an empty minimax tree and passes its pointer 
*/
node_t*
make_empty_node() {
    node_t *node;
    node = malloc(sizeof(*node));
    assert(node != NULL);
    node->children = NULL;
    return node;
}

/* assigns node variables needed to fill a tree
*/
void
fill_node(node_t *node, board_t parent_board, move_t *cur_move) {
    copy_board(parent_board, node->board);
    node->move = *cur_move;
    node->max_depth = 0;
}

/* reads and applies input moves
*/
void
process_input(board_t main_board, move_t *main_move, char *command) {
    int num1, num2, error;
    char char1, char2;

    /* get moves from user */
    while (scanf("%c%d-%c%d\n", &char1, &num1, &char2, &num2) == 4) {  

        /* put game info into node */
        node_t *node;
        node = make_empty_node(); 
        fill_node(node, main_board, main_move);
        node->max_depth = 1; // ensure cost accounts for game end
        
        /* assign move coordinates */
        node->move.src.row = num1 - 1;
        node->move.tgt.row = num2 - 1;   
        node->move.src.col = a2n(char1);
        node->move.tgt.col = a2n(char2);
        node->move.num = node->move.num;

        /* assign move and board */
        node->move.src.cell = node->board[num1 - 1][a2n(char1)];
        node->move.tgt.cell = node->board[num2 - 1][a2n(char2)];
        *main_move = node->move; 
        
        /* check if move is valid */
        error = illegal_move(node->board, &node->move);
        if (error) {
            print_error(error);
            free(node);
            node = NULL;
            exit(EXIT_FAILURE);
        }

        /* apply move to node so game end can be checked */
        make_move(node->board, &node->move);

        /* apply move to main board */ 
        make_move(main_board, main_move);
        print_move_info(main_board, main_move, node_cost(node), 0);
        print_board(node->board, 0); 
        check_winner(node);
        free(node);
    }

    /* check for command at end of input */
    if ((char1 == ACTION) || (char1 == PLAY)) {
        *command = char1;
    }
}

/* returns allocated memory back to the heap 
*/
void
free_tree(node_t *node) {

    /* free leaf nodes */
    if (node->num_children == 0) {
        free(node); 
        node = NULL;    
        return;
    }

    int i;
    for (i = 0; i < node->num_children; i++) {
        free_tree(node->children[i]);
    }
    node->num_children = 0;
}


/* STAGE 1 & 2-----------------------------------------------------------------*/

/* plays move determined by minimax algorithm
*/
node_t*
play_best_move(node_t *root, board_t main_board, move_t *main_move) {

    /* determine best move using minimax */
    fill_tree(root, TREE_DEPTH);
    root->propagated_cost = minimax_cost(root, INT_MIN, INT_MAX);

    /* reassign root to child with best move */
    root = root->children[root->best_child];

    /* reset move number (make_move iterates it) */
    *main_move = root->move;
    main_move->num--;

    /* apply move to main board */ 
    make_move(main_board, main_move);
    print_move_info(main_board, main_move, node_cost(root), 1);
    print_board(main_board, 0);
    check_winner(root);

    /* return pointer to best node */
    return root;
}

/* stage 0 helper functions ------------------------------------------------- */

/* checks if move is illegal or not
*/
int
illegal_move(board_t board, move_t *move) {    
    int row1, row2, col1, col2, next_move_num, error;
    char src_cell = move->src.cell;
    char tgt_cell = move->tgt.cell;
    row1 = move->src.row;
    row2 = move->tgt.row;
    col1 = move->src.col;
    col2 = move->tgt.col;
    next_move_num = move->num + 1;

    /* ERROR #1: source cell is outside of the board */
    if ((row1 < 0) || (row1 > ROWS - 1) || (col1 < 0) || (col1 > COLS - 1)) {
        return error = 1;
    }

    /* ERROR #2: target cell is outside of the board */
    if ((row2 < 0) || (row2 > ROWS - 1) || (col2 < 0) || (col2 > COLS - 1)) {
        return error = 2;
    }

    /* ERROR #3: source cell is empty */
    if (src_cell == CELL_EMPTY) {
        return error = 3;
    }

    /* ERROR #4: target cell is not empty */
    if (tgt_cell != CELL_EMPTY) {
        return error = 4;
    }

    /* ERROR #5: source cell holds opponent's piece/tower */
    
    /* white turn, moving black piece */
    if (next_move_num % 2 == 0) {
        if ((src_cell == CELL_BPIECE) || (src_cell == CELL_BTOWER)) {
            return error = 5;
        }
    }

    /* black turn, moving white piece */
    else {
        if ((src_cell == CELL_WPIECE) || (src_cell == CELL_WTOWER)) {
            return error = 5;
        }
    }

    /* ERROR #6: illegal action */
    int misc_illegal = misc_illegal_move(board, move);
    if (misc_illegal) {
        return error = 6;
    }
    return error = 0;
}

/* checks other conditions for legal move
*/
int
misc_illegal_move(board_t board, move_t *move) {
    int row1, row2, col1, col2, row_jump, col_jump, illegal_move;
    row1 = move->src.row;
    row2 = move->tgt.row;
    col1 = move->src.col;
    col2 = move->tgt.col;
    row_jump = row2 - row1;
    col_jump = col2 - col1;
    char src_cell = board[row1][col1];

    /* piece is not moving diagonally (1:1) */
    if (abs(row_jump) != abs(col_jump)) {
        return illegal_move = 1;
    }
    
    /* piece is moving in wrong direction  */
    if ((src_cell == CELL_WPIECE) && (row_jump <= 0)) {
        return illegal_move = 1;    
    }
    else if ((src_cell == CELL_BPIECE) && (row_jump >= 0)) {
        return illegal_move = 1;
    }

    /* piece has jumped too far */
    if (row_jump > CAPTURE_JUMP) {
        return illegal_move = 1;    
    }
    
    /* a capture is being attempted */
    if (abs(row_jump) == CAPTURE_JUMP) {
        char cpt_cell = board[(row1 + row2) / 2][(col1 + col2) / 2];

        /* black must capture white */
        if ((src_cell == CELL_BPIECE) || (src_cell == CELL_BTOWER)) {
            if ((cpt_cell != CELL_WPIECE) && (cpt_cell != CELL_WTOWER)) {
                return illegal_move = 1;
            }
        }

        /* white must capture black */
        else if ((src_cell == CELL_WPIECE) || (src_cell == CELL_WTOWER)) {
            if ((cpt_cell != CELL_BPIECE) && (cpt_cell != CELL_BTOWER)) {
                return illegal_move = 1;
            }
        }
    }
    return illegal_move = 0;
}

/* prints type of move error by user
*/
void
print_error(int error_num) {
    if (error_num == 1) {
        printf("ERROR: Source cell is outside of the board.\n");
    }
    else if (error_num == 2) {
        printf("ERROR: Target cell is outside of the board.\n");
    }
    else if (error_num == 3) {
        printf("ERROR: Source cell is empty.\n");
    }
    else if (error_num == 4) {
         printf("ERROR: Target cell is not empty.\n");
    }
    else if (error_num == 5) {
        printf("ERROR: Source cell holds opponent's piece/tower.\n");
    }
    else if (error_num == 6) {
        printf("ERROR: Illegal action.\n");
    }
}

/* applies a move to the board and prints the result
*/
void
make_move(board_t board, move_t *move) {    
    int row1, col1, row2, col2;
    row1 = move->src.row;
    col1 = move->src.col;
    row2 = move->tgt.row;
    col2 = move->tgt.col;

    /* move piece and clear old position */
    board[row2][col2] = board[row1][col1];
    board[row1][col1] = CELL_EMPTY;  

    /* check if a capture was made */ 
    if (abs(row2 - row1) == abs(CAPTURE_JUMP)) {
    
        /* remove piece from play */
        board[(row1 + row2) / 2][(col1 + col2) / 2] = CELL_EMPTY;
    }
    check_tower(board, move);
    move->num++;
}

/* changes a piece to a tower if the necessary move is made
*/
void
check_tower(board_t board, move_t *move) {
    char piece;
    piece = board[move->tgt.row][move->tgt.col];

    if (piece == CELL_BTOWER || piece == CELL_WTOWER) {
        return;
    }

    if (piece == CELL_BPIECE && move->tgt.row == 0) {
        piece = CELL_BTOWER;
    }
    else if (piece == CELL_WPIECE && move->tgt.row == ROWS - 1) {
        piece = CELL_WTOWER;
    }
    board[move->tgt.row][move->tgt.col] = piece;
}

/* prints information about current board
*/
void
print_move_info(board_t board, move_t *move, int cost, int minimax_move) {
    int row1, row2;
    char col1, col2;

    /* convert index values to output values */
    row1 = move->src.row + 1;
    row2 = move->tgt.row + 1;
    col1 = n2a(move->src.col);
    col2 = n2a(move->tgt.col);

    /* delimiter */
    printf("=====================================\n");
    if (minimax_move) {
        printf("*** ");
    }
    if ((move->num % 2) == 0) {
        printf("WHITE ACTION #%d: %c%d-%c%d\n", move->num, 
            col1, row1, col2, row2);
    }
    else {
        printf("BLACK ACTION #%d: %c%d-%c%d\n", move->num, 
            col1, row1, col2, row2);

    }
    printf("BOARD COST: %d\n", cost);
}

/* calculates cost of the board
*/
int
node_cost(node_t* node) {

    /* determine if previous move ended the game */
    if (node->max_depth) {
        
        /* check if other player has moves in current state */
        moveset_t *moveset;
        moveset = (moveset_t*)malloc(sizeof(moveset_t));
        moveset->moves_arr = fill_moves_arr(node, moveset);
        node->num_children = moveset->n_moves;
    
        /* free used memory*/
        free(moveset->moves_arr);
        moveset->moves_arr = NULL;
        free(moveset);
        moveset = NULL;
    }

    /* player has no available moves */ 
    if (node->num_children == 0) {

        /* black to move, they lose */
        if (node->move.num % 2 == 0) {  
            node->cost = INT_MIN;
        }

        /* white to move, they lose */
        else {
            node->cost = INT_MAX;
        }
        return node->cost;
    }

    /* game did not end, calculate cost */
    node->cost = cost(node->board);
    return node->cost;
}

/* sums the total cost of the board
*/
int
cost(board_t board) {
    int row, col, cost, i, j;
    char piece;
    int bP = 0, bT = 0, wP = 0, wT = 0;

    /* sum cost of all pieces */
    for (i = 0; i < ROWS; i++) {
        row = i;
        for (j = 0; j < COLS; j++) {
            col = j;
            piece = board[row][col];

            /* add value of piece */
            if (piece == CELL_BPIECE) {
                bP++;
            }
            else if (piece == CELL_BTOWER) {
                bT++;
            }
            else if (piece == CELL_WPIECE) {
                wP++;
            }
            else if (piece == CELL_WTOWER) {
                wT++;
            }
        }
    }

    /* calculate cost according to formula */
    cost = ((bP * COST_PIECE) + (bT * COST_TOWER) - (wP * COST_PIECE) - 
        (wT * COST_TOWER));
    return cost;
}

/* ends the game if player wins
*/
void
check_winner(node_t *node) {
    if (node->cost == INT_MAX) {
        printf("BLACK WIN!\n");
        free_tree(node);
        exit(EXIT_SUCCESS);
    }
    else if (node->cost == INT_MIN) {
        printf("WHITE WIN!\n");
        free_tree(node);
        exit(EXIT_SUCCESS);
    }
    else {
        return;
    }
}

/* stage 1 & 2 helper functions ----------------------------------------------*/

/* fills minimax tree with possible board outcomes 
*/
void
fill_tree(node_t *node, int depth) {
    
    /* assign leaf node data */
    if (depth == 0) {
        node->max_depth = 1;
        node->cost = node_cost(node);
        node->num_children = 0; // reset after calculating cost (avoid segfault)
        return;
    }    

    /* allocate and generate a moveset for this node */
    moveset_t *moveset;
    moveset = malloc(sizeof(*moveset));
    assert(moveset != NULL);
    moveset->moves_arr = fill_moves_arr(node, moveset);
    node->num_children = moveset->n_moves;
    
    /* no moves can be made */
    if (node->num_children == 0) {
        node->cost = node_cost(node);
        free(moveset->moves_arr);
        moveset->moves_arr = NULL;
        free(moveset);
        moveset = NULL;
        return;
    }

    /* assign number of children based on possible moves */
    node->children = (node_t**)malloc(node->num_children * sizeof(node_t*));
    assert(node->children != NULL);
    
    /* create a new child for each move */
    int i;
    for (i = 0; i < node->num_children; i++) {
        
        /* create child node */
        node_t *child;
        child = make_empty_node();
        fill_node(child, node->board, &moveset->moves_arr[i]);
        make_move(child->board, &child->move);
        node->children[i] = child;
       
        /* find possible moves for each child node */
        fill_tree(child, depth - 1);
    }
    free(moveset->moves_arr);
    moveset->moves_arr = NULL;
    free(moveset);
    moveset = NULL;
}

/* fills array with available moves
*/
move_t*
fill_moves_arr(node_t *node, moveset_t *moveset) {

    /* initialise counters for array of moves */
    moveset->n_moves = 0;
    moveset->max_moves = MAX_TOWER_MOVES;
    
    /* get memory for array of moves */
    moveset->moves_arr = (move_t*)malloc(moveset->max_moves * sizeof(move_t));
    assert(moveset->moves_arr != NULL);
    
    /* find possible moves for each cell (row-major order) */
    int i, j;
    cell_t src_cell;
    for (i = 0; i < ROWS; i++) {
        src_cell.row = i;
        for (j = 0; j < COLS; j++) {
            src_cell.col = j;
            src_cell.cell = node->board[i][j];

            /* store moves for this cell in array */
            if (src_cell.cell != CELL_EMPTY) {
                moveset->moves_arr = get_moves(node, &src_cell, moveset);
            }           
        }
    }
    return moveset->moves_arr;
}

/* adds moves for an individual cell to array
*/
move_t*
get_moves(node_t *node, cell_t *src_cell, moveset_t *moveset) {

    /* initialise move */
    move_t cur_move; 
    cur_move.src = *src_cell;
    cur_move.tgt = *src_cell;
    cur_move.num = node->move.num; 

    /* moves for black piece */
    if (cur_move.src.cell == CELL_BPIECE) {

        /* check top right and left moves (1st and 4th quadrants) */
        moveset->moves_arr = get_move(node->board, &cur_move, QUAD1, moveset);
        moveset->moves_arr = get_move(node->board, &cur_move, QUAD4, moveset);
    return moveset->moves_arr;
    }

    /* moves for white piece */
    else if (cur_move.src.cell == CELL_WPIECE) {

        /* check bottom left and right moves (2nd and 3rd quadrants) */
        moveset->moves_arr = get_move(node->board, &cur_move, QUAD2, moveset);
        moveset->moves_arr = get_move(node->board, &cur_move, QUAD3, moveset);
    return moveset->moves_arr;
    }

    /* moves for any tower */ 
    else {

        /* check all four quadrants */
        moveset->moves_arr = get_move(node->board, &cur_move, QUAD1, moveset);
        moveset->moves_arr = get_move(node->board, &cur_move, QUAD2, moveset);
        moveset->moves_arr = get_move(node->board, &cur_move, QUAD3, moveset);
        moveset->moves_arr = get_move(node->board, &cur_move, QUAD4, moveset);
    }    
    return moveset->moves_arr;
}

/* adds legal move to an array of moves
*/
move_t*
get_move(board_t board, move_t *move, int quadrant, moveset_t *moveset) {
    int i, jump;
    for (i = 1; i <= CAPTURE_JUMP; i++) {
        jump = i;

        /* check upper right move */
        if (quadrant == 1) {
            move->tgt.row = move->src.row - jump;
            move->tgt.col = move->src.col + jump; 
            move->tgt.cell = board[move->tgt.row][move->tgt.col];
            if (illegal_move(board, move)) {
                continue;
            }   
        }

        /* check bottom right move */
        else if (quadrant == 2) {
            move->tgt.row = move->src.row + jump;
            move->tgt.col = move->src.col + jump;
            move->tgt.cell = board[move->tgt.row][move->tgt.col];
            if (illegal_move(board, move)) {
                continue;
            }
        }

        /* check bottom left move */
        else if (quadrant == 3) {
            move->tgt.row = move->src.row + jump;
            move->tgt.col = move->src.col - jump;
            move->tgt.cell = board[move->tgt.row][move->tgt.col];
            if (illegal_move(board, move)) {
                continue;
            }   
        }

        /* check top right move */
        else {
            move->tgt.row = move->src.row - jump;
            move->tgt.col = move->src.col - jump;
            move->tgt.cell = board[move->tgt.row][move->tgt.col];
            if (illegal_move(board, move)) {
                continue;
            }   
        }

        /* add move to moves array if legal */
        if (!illegal_move(board, move)) {
            moveset->moves_arr[(moveset->n_moves)++] = *move;
            moveset->moves_arr = realloc_moves_arr(moveset);
            return moveset->moves_arr;
        }  
    }
    return moveset->moves_arr;
}

/* resizes and reallocates memory for array of moves
*/
move_t*
realloc_moves_arr(moveset_t *moveset) {

    /* array reached maximum capacity */
    if (moveset->n_moves == moveset->max_moves) {

        /* reallocate array with 2x size */
        moveset->max_moves *= 2;
        moveset->moves_arr = (move_t*)realloc(moveset->moves_arr, 
            (moveset->max_moves * sizeof(move_t)));
        assert(moveset->moves_arr != NULL);
    }
    return moveset->moves_arr;      
}

/* find best move using minimax algorithm and alpha-beta pruning
*/
int 
minimax_cost(node_t *node, int best_max, int best_min) {

    /* base case - reach leaf node and return cost */
    if (node->num_children == 0) {
        node->propagated_cost = node->cost;
        return node->cost;
    }
 
    /* black to move, maximising cost */
    int i;
    if (node->move.num % 2 == 0) {

        /* assume losing position and find better child nodes */
        node->cost = INT_MIN;
        for (i = 0; i < node->num_children; i++) {
            
            /* recurse to base case - get leaf node cost */
            node->temp_cost = minimax_cost(node->children[i], 
                best_max, best_min);

            /* update node if child cost is better than current cost */
            if (node->temp_cost > node->cost) {
                node->cost = node->temp_cost;
                node->best_child = i;               
                best_max = node->cost;
            }

            /* white has a better move, don't need to compute further costs */ 
            if (best_min <= best_max) {
                break;
            }
        } 

        /* all children lead to losing position, choose first one */
        if (node->cost == INT_MIN) {
            node->best_child = 0;

        }
    }

    /* white to move, minimising cost */ 
    else {

        /* assume losing position and find better child nodes */
        node->cost = INT_MAX;
        for (i = 0; i < node->num_children; i++) {

            /* recurse to base case - get leaf node cost */
            node->temp_cost = minimax_cost(node->children[i], 
                best_max, best_min);

            /* update node if child cost is better than current cost */
            if (node->temp_cost < node->cost) {
                node->cost = node->temp_cost;
                node->best_child = i; 
                best_min = node->cost;
            }

            /* black has a better move, don't need to compute further costs */ 
            if (best_min <= best_max) {
                break;
            }
        }  

        /* all children lead to losing position, choose first one */
        if (node->cost == INT_MAX) {
            node->best_child = 0;
        }      
    }
    return node->cost;
}

/* miscellaneous helper functions --------------------------------------------*/

/* copies one board to another
*/
void 
copy_board(board_t original, board_t copy) {
    int i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            copy[i][j] = original[i][j];
        }
    }
}

/* converts column letter to its number equivalent 
*/
int 
a2n(char n) {
    n -= ASCII_OFFSET; // e.g. 'A' - 65 = 0
    return n;
}

/* converts column number to its letter equivalent 
*/
char 
n2a(int n) {
    n += ASCII_OFFSET;
    return n;
}

/* algorithms are fun :) */
/* THE END -------------------------------------------------------------------*/