__kernel void game(__global const int *a, __global const int *b, __global int *c)
{
    int gid = get_global_id(0);
    c[gid] = get_global_id(0);
}
