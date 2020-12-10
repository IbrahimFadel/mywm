#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

#define MOD XCB_MOD_MASK_1
#define SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

typedef struct Key
{
    unsigned int mod;
    xcb_keysym_t keysym;
    void (*func)(const char **com);
    const char **com;
} Key;

typedef struct Client
{
    struct Client *prev, *next;
    xcb_window_t *win;
} Client;

static void init_wm();
static void init_input();

static void run(const char **com);
static void kill(const char **com);
static void focus_window(xcb_drawable_t window);
static Client *new_client(xcb_window_t *window);

static xcb_keysym_t keycode_to_keysym(xcb_keycode_t keycode);

static void configure_request(xcb_generic_event_t *e);
static void key_press(xcb_generic_event_t *e);
static void destroy_notify(xcb_generic_event_t *e);
static void button_press(xcb_generic_event_t *e);
static void button_release(xcb_generic_event_t *e);
static void create_notify(xcb_generic_event_t *e);
static void enter_notify(xcb_generic_event_t *e);
static void motion_notify(xcb_generic_event_t *e);