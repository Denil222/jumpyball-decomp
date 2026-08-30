#ifndef JB_PLAYER_H
#define JB_PLAYER_H

/* JumpyBall.exe WndProc 0x0001fd2c: WM_CREATE arms SetTimer(hwnd, 10, 5, 0) and
   the WM_TIMER 0x113 body at 0x000208f8 runs input, physics and the camera. */

/* JumpyBall.exe .data: kAccelX 0x000262ac 7.0, g_jumpImpulseCur 0x000262b0 10.0,
   kJumpImpulse 0x000262b4 10.0, kRowPixels 0x000262bc 10, g_autoJump 0x00026430
   1, kGravity 0x00026434 20.0. */
#define JB_ACCEL_X      7.0f
#define JB_JUMP_IMPULSE 10.0f
#define JB_GRAVITY      20.0f

/* JumpyBall.exe WndProc 0x00020e5c reads the literal pool words [0x0021119c] =
   0x3fe28f5c and [0x000211a4] = 0x28f5c28f, the double 0.58 added to g_ballRow
   before __dtoi at 0x00020e80 takes the tile row. */
#define JB_ROW_PROBE_BIAS 0.58

/* JumpyBall.exe WndProc 0x00021094 scales g_ballVelZ * dt by 350.0 and wraps
   g_ballSpin 0x00064994 against 0x240. */
#define JB_SPIN_SCALE 350.0f
#define JB_SPIN_WRAP  0x240

typedef struct {
    float ball_x;        /* g_ballX 0x00064b0c */
    float ball_y;        /* g_ballY 0x00064b10 */
    float ball_row;      /* g_ballRow 0x00064b08 */
    float ball_prev_x;   /* g_ballPrevX 0x0006118c */
    float vel_x;         /* g_ballVelX 0x000649d4 */
    float vel_y;         /* g_ballVelY 0x000649d8 */
    float vel_z;         /* g_ballVelZ 0x000649cc */
    int   spin;          /* g_ballSpin 0x00064994 */
    int   cam_row;       /* g_camRow 0x000649b8 */
    float cam_pixel_ofs; /* g_camPixelOfs 0x00061a14 */

    int left_down;       /* g_leftDown 0x000649f8 */
    int right_down;      /* g_rightDown 0x000649fc */
    int up_down;         /* g_upDown 0x00064a08 */
    int down_down;       /* g_downDown 0x00064a0c */

    /* JumpyBall.exe WndProc LAB_000205a4 stores 0 on WM_KEYDOWN and
       LAB_000215cc stores 1 on WM_KEYUP. */
    int left_released;   /* DAT_00064a00 */
    int right_released;  /* DAT_00064a04 */
    int up_released;     /* DAT_00064a58 */
    int down_released;   /* DAT_00064a54 */

    int jump_buffered;   /* g_jumpBuffered 0x00064b00 */
    int fall_flag;       /* g_fallFlag 0x00064970 */

    /* JumpyBall.exe WndProc 0x0001fd2c calls hssSpeaker::playSound with
       g_sndBounce immediately before the store of 0.0 to g_ballY 0x00064b10. */
    int landed;

    int alt_track_mode;  /* g_altTrackMode 0x00064ab4 */

    /* JumpyBall.exe WndProc 0x00020b98 gates the g_ballVelZ decay on
       DAT_00064940, and 0x000214f0 gates the WM_KEYUP latch on it. */
    int z_axis_damped;

    int auto_jump;       /* g_autoJump 0x00026430 */
    int map_cols;        /* g_mapCols 0x0002628c */
    int map_rows;        /* g_mapRows 0x00026274 */

    /* JumpyBall.exe WndProc 0x00020f74 indexes (&DAT_00031180), which is
       g_tileGrid 0x00031178 + 8. */
    const unsigned char *tile_grid;
} jb_player_state;

/* JumpyBall.exe Player_Respawn 0x00012f18, the stores at 0x00012f2c..0x00013024. */
void Player_Respawn(jb_player_state *st);

/* JumpyBall.exe WndProc WM_KEYDOWN 0x00020390 gameplay branch and WM_KEYUP
   0x000214a0. */
void Player_KeyDown(jb_player_state *st, int key);
void Player_KeyUp(jb_player_state *st, int key);

/* JumpyBall.exe WndProc WM_TIMER 0x000208f8 body, dt in seconds
   (g_dtMs 0x00064968 * 0.001). */
void Player_Step(jb_player_state *st, float dt);

#endif /* JB_PLAYER_H */
