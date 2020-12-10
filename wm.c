#include "wm.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static xcb_connection_t *dpy;
static xcb_screen_t *scr;
// static xcb_window_t cur;
static Client *cur;
static Client *clients;

static void (*events[XCB_NO_OPERATION])(xcb_generic_event_t *) = {
    [XCB_BUTTON_PRESS] = button_press,
    [XCB_BUTTON_RELEASE] = button_release,
    [XCB_CONFIGURE_REQUEST] = configure_request,
    [XCB_KEY_PRESS] = key_press,
    [XCB_CREATE_NOTIFY] = create_notify,
    [XCB_DESTROY_NOTIFY] = destroy_notify,
    [XCB_ENTER_NOTIFY] = enter_notify,
    [XCB_MOTION_NOTIFY] = motion_notify};

xcb_keysym_t keycode_to_keysym(xcb_keycode_t keycode)
{
    xcb_key_symbols_t *keysyms = xcb_key_symbols_alloc(dpy);
    xcb_keysym_t keysym;
    keysym = (!(keysyms) ? 0 : xcb_key_symbols_get_keysym(keysyms, keycode, 0));
    xcb_key_symbols_free(keysyms);
    return keysym;
}

void focus_window(xcb_drawable_t window)
{
    if ((window != 0) && (window != scr->root))
    {
        xcb_set_input_focus(dpy, XCB_INPUT_FOCUS_POINTER_ROOT, window,
                            XCB_CURRENT_TIME);
    }
}

void configure_request(xcb_generic_event_t *e)
{
    printf("Configure\n");
}

void key_press(xcb_generic_event_t *e)
{
    printf("Key Press\n");

    xcb_key_press_event_t *ev = (xcb_key_press_event_t *)e;
    xcb_keycode_t keycode = ev->detail;
    xcb_keysym_t keysym = keycode_to_keysym(keycode);

    for (int i = 0; i < SIZE(keys); i++)
    {
        if (keysym == keys[i].keysym)
        {
            keys[i].func(keys[i].com);
        }
    }
}

void destroy_notify(xcb_generic_event_t *e)
{
    printf("Destroy\n");
}

void button_press(xcb_generic_event_t *e)
{
    printf("Button Press\n");
}

void button_release(xcb_generic_event_t *e)
{
    printf("Button Release\n");
}

Client *new_client(xcb_window_t *window)
{
    Client *c = malloc(sizeof(Client));
    c->win = window;

    if (clients)
    {
        printf("Clients exists\n");
        clients->prev->next = c;
        c->prev = clients->prev;
        clients->prev = c;
        c->next = clients;
    }
    else
    {
        printf("Clients dont exist\n");
        clients = c;
        clients->prev = clients->next = clients;
    }

    return c;
}

void create_notify(xcb_generic_event_t *e)
{
    printf("Create\n");

    xcb_create_notify_event_t *ev = (xcb_create_notify_event_t *)e;
    uint32_t value;

    value = XCB_EVENT_MASK_ENTER_WINDOW |
            XCB_EVENT_MASK_FOCUS_CHANGE |
            XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
            XCB_EVENT_MASK_BUTTON_PRESS |
            XCB_EVENT_MASK_BUTTON_RELEASE |
            XCB_EVENT_MASK_BUTTON_MOTION;

    xcb_change_window_attributes(dpy, ev->window, XCB_CW_EVENT_MASK, &value);

    xcb_change_window_attributes(dpy, ev->window, XCB_CW_BORDER_PIXEL | XCB_CONFIG_WINDOW_BORDER_WIDTH, &border_color);
    xcb_change_window_attributes(dpy, ev->window, XCB_CONFIG_WINDOW_BORDER_WIDTH, &border_size);
    xcb_map_window(dpy, ev->window);

    focus_window(ev->window);

    Client *c = new_client(&ev->window);
    cur = c;
    // printf("Current Client: %p\n", (uintptr_t)cur);
    // printf("Client window: %p\n", (uintptr_t)cur->win);
    // cur = ev->window;
}

void enter_notify(xcb_generic_event_t *e)
{
    printf("Enter\n");
}

void motion_notify(xcb_generic_event_t *e)
{
    printf("Motion\n");
}

void run(const char **com)
{
    if (fork() == 0)
    {
        if (dpy != NULL)
        {
            close(scr->root);
        }
        setsid();
        if (fork() != 0)
        {
            _exit(0);
        }
        execvp((char *)com[0], (char **)com);
        _exit(0);
    }
}

void kill(const char **com)
{
    printf("kill\n");
    if (cur->prev)
    {
        printf("there is a prev\n");
    }
    // printf("Kill Current Client: %p\n", cur);
    // printf("Kill Clgsient window: %p\n", cur->win);
    xcb_kill_client(dpy, *cur->win);
}

void init_wm()
{
    uint32_t values;
    dpy = xcb_connect(NULL, NULL);

    if (xcb_connection_has_error(dpy))
    {
        printf("Could not establish connection\n");
        exit(1);
    }

    scr = xcb_setup_roots_iterator(xcb_get_setup(dpy)).data;

    if (!scr)
    {
        printf("Could not assign screen\n");
        xcb_disconnect(dpy);
        exit(1);
    }

    values = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
    xcb_change_window_attributes_checked(dpy, scr->root, XCB_CW_EVENT_MASK, &values);

    xcb_flush(dpy);
}

void init_input()
{
    xcb_grab_key(dpy, 1, scr->root, MOD, XCB_NO_SYMBOL, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
    xcb_grab_button(dpy, 0, scr->root, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, scr->root, XCB_NONE, 1, MOD);
    xcb_grab_button(dpy, 0, scr->root, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, scr->root, XCB_NONE, 3, MOD);

    xcb_flush(dpy);
}

int main()
{
    xcb_generic_event_t *e;
    unsigned int e_type;

    init_wm();
    init_input();

    while ((e = xcb_wait_for_event(dpy)))
    {
        e_type = e->response_type & ~0x80;

        if (events[e_type])
        {
            events[e_type](e);
            xcb_flush(dpy);
        }

        free(e);
    }

    xcb_disconnect(dpy);
    return 0;
}