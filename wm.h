#ifndef WM_H
#define WM_H

#include <X11/Xlib.h>

typedef struct client
{
    struct client *prev;
    int wx, wy;
    unsigned int ww, wh;
    Window w;
} client;

void configure_request(XEvent *);
void map_request(XEvent *);
void destroy_notify(XEvent *);
int tile(bool map_request = false);

#endif