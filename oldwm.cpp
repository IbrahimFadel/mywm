#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XF86keysym.h>
#include <iostream>
#include <vector>
#include <unistd.h>

// #include "wm.h"
// #include "config.h"

using std::cout;
using std::endl;
using std::vector;

static Display *d;
static Window root;
static int sw, sh, wx, wy;
static unsigned int ww, wh, numlock = 0;
static vector<client *> windows;
static client *cur;
static XButtonEvent mouse;

static bool mod_down = false;

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
    win_size(w, &wx, &wy, &ww, &wh);
    client *c = new client();
    c->w = w;
    c->wx = sw / 2 - ww / 2;
    c->wy = sh / 2 - wh / 2;
    c->ww = ww;
    c->wh = wh;
    if (windows.size() > 0)
        c->prev = windows[windows.size() - 1];
    windows.push_back(c);
    cur = c;

    XMoveResizeWindow(d, w, c->wx, c->wy, c->ww, c->wh);
    XMapWindow(d, w);
    XSetInputFocus(d, cur->w, RevertToParent, CurrentTime);
    XRaiseWindow(d, cur->w);
}

void destroy_notify(XEvent *e)
{
    XDestroyWindowEvent *ev = &e->xdestroywindow;

    size_t i = 0;
    for (auto &c : windows)
    {
        if (c->w == ev->window)
        {
            windows.erase(windows.begin() + i);
            break;
        }
        i++;
    }
}

void key_press(XEvent *e)
{
    KeySym keysym = XkbKeycodeToKeysym(d, e->xkey.keycode, 0, 0);
    for (size_t i = 0; i < commands.size(); i++)
    {
        if (commands[i].keysym == keysym && commands[i].mod == e->xkey.state)
        {
            commands[i].function(commands[i].arg);
        }
    }
}

void spawn(const Arg arg)
{
    if (fork() == 0)
    {
        if (fork() == 0)
        {
            if (d)
                close(ConnectionNumber(d));

            setsid();
            execvp((char *)arg.com[0], (char **)arg.com);
        }
        exit(0);
    }
}

void kill(const Arg arg)
{
    size_t i = 0;
    for (auto &c : windows)
    {
        if (c->w == cur->w)
        {
            XKillClient(d, c->w);
            if (windows.size() > 1)
                cur = cur->prev;
            windows.erase(windows.begin() + i);
            break;
        }
        i++;
    }
}

void enter_notify(XEvent *e)
{
    update_current(e->xcrossing.window);
    // for (auto &c : windows)
    // {
    //     if (c->w == e->xcrossing.window)
    //     {
    //         cur = c;
    //         XRaiseWindow(d, c->w);
    //         XSetInputFocus(d, cur->w, RevertToParent, CurrentTime);
    //     }
    // }
}

void button_press(XEvent *e)
{
    if (!e->xbutton.subwindow)
        return;

    for (auto &c : windows)
    {
        if (c->w == e->xbutton.subwindow)
            cur = c;
    }
    // cur = e->xbutton.subwindow;

    if (e->xbutton.button != Button1)
        return;

    win_size(e->xbutton.subwindow, &wx, &wy, &ww, &wh);
    // XRaiseWindow(d, e->xbutton.subwindow);
    update_current(e->xbutton.subwindow);
    mouse = e->xbutton;
}

void button_release(XEvent *e)
{
    mouse.subwindow = 0;
}

void motion_notify(XEvent *e)
{
    if (!mouse.subwindow || mouse.button != Button1)
        return;

    int xd = e->xbutton.x_root - mouse.x_root;
    int yd = e->xbutton.y_root - mouse.y_root;

    XMoveResizeWindow(d, mouse.subwindow,
                      wx + xd,
                      wy + yd,
                      ww,
                      wh);
}

void update_current(Window w)
{
    for (auto &c : windows)
    {
        if (c->w == w)
        {
            XSetInputFocus(d, w, RevertToParent, CurrentTime);
            XRaiseWindow(d, w);
            cur = c;
        }
    }
}

void input_grab(Window root)
{
    unsigned int i, j, modifiers[] = {0, LockMask, numlock, numlock | LockMask};
    XModifierKeymap *modmap = XGetModifierMapping(d);
    KeyCode code;

    for (i = 0; i < 8; i++)
        for (int k = 0; k < modmap->max_keypermod; k++)
            if (modmap->modifiermap[i * modmap->max_keypermod + k] == XKeysymToKeycode(d, 0xff7f))
                numlock = (1 << i);

    for (i = 0; i < commands.size(); i++)
        if ((code = XKeysymToKeycode(d, commands[i].keysym)))
            for (j = 0; j < sizeof(modifiers) / sizeof(*modifiers); j++)
                XGrabKey(d, code, commands[i].mod | modifiers[j], root,
                         True, GrabModeAsync, GrabModeAsync);

    for (i = 1; i < 4; i += 2)
        for (j = 0; j < sizeof(modifiers) / sizeof(*modifiers); j++)
            XGrabButton(d, i, MOD | modifiers[j], root, True,
                        ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                        GrabModeAsync, GrabModeAsync, 0, 0);

    XFreeModifiermap(modmap);
}

int main()
{
    if (!(d = XOpenDisplay(0)))
        exit(1);

    int s = DefaultScreen(d);
    root = RootWindow(d, s);
    sw = XDisplayWidth(d, s);
    sh = XDisplayHeight(d, s);

    XSelectInput(d, root, SubstructureNotifyMask | SubstructureRedirectMask | KeyPressMask | EnterNotify);
    input_grab(root);

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
        case KeyPress:
            key_press(&e);
            break;
        case EnterNotify:
            enter_notify(&e);
            break;
        case ButtonPress:
            button_press(&e);
            break;
        case ButtonRelease:
            button_release(&e);
            break;
        case MotionNotify:
            motion_notify(&e);
            break;
        default:
            break;
        }
    }

    return 0;
}