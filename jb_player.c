#include "jb_player.h"

#include "jb_consts.h"
#include "jb_platform.h"

/* JumpyBall.exe Player_Respawn 0x00012f18, the stores at 0x00012f2c..0x00013024. */
void Player_Respawn(jb_player_state *st)
{
    st->fall_flag      = 0;
    st->alt_track_mode = 0;
    st->map_rows       = 0x400;
    st->ball_prev_x    = 0.0f;
    st->ball_y         = 5.0f;
    st->ball_x         = 0.0f;
    st->ball_row       = 0.0f;
    st->vel_y          = -1.0f;
    st->vel_x          = 0.0f;
    st->vel_z          = 0.0f;
    st->left_down      = 0;
    st->right_down     = 0;
    st->up_down        = 0;
    st->down_down      = 0;
    st->cam_row        = 0;
    st->cam_pixel_ofs  = 0.0f;
}

void Player_KeyDown(jb_player_state *st, int key)
{
    switch (key) {
    case JB_KEY_LEFT:
        st->left_down = 1;
        st->left_released = 0;
        break;
    case JB_KEY_RIGHT:
        st->right_down = 1;
        st->right_released = 0;
        break;
    case JB_KEY_UP:
        st->up_down = 1;
        st->up_released = 0;
        break;
    case JB_KEY_DOWN:
        st->down_down = 1;
        st->down_released = 0;
        break;
    /* JumpyBall.exe WndProc 0x000204a0 raises g_ballVelY to kJumpImpulse only
       while it reads exactly 0.0, otherwise it sets g_jumpBuffered 0x00064b00. */
    case JB_KEY_JUMP:
        if (st->vel_y == 0.0f)
            st->vel_y = JB_JUMP_IMPULSE;
        else
            st->jump_buffered = 1;
        break;
    default:
        break;
    }
}

void Player_KeyUp(jb_player_state *st, int key)
{
    switch (key) {
    case JB_KEY_LEFT:
        st->left_down = 0;
        st->left_released = 1;
        break;
    case JB_KEY_RIGHT:
        st->right_down = 0;
        st->right_released = 1;
        break;
    /* JumpyBall.exe WndProc 0x000214f0 and 0x00021518 return without touching
       the latch while DAT_00064940 is not 1. */
    case JB_KEY_UP:
        st->up_down = 0;
        if (st->z_axis_damped == 1)
            st->up_released = 1;
        break;
    case JB_KEY_DOWN:
        st->down_down = 0;
        if (st->z_axis_damped == 1)
            st->down_released = 1;
        break;
    default:
        break;
    }
}

/* JumpyBall.exe WndProc 0x00020fbc "cmp r3,r0" and 0x00020fe0 send a column
   index outside 1..g_mapCols to the same LAB_00020fa4 a zero tile byte reaches,
   and Level_LoadTileMap 0x0001261c fills 0x3fc rows of g_tileGrid 0x00031178. */
static int TileAt(const jb_player_state *st, int row, int col)
{
    if (col + 8 < 1 || st->map_cols < col + 8)
        return 0;
    if (row < 0 || row >= JB_GRID_ROWS)
        return 0;
    return st->tile_grid[8 + row * st->map_cols - col];
}

/* JumpyBall.exe WndProc 0x00020a1c..0x00021268, the WM_TIMER body between the
   g_fallFlag test and Game_DrawFrame 0x0001d288. */
static void StepMotion(jb_player_state *st, float dt)
{
    float accel = JB_ACCEL_X * dt;
    int   row, col;

    if (st->left_down != 0) {
        st->vel_x += accel * 2.0f;
    } else if (st->left_released != 0) {
        st->vel_x -= accel;
        if (st->vel_x <= 0.0f) {
            st->vel_x = 0.0f;
            st->left_released = 0;
        }
    }
    if (st->right_down != 0) {
        st->vel_x -= accel * 2.0f;
    } else if (st->right_released != 0) {
        st->vel_x += accel;
        if (st->vel_x >= 0.0f) {
            st->vel_x = 0.0f;
            st->right_released = 0;
        }
    }
    if (st->up_down != 0) {
        st->vel_z += accel;
    } else if (st->up_released != 0 && st->z_axis_damped == 1) {
        st->vel_z -= accel;
        if (st->vel_z <= 0.0f) {
            st->vel_z = 0.0f;
            st->up_released = 0;
        }
    }
    if (st->down_down != 0) {
        st->vel_z -= (st->vel_z >= 0.0f) ? accel * 2.0f : accel;
    } else if (st->down_released != 0 && st->z_axis_damped == 1) {
        st->vel_z += accel;
        if (st->vel_z >= 0.0f) {
            st->vel_z = 0.0f;
            st->down_released = 0;
        }
    }

    st->ball_row += st->vel_z * dt;
    st->ball_x   += st->vel_x * dt;
    st->ball_y   += st->vel_y * dt;

    /* JumpyBall.exe WndProc 0x00020ecc clamps g_ballX 0x00064b0c to
       (float)g_mapCols and 0x00020ef4 clamps it to -16.0. */
    if (st->ball_x > (float)st->map_cols)
        st->ball_x = (float)st->map_cols;
    if (st->ball_x <= -16.0f)
        st->ball_x = -16.0f;

    row = (int)((double)st->ball_row + JB_ROW_PROBE_BIAS);
    col = (int)(st->ball_prev_x + (st->ball_prev_x > 0.0f ? 1.0f : 0.0f));

    /* JumpyBall.exe WndProc 0x00020fc4 reaches the g_autoJump 0x00026430 test
       only when the probed tile byte is 0 or the column index is off the grid. */
    if (TileAt(st, row, col) == 0 && st->ball_y <= 0.0f) {
        if (st->auto_jump == 1 && st->ball_y == 0.0f)
            st->vel_y = JB_JUMP_IMPULSE;
        else
            st->fall_flag = 1;
    }

    /* JumpyBall.exe WndProc 0x00021028 "mov r2,#0x0; mov r3,#0x0" makes the
       __ltd at 0x0002103c a g_ballY < 0.0 test; 0x00021064 then stores 0 to
       g_ballY and g_ballVelY and 0x00021088 spends g_jumpBuffered. */
    if (st->ball_y < 0.0f) {
        st->landed = 1;
        st->ball_y = 0.0f;
        st->vel_y  = 0.0f;
        if (st->jump_buffered != 0) {
            st->jump_buffered = 0;
            st->vel_y = JB_JUMP_IMPULSE;
        }
    }

    st->ball_prev_x = st->ball_x;

    /* JumpyBall.exe WndProc 0x000211e8 "mov r2,#0x0; mov r3,#0x0" makes the
       __ned at 0x000211f8 a g_ballVelY != 0.0 test. */
    if (st->vel_y != 0.0f)
        st->vel_y -= JB_GRAVITY * dt;

    /* JumpyBall.exe WndProc 0x00021254 "mov r2,#0x0; mov r3,#0x0" makes the
       __ltd that follows a g_ballRow < 0.0 test. */
    if (st->ball_row < 0.0f) {
        st->ball_row = 0.0f;
        st->vel_z    = 0.0f;
    }

    st->cam_row = (int)st->ball_row;
    st->cam_pixel_ofs = (st->ball_row - (float)st->cam_row) * (float)JB_ROW_PIXELS;
}

/* JumpyBall.exe WndProc 0x00021094 wraps g_ballSpin 0x00064994 into 1..0x240
   with __rt_udiv, downwards at 0x000210cc and upwards at 0x000210f4. */
static void StepSpin(jb_player_state *st, float dt)
{
    int spin = (int)(st->vel_z * dt * JB_SPIN_SCALE + (float)st->spin);

    if (spin > JB_SPIN_WRAP) {
        unsigned q = (unsigned)(spin - (JB_SPIN_WRAP + 1)) / (unsigned)JB_SPIN_WRAP;

        spin -= (int)(q + 1u) * JB_SPIN_WRAP;
    }
    st->spin = spin;
    if (spin < 0) {
        unsigned q = (unsigned)(-spin - 1) / (unsigned)JB_SPIN_WRAP;

        st->spin = (int)(q + 1u) * JB_SPIN_WRAP + spin;
    }
}

void Player_Step(jb_player_state *st, float dt)
{
    /* JumpyBall.exe WndProc 0x000209e4 drives the fall with g_ballVelY -11.0
       and respawns below -20.0. */
    if (st->fall_flag == 1) {
        st->vel_y = -11.0f;
        st->ball_y -= dt * 11.0f;
        if (st->ball_y < -20.0f)
            Player_Respawn(st);
    }
    if (st->alt_track_mode == 0 && st->fall_flag != 1)
        StepMotion(st, dt);
    StepSpin(st, dt);
}
