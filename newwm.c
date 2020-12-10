
// #include "wm.h"
// #include "config.h"

// static Display *d;
// static int s;
// static Window root;
// static int sw, sh;
// static unsigned int ww = 500, wh = 400;
// static int numlock = 0;
// static XButtonEvent mouse;

// static void (*events[LASTEvent])(XEvent *e) = {
//     [ConfigureRequest] = configure_request,
//     [MapRequest] = map_request,
//     [EnterNotify] = enter_notify,
//     [KeyPress] = key_press,
//     [ButtonPress] = button_press,
//     [MapNotify] = map_notify,
//     [DestroyNotify] = destroy_notify,
// };

// static void destroy_notify(XEvent *e)
// {
// }

// static void map_notify(XEvent *e)
// {
//     printf("map_notif\n");
//     XMappingEvent *ev = &e->xmapping;

//     if (ev->request == MappingKeyboard || ev->request == MappingModifier)
//     {
//         XRefreshKeyboardMapping(ev);
//         input_grab(root);
//     }
// }

// void new_window(Window *w)
// {
//     client *c = malloc(sizeof(client));
//     if (!c)
//         exit(1);

//     c->win = w;

//     if (clients)
//     {
//         clients->prev->next = c;
//         c->prev = clients->prev;
//         clients->prev = c;
//         c->next = clients;
//     }
//     else
//     {
//         clients = c;
//         clients->prev = clients->next = clients;
//     }

//     cur = c;
// }

// void button_press(XEvent *e)
// {
//     if (!e->xbutton.subwindow)
//         return;

//     // win_size(e->xbutton.subwindow, &wx, &wy, &ww, &wh);
//     XRaiseWindow(d, e->xbutton.subwindow);
//     mouse = e->xbutton;
// }

// void map_request(XEvent *e)
// {
//     Window w = e->xmaprequest.window;
//     int x = sw / 2 - ww / 2, y = sh / 2 - wh / 2;
//     XMoveResizeWindow(d, w, x, y, ww, wh);
//     new_window(&w);
//     XSelectInput(d, w, StructureNotifyMask | SubstructureRedirectMask | ResizeRedirectMask | KeyPressMask | KeyReleaseMask);
//     XMapWindow(d, w);
//     XSync(d, 0);
//     input_grab(w);
//     XSetInputFocus(d, w, RevertToParent, CurrentTime);
// }

// void configure_request(XEvent *e)
// {
//     XConfigureRequestEvent *ev = &e->xconfigurerequest;
//     XConfigureWindow(d, ev->window, ev->value_mask, &(XWindowChanges){.x = ev->x, .y = ev->y, .width = ev->width, .height = ev->height, .sibling = ev->above, .stack_mode = ev->detail});
// }

// void enter_notify(XEvent *e)
// {
//     XRaiseWindow(d, e->xcrossing.window);
// }

// void key_press(XEvent *e)
// {
//     KeySym keysym = XkbKeycodeToKeysym(d, e->xkey.keycode, 0, 0);

//     for (unsigned int i = 0; i < sizeof(keys) / sizeof(*keys); ++i)
//         if (keys[i].keysym == keysym)
//             keys[i].function(keys[i].arg);
// }

// void kill(const Arg arg)
// {
//     printf("kill\n");
//     printf("cur: %d", cur);
//     printf("win: %d", cur->win);
//     XMoveResizeWindow(d, *cur->win, 0, 0, ww, wh);
//     // if (cur)
//     // {
//     //     printf("cur: %d", cur);
//     //     printf("win: %d", cur->win);
//     //     // XKillClient(d, *cur->win);
//     //     XDestroyWindow(d, *cur->win);
//     // }
// }

// void spawn(const Arg arg)
// {
//     if (fork())
//         return;
//     if (d)
//         close(ConnectionNumber(d));

//     setsid();
//     execvp((char *)arg.com[0], (char **)arg.com);
// }

// void input_grab(Window root)
// {
//     unsigned int i, j, modifiers[] = {0, LockMask, numlock, numlock | LockMask};
//     XModifierKeymap *modmap = XGetModifierMapping(d);
//     KeyCode code;

//     for (i = 0; i < 8; i++)
//         for (int k = 0; k < modmap->max_keypermod; k++)
//             if (modmap->modifiermap[i * modmap->max_keypermod + k] == XKeysymToKeycode(d, 0xff7f))
//                 numlock = (1 << i);

//     XUngrabKey(d, AnyKey, AnyModifier, root);

//     for (i = 0; i < sizeof(keys) / sizeof(*keys); i++)
//         if ((code = XKeysymToKeycode(d, keys[i].keysym)))
//             for (j = 0; j < sizeof(modifiers) / sizeof(*modifiers); j++)
//                 XGrabKey(d, code, keys[i].mod | modifiers[j], root,
//                          True, GrabModeAsync, GrabModeAsync);

//     for (i = 1; i < 4; i += 2)
//         for (j = 0; j < sizeof(modifiers) / sizeof(*modifiers); j++)
//             XGrabButton(d, i, MOD | modifiers[j], root, True,
//                         ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
//                         GrabModeAsync, GrabModeAsync, 0, 0);

//     XFreeModifiermap(modmap);
// }

// int main()
// {
//     XEvent e;

//     d = XOpenDisplay(0);
//     if (!d)
//         exit(1);

//     s = DefaultScreen(d);
//     root = RootWindow(d, s);
//     sw = XDisplayWidth(d, s);
//     sh = XDisplayHeight(d, s);

//     printf("W: %d, H: %d\n", sw, sh);

//     XSelectInput(d, root, SubstructureNotifyMask | SubstructureRedirectMask | KeyPressMask | EnterNotify | MapNotify);
//     XDefineCursor(d, root, XCreateFontCursor(d, 68));
//     input_grab(root);

//     while (1 && !XNextEvent(d, &e)) // 1 && will forever be here.
//     {
//         printf("Ev: %d\n", e.type);
//         if (events[e.type])
//             events[e.type](&e);
//     }

//     return 0;
// }