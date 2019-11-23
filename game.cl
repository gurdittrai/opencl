int count_neightbours(__global const int *board, int gid, int row_size, int arr_size)
{
    int i, j;
    int count = 0;
    // check rows
    for (i = gid - row_size; i < gid + (2*row_size) ; i += row_size)
    {
        // border
        if ((i < 0) || (i >= arr_size))
        {
            continue;
        }

        // check columns
        for (j = (i - 1); j < (i + 2); j += 1)
        {
            // border
            int y = (int) i / row_size;
            int lwr_lim = j < ((y) * row_size);
            int upp_lim = j > (((y+1) * row_size) - 1);
            if (lwr_lim || upp_lim)
                continue;
            
            count += board[j];
        }
    }
    return count;
}

__kernel void game(__global const int *a, __global const int *b, __global int *c)
{
    int gid = get_global_id(0);

    int arr_size = get_global_size(0);
    int row_size = get_local_size(0);
    int count = 0;
    int bours = 0;

    int i, j;
    for (i = 0; i < arr_size; i += 1)
    {
        // count += a[i];
    }
    
    
    for (i = gid - row_size; i < gid + (2*row_size) ; i += row_size)
    {
        if ((i < 0) || (i >= arr_size))
        {
            continue;
        }
        for (j = (i - 1); j < (i + 2); j += 1)
        {
            // border
            int y = (int) i / row_size;
            int lwr_lim = j < ((y) * row_size);
            int upp_lim = j > (((y+1) * row_size) - 1);
            if (lwr_lim || upp_lim)
                continue;
            
            bours += a[j];
        }
    }

    count = count_neightbours(a, gid, row_size, arr_size);

    c[gid] = (bours * 100) + (count * 10) + a[gid];
}
