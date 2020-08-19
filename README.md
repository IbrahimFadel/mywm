# mywm
A small floating window manager


## Development

Launch a nested X server
```
Xephyr -br -ac -noreset -screen 800x600 :1
```

Run the window manager
```
make && DISPLAY=:1 ./wm
```

Launch programs
```
DISPLAY=:1 xterm
```
