#ifndef BOARD_H
#define BOARD_H

// 24x24 board (576 elements)
#define ROW_SIZE 6
#define ARRAY_SIZE 36

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ncurses.h>
#include <unistd.h>


// void insert_shapes(int *board)
// {
//     int ofs = ROW_SIZE;
//     // check
//     board[(0 * ofs) + 0] = 1;
//     board[(0 * ofs) + 5] = 0;
//     board[(5 * ofs) + 0] = 0;
//     board[(5 * ofs) + 5] = 1;

//     // toad
//     board[(2 * ofs) + 2] = 1;
//     board[(2 * ofs) + 3] = 1;
//     board[(2 * ofs) + 4] = 1;
//     board[(3 * ofs) + 1] = 1;
//     board[(3 * ofs) + 2] = 1;
//     board[(3 * ofs) + 3] = 1;
// }

void insert_shapes(int *board)
{
    int ofs = ROW_SIZE;
    // still lifes 
    // block
    board[(1 * ofs) + 2] = 1;
    board[(1 * ofs) + 3] = 1;
    board[(2 * ofs) + 2] = 1;
    board[(2 * ofs) + 3] = 1;

    // beehive
    board[(5 * ofs) + 2] = 1;
    board[(5 * ofs) + 3] = 1;
    board[(6 * ofs) + 1] = 1;
    board[(6 * ofs) + 4] = 1;
    board[(7 * ofs) + 2] = 1;
    board[(7 * ofs) + 3] = 1;

    // tub
    board[(10 * ofs) + 2] = 1;
    board[(11 * ofs) + 1] = 1;
    board[(11 * ofs) + 3] = 1;
    board[(12 * ofs) + 2] = 1;

    // boat
    board[(15 * ofs) + 1] = 1;
    board[(15 * ofs) + 2] = 1;
    board[(16 * ofs) + 1] = 1;
    board[(16 * ofs) + 3] = 1;
    board[(17 * ofs) + 2] = 1;

    // oscillators
    // blinker
    board[(1 * ofs) + 10] = 1;
    board[(1 * ofs) + 11] = 1;
    board[(1 * ofs) + 12] = 1;

    // toad
    board[(6 * ofs) + 10] = 1;
    board[(6 * ofs) + 11] = 1;
    board[(6 * ofs) + 12] = 1;
    board[(7 * ofs) + 9] = 1;
    board[(7 * ofs) + 10] = 1;
    board[(7 * ofs) + 11] = 1;
}

int *initBoard(int test)
{
    int i, j;

    int *board = malloc(sizeof(int) * ARRAY_SIZE);

    for (i = 0; i < ARRAY_SIZE; i += ROW_SIZE)
    {
        // assign values
        for (j = 0; j < ROW_SIZE; j += 1)
        {
            //random
            int r = ((rand() % 100) > 45) ? 0 : 1;
            board[i + j] = r * RAND;
        }
    }

    if (ARRAY_SIZE == 576)
    {
        insert_shapes(board);
    }

    return board;
}

void printBoard(int *board)
{
    int i, j;
    for (i = 0; i < ARRAY_SIZE; i += ROW_SIZE)
    {
        for (j = 0; j < ROW_SIZE; j += 1)
        {
            printf("%3d ", board[i + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void freeBoard(int **board)
{
    int i;
    for (i = 0; i < ROW_SIZE; i += 1)
    {
        free(board[i]);
    }
    free(board);
}

#endif