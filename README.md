pmck
====

a poor man's clock

This is a simple analog clock for Linux/UNIX based on xlib and cairo.


Build
-----
run:

```
./configure
```

then:

```
make
```

and as root:

```
make install
```

Some basic options to `configure` are provided. Run `configure --help`

You will have to manually move `pmckrc` to `$XDG_CONFIG_HOME` for it to be recognised.

Usage
-----
Just run `pmck` from the command line, it needs no options. It will read an rc file 
in `~/.config/pmckrc` to set different features. There is an example rc file
provided. This should be self explanatory. You can set the size, background
and foreground colours and the style, 4 to choose from, including plain numbers(0 and default),
Roman numerals (1), markings only (2) or even the classic Swiss SBB face (3). 
You can autostart it if you wish. See your distro's documentation for this feature. 

On some window managers the root window is not the desktop window. In the case
of ROX-Filer pinboard this is easily overcome by finding the child window which
is "ROX-Pinboard", which in most cases is the very first child. To address this 
limitation I have added code to find the true *desktop*. `pmck` now comes
with a header file `pmdesktop.h` with the funtion in `pmdesktop.c`. In the future
a shared library named `libpmdesktop.so` with it's static counterpart `libpmdesktop.a`
will become available. As time permits I will utilise this library for other projects.
So far this is tested with *JWM*, *JWM + ROX-Filer*, *Fluxbox*, *XFCE*, 
*WindowMaker* and *KDE*.

If finding the desktop fails it can be overcome by 1 single option to the 
command line; the desktop window ID which can be found by running `xwininfo` and 
clicking on a blank part of the desktop. The string will be a hex value something 
like `0x400066`. Just type (or script) `pmck $hex_value_here`. This should work 
with any other WM/DE also as it overrides the internal algorithm. 

Extra notes
-----------
`pmck` now comes with an optional library. It is built 
**statically** by default, all extra code is included in `pmdesktop.h`. 

The library, [pmdesktop](https://github.com/01micko/pmdesktop) is
available, with the intent of future desktop programs to be developed.
* disclaimer: **statically** only refers to `libpmdesktop`. All other depends are dynamically linked

Bugs
----
- may run 1 or 2 seconds behind system time and skip a second to catch up
- can flicker at times of heavy load or on slow machines
- may have jagged edges which may or may not be improved with an antialias setting in pmckrc.
- may be slow to refresh after being uncovered by another window (less than 1 second)

Please report all bugs here for now.
