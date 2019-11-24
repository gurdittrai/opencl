#ifndef BOARD_H
#define BOARD_H

#define DELAY 1000000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ncurses.h>
#include <time.h>

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

int *initBoard(int arr_size, int row_size)
{
    int i, j;

    int *board = malloc(sizeof(int) * arr_size);

    for (i = 0; i < arr_size; i += row_size)
    {
        // assign values
        for (j = 0; j < row_size; j += 1)
        {
            //random
            int r = ((rand() % 100) > 45) ? 0 : 1;
            board[i + j] = r * RAND;
        }
    }

    if (arr_size == 576)
    {
        insert_shapes(board);
    }

    return board;
}

void printBoard(int *board, int arr_size, int row_size)
{
    int i, j;
    for (i = 0; i < arr_size; i += row_size)
    {
        for (j = 0; j < row_size; j += 1)
        {
            printf("%3d ", board[i + j]);
        }
        printf("\n");
    }
    printf("\n");
}

int drawBalls(int *board, int row_size, int k_iter, int turn)
{
    int i, j;

    // clear old screen
    clear();

    // draw header
    mvprintw(1, 1, "-- kernal %2d --", k_iter);
    mvprintw(1, 12, "-- board %c --", (turn ? 'B' : 'A'));

    // display balls
    for (i = 0; i < row_size; i += 1)
    {
        for (j = 0; j < row_size; j += 1)
        {
            if (board[(i * row_size) + j])
                mvprintw(i + 1, j + 1, "o");
        }
    }

    refresh();

    char ch = getch();
    if (ch == 'q')
    {
        return 0;
    }

    return 1;
}

#endif