int count_neightbours(int *board, int y, int x, int row_size)
{
    int count = 0;

    int i, j;
    for (i = y - 1; i < y + 2; i += 1)
    {
        // border
        if ((i < 0) || (i > row_size-1))
            continue;

        for (j = x - 1; j < x + 2; j += 1)
        {
            //border
            if ((j < 0) || (j > row_size-1))
                continue;

            if (board[(i * row_size) + j] == 1)
                count += 1;
        }
    }

    // -1 itself
    return count
}

__kernel void game(__global const int *a, __global const int *b, __global int *c)
{
    int gid = get_global_id(0);

    int y = (int) gid / 4;
    int x = (int) gid % 4;
    int row_size = (int) get_local_size(0);

    int count = 0;
    if (a[gid - 1] == 1)
        count += 1;
    if (a[gid + 1] == 1)
        count += 1;

    c[gid] = (count*100) + (row_size*10) + a[gid] + b[gid];
}
