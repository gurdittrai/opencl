int count_neightbours(int *board, int y, int x)
{
    int count = 0;

    int i, j;
    for (i = y - 1; i < y + 2; i += 1)
    {
        // border
        if ((i < 0) || (i > 4))
            continue;

        for (j = x - 1; j < x + 2; j += 1)
        {
            //border
            if ((j < 0) || (j > 4))
                continue;

            if (board[(i*5)+j] == 1)
                count += 1;
        }
    }

    // -1 itself
    return count - board[(y*5)+x];
}

__kernel void game(__global const int *board_curr, __global const int *b, __global int *board_next)
{
    int gid = get_global_id(0);

    int y = (int) gid / 4;
    int x = (int) gid % 4;

    int count = 0;

    int i;
    for (i = 0; i < get_local_size(); i += 1)
    {
    }

    

    // int i, j;
    // for (i = y - 1; i < y + 2; i += 1)
    // {
    //     // border
    //     if ((i < 0) || (i > 4))
    //         continue;

    //     for (j = x - 1; j < x + 2; j += 1)
    //     {
    //         //border
    //         if ((j < 0) || (j > 4))
    //             continue;

    //         if (board[(i*5)+j] == 1)
    //             count += 1;
    //     }
    // }

    board_next[gid] = (i*1000)+get_global_size();
}
