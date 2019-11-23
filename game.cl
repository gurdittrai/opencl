int count_neighbours(__global const int *board, int gid, int row_size, int arr_size)
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

int update_value(int value, int count)
{
    // rule 1, filled cell 2 or 3 count
    if ((value == 1) && ((count == 2) || (count == 3)))
    {
        return 1;
    }
    // rule 2, fill empty cell 3 count
    else if ((value == 0) && (count == 3))
    {
        return 1;
    }

    // rule 3, empty cell
    return 0;
}

__kernel void game(__global const int *a, __global int *b)
{
    // get variable info
    int gid = get_global_id(0);
    int arr_size = get_global_size(0);
    int row_size = get_local_size(0);

    // get neighbours
    int count = 0;
    count = count_neighbours(a, gid, row_size, arr_size) - a[gid];

    // updated value on new board
    b[gid] = (count*100) + update_value(a[gid], count);
}
