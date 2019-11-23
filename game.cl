int count_neightbours(int *board, int y, int x)
{
    int count = 0;

    int i, j;
    for (i = y - 1; i < y + 2; i += 1)
    {
        // border
        if ((i < 0) || (i > 3))
            continue;

        for (j = x - 1; j < x + 2; j += 1)
        {
            //border
            if ((j < 0) || (j > 3))
                continue;

            if (board[(i*4)+j] == 1)
                count += 1;
        }
    }

    // -1 itself
    return count - board[(y*24)+x];
}

__kernel void game(__global const int *board_curr, __global const int *b, __global int *board_next)
{
    int gid = get_global_id(0);

    int y = (int) gid / 4;
    int x = (int) gid % 4;
    board_next[gid] = count_neightbours(a, y, x);
}
