#include <X11/Xlib.h>
#include <iostream>
#include <vector>

#include "wm.h"

using std::cout;
using std::endl;
using std::vector;

static Display *d;
static Window root;
static int sw, sh;
static vector<client *> windows;

static unsigned int outer_gaps = 20;

void configure_request(XEvent *e)
{
    XConfigureRequestEvent *ev = &e->xconfigurerequest;
    XWindowChanges xwc;
    xwc.x = ev->x;
    xwc.y = ev->y;
    xwc.width = ev->width;
    xwc.height = ev->height;
    xwc.sibling = ev->above;
    xwc.stack_mode = ev->detail;

    XConfigureWindow(d, ev->window, ev->value_mask, &xwc);
}

void map_request(XEvent *e)
{
    Window w = e->xmaprequest.window;
    XSelectInput(d, w, StructureNotifyMask | EnterWindowMask);

    client *c = new client();
    c->w = w;

    if (windows.size() == 0)
    {
        c->wx = outer_gaps;
        c->wy = outer_gaps;
        c->ww = sw - 2 * outer_gaps;
        c->wh = sh - 2 * outer_gaps;
    }
    else
    {
        c->prev = windows[windows.size() - 1];

        int wx = tile(true);

        c->wx = wx;
        c->wy = outer_gaps;
        c->ww = (sw - 2 * outer_gaps) / (windows.size() + 1);
        c->wh = sh - 2 * outer_gaps;
    }
    windows.push_back(c);

    XMoveResizeWindow(d, w, c->wx, c->wy, c->ww, c->wh);

    XMapWindow(d, w);
}

void destroy_notify(XEvent *e)
{
    XDestroyWindowEvent *ev = &e->xdestroywindow;

    size_t i = 0;
    for (auto &win : windows)
    {
        if (win->w == ev->window)
        {
            windows.erase(windows.begin() + i);
        }

        i++;
    }

    tile();
}

int tile(bool map_request)
{
    int size = windows.size();
    if (map_request)
        size++;
    int wx = outer_gaps;
    for (auto &win : windows)
    {
        win->ww = (sw - 2 * outer_gaps) / size;
        XMoveResizeWindow(d, win->w, wx, win->wy, win->ww, win->wh);
        wx += win->ww;
    }

    return wx;
}

int main()
{
    if (!(d = XOpenDisplay(0)))
        exit(1);

    int s = DefaultScreen(d);
    root = RootWindow(d, s);
    sw = XDisplayWidth(d, s);
    sh = XDisplayHeight(d, s);

    XSelectInput(d, root, SubstructureRedirectMask);

    XEvent e;

    while (1 && !XNextEvent(d, &e))
    {
        switch (e.type)
        {
        case MapRequest:
            map_request(&e);
            break;
        case ConfigureRequest:
            configure_request(&e);
            break;
        case DestroyNotify:
            destroy_notify(&e);
            break;
        default:
            break;
        }
    }

    return 0;
}