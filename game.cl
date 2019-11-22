__kernel game_simulation(__global int *board_curr, __global int *board_next)
{
    uint global_addr;
    global_addr = get_global_id(0);

    if (board_curr[global_addr] % 2)
        board_next[global_addr] = board_curr[global_addr] * 2;
}