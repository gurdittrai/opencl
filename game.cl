// int count_neightbours(int *board, int y, int x)
// {
//     int count = 0;

//     int i, j;
//     for (i = y - 1; i < y + 2; i += 1)
//     {
//         // border
//         if ((i < 0) || (i > 4))
//             continue;

//         for (j = x - 1; j < x + 2; j += 1)
//         {
//             //border
//             if ((j < 0) || (j > 4))
//                 continue;

//             if (board[(i * 5) + j] == 1)
//                 count += 1;
//         }
//     }

//     // -1 itself
//     return count - board[(y * 5) + x];
// }

__kernel void game(__global const int *a, __global const int *b, __global int *c)
{
    int gid = get_global_id(0);

    int arr_size = get_global_size(0);
    int row_size = get_local_size(0);
    int count = 0;

    int i;
    for (i = 0; i < arr_size; i += 1)
    {
        count += a[i];
    }

    int y = (int)gid / row_size;
    int x = (int)gid % row_size;
    int bours = 0;
    // if (gid - 1 >= 0)
    // {
    //     bours += a[gid-1];
    // }

    // if (gid + 1 < row_size)
    // {
    //     bours += a[gid+1];
    // }

    // check row
    for (i = (gid - 1); i < (gid + 2); i += 1)
    {
        // border
        int lwr_lim = i < ((y) * row_size);
        int upp_lim = i > (((y+1) * row_size) - 1);
        if (lwr_lim || upp_lim)
            continue;
        
        bours += a[i];
    }

    if ((gid - row_size) >= 0)
    {
        bours += 10;
    }
    if ((gid + row_size) < arr_size)
    {
        bours += 10;
    }

    c[gid] = (bours * 100) + (count * 10) + a[gid];
}
