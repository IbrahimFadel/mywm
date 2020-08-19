#ifndef CONFIG_H
#define CONFIG_H

#include "wm.h"
#include <vector>

#define MOD Mod1Mask

const char *xtermcmd[] = {"xterm", NULL};

static std::vector<command> commands = {
    {MOD, XK_Return, spawn, {.com = xtermcmd}},
    {MOD, XK_q, kill, {NULL}}};

// static unsigned int Mod = 64; // Alt

#endif