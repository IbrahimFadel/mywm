#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

static const uint32_t border_size = 2;
static const uint32_t border_color = 0x458588;

static const char *termcmd[] = {"xterm", NULL};
static const char *dmenucmd[] = {"dmenu_run", NULL};

static Key keys[] = {
    {MOD, XK_Return, run, termcmd},
    {MOD, XK_space, run, dmenucmd},
    {MOD, XK_q, kill, NULL},
};

// const char *xtermcmd[] = {"xterm", 0};
// const char *dmenucmd[] = {"dmenu", 0};

// static struct key keys[] = {
//     {MOD, XK_Return, spawn, {.com = xtermcmd}},
//     {MOD, XK_space, spawn, {.com = dmenucmd}},
//     {MOD, XK_q, kill, {NULL}}};

// static std::vector<command> commands = {
//     {MOD, XK_Return, spawn, {.com = xtermcmd}},
//     {MOD, XK_q, kill, {NULL}}};

// static unsigned int Mod = 64; // Alt

#endif