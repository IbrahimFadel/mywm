#ifndef WM_H
#define WM_H

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>

static Window dummy_window;
static unsigned int dummy_uint1;
static unsigned int dummy_uint2;
#define win_size(W, gx, gy, gw, gh)                   \
    XGetGeometry(d, W, &dummy_window, gx, gy, gw, gh, \
                 &dummy_uint1, &dummy_uint2)

typedef struct client
{
    struct client *prev;
    int wx, wy;
    unsigned int ww, wh;
    Window w;
} client;

typedef union
{
    const char **com;
    const int i;
} Arg;

typedef struct command
{
    unsigned int mod;
    KeySym keysym;
    void (*function)(const Arg arg);
    const Arg arg;
} command;

void configure_request(XEvent *);
void map_request(XEvent *);
void destroy_notify(XEvent *);
void key_press(XEvent *);
void enter_notify(XEvent *);
void button_press(XEvent *);
void button_release(XEvent *);
void motion_notify(XEvent *);

void update_current(Window w);
void input_grab(Window root);
void spawn(const Arg arg);
void kill(const Arg arg);

#endif