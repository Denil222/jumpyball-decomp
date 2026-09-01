#include "jb_touch_sdl2.h"

#include "jb_platform.h"

#define JB_TOUCH_FINGERS 8

typedef struct {
    SDL_FingerID id;
    int          used;
    int          key;
} jb_finger;

static SDL_Rect  jb_game;
static SDL_Rect  jb_pad[JB_KEY_COUNT];
static jb_finger jb_fingers[JB_TOUCH_FINGERS];
static int       jb_down[JB_KEY_COUNT];
static int       jb_win_w;
static int       jb_win_h;

static void SetRect(int key, int x, int y, int w, int h)
{
    jb_pad[key].x = x;
    jb_pad[key].y = y;
    jb_pad[key].w = w;
    jb_pad[key].h = h;
}

static int HitTest(int x, int y)
{
    int k;

    for (k = 0; k < JB_KEY_COUNT; k++) {
        if (x >= jb_pad[k].x && x < jb_pad[k].x + jb_pad[k].w &&
            y >= jb_pad[k].y && y < jb_pad[k].y + jb_pad[k].h)
            return k;
    }
    return -1;
}

void Touch_Layout(int win_w, int win_h, int game_w, int game_h)
{
    int pad_h, view_h, cell, margin, side;
    int gw, gh;

    if (win_w == jb_win_w && win_h == jb_win_h)
        return;
    jb_win_w = win_w;
    jb_win_h = win_h;

    pad_h = win_h / 3;
    if (pad_h < 96)
        pad_h = 96;
    if (pad_h > win_h / 2)
        pad_h = win_h / 2;
    view_h = win_h - pad_h;

    gw = win_w;
    gh = game_h * gw / game_w;
    if (gh > view_h) {
        gh = view_h;
        gw = game_w * gh / game_h;
    }
    jb_game.x = (win_w - gw) / 2;
    jb_game.y = (view_h - gh) / 2;
    jb_game.w = gw;
    jb_game.h = gh;

    cell   = pad_h / 3;
    margin = pad_h / 12;
    side   = cell + cell / 2;

    SetRect(JB_KEY_UP, margin + cell, view_h, cell, cell);
    SetRect(JB_KEY_LEFT, margin, view_h + cell, cell, cell);
    SetRect(JB_KEY_RIGHT, margin + 2 * cell, view_h + cell, cell, cell);
    SetRect(JB_KEY_DOWN, margin + cell, view_h + 2 * cell, cell, cell);
    SetRect(JB_KEY_JUMP, win_w - margin - side, win_h - margin - side, side,
            side);
    SetRect(JB_KEY_MENU, win_w - margin - cell, view_h + margin, cell, cell);
}

const SDL_Rect *Touch_GameRect(void)
{
    return &jb_game;
}

static void Rebuild(void)
{
    int i, k;

    for (k = 0; k < JB_KEY_COUNT; k++)
        jb_down[k] = 0;
    for (i = 0; i < JB_TOUCH_FINGERS; i++) {
        if (jb_fingers[i].used && jb_fingers[i].key >= 0)
            jb_down[jb_fingers[i].key] = 1;
    }
}

static jb_finger *Find(SDL_FingerID id)
{
    int i;

    for (i = 0; i < JB_TOUCH_FINGERS; i++) {
        if (jb_fingers[i].used && jb_fingers[i].id == id)
            return &jb_fingers[i];
    }
    return NULL;
}

void Touch_Handle(const SDL_Event *ev)
{
    jb_finger *f;
    int        i, x, y;

    if (jb_win_w == 0 || jb_win_h == 0)
        return;
    x = (int)(ev->tfinger.x * (float)jb_win_w);
    y = (int)(ev->tfinger.y * (float)jb_win_h);

    switch (ev->type) {
    case SDL_FINGERDOWN:
        for (i = 0; i < JB_TOUCH_FINGERS; i++) {
            if (!jb_fingers[i].used) {
                jb_fingers[i].used = 1;
                jb_fingers[i].id   = ev->tfinger.fingerId;
                jb_fingers[i].key  = HitTest(x, y);
                break;
            }
        }
        break;
    case SDL_FINGERMOTION:
        f = Find(ev->tfinger.fingerId);
        if (f != NULL)
            f->key = HitTest(x, y);
        break;
    case SDL_FINGERUP:
        f = Find(ev->tfinger.fingerId);
        if (f != NULL) {
            f->used = 0;
            f->key  = -1;
        }
        break;
    default:
        return;
    }
    Rebuild();
}

int Touch_Down(int key)
{
    if (key < 0 || key >= JB_KEY_COUNT)
        return 0;
    return jb_down[key];
}

static void Chevron(SDL_Renderer *ren, const SDL_Rect *r, int dx, int dy)
{
    SDL_Point p[3];
    int       cx = r->x + r->w / 2;
    int       cy = r->y + r->h / 2;
    int       a  = r->w / 5;

    p[0].x = cx - dy * a - dx * a / 2;
    p[0].y = cy - dx * a - dy * a / 2;
    p[1].x = cx + dx * a;
    p[1].y = cy + dy * a;
    p[2].x = cx + dy * a - dx * a / 2;
    p[2].y = cy + dx * a - dy * a / 2;
    SDL_RenderDrawLines(ren, p, 3);
}

static void Bars(SDL_Renderer *ren, const SDL_Rect *r)
{
    int i, x0, x1, y;

    x0 = r->x + r->w / 4;
    x1 = r->x + r->w - r->w / 4;
    for (i = 0; i < 3; i++) {
        y = r->y + r->h / 4 + i * r->h / 4;
        SDL_RenderDrawLine(ren, x0, y, x1, y);
    }
}

void Touch_Draw(SDL_Renderer *ren)
{
    static const int dir_x[JB_KEY_COUNT] = { -1, 1, 0, 0, 0, 0 };
    static const int dir_y[JB_KEY_COUNT] = { 0, 0, -1, 1, -1, 0 };
    int              k;

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    for (k = 0; k < JB_KEY_COUNT; k++) {
        if (jb_down[k])
            SDL_SetRenderDrawColor(ren, 0x60, 0x90, 0xd0, 0xd0);
        else
            SDL_SetRenderDrawColor(ren, 0x20, 0x20, 0x28, 0xa0);
        SDL_RenderFillRect(ren, &jb_pad[k]);
        SDL_SetRenderDrawColor(ren, 0xd0, 0xd0, 0xd8, 0xff);
        SDL_RenderDrawRect(ren, &jb_pad[k]);
        if (k == JB_KEY_MENU)
            Bars(ren, &jb_pad[k]);
        else
            Chevron(ren, &jb_pad[k], dir_x[k], dir_y[k]);
    }
}
