pmck
====

a poor man's clock

This is a simple analog clock based on xlib and cairo.

This is not meant for production systems as it has bugs and was written just as
a proof of concept.

Build
-----
Just run 'make' and install the pmck binary in your path. Optionally,
install the pmckrc file to $HOME/.config/ and edit to your preferences. 

Usage
-----
Just run 'pmck' from the command line, it needs no options. It will read an rc file 
in $HOME/.config/pmckrc to set different features. There is an example rc file
provided. This should be self explanatory. You can set the size, background
and foreground colours and the style, 2 to choose from. You can autostart it if
you wish. See your distro's documentation for this feature. 

On some window managers the root window is not the desktop window. In the case
of ROX-Filer pinboard this is easily overcome by finding the child window which
is "ROX-Pinboard", which in most cases is the very first child. However, in the
case of XFCE I had trouble coding the correct window so added 1 single option to the 
command line; the desktop window ID which can be found by running "xwininfo" and 
clicking on a blank part of the desktop. The string will be a hex value something 
like "0xae44010". Just type (or script) "pmck $hex_value_here". This should work 
with any other WM/DE also as it overrides the internal algorithm.

Bugs
----
- must quit with CTRL-C or kill, there is code block supposed to quit with 'q'.. :(
(fixed thanks to jamesbond)
- redraws forever, so may use too much CPU and eventually memory
- may run 1 or 2 seconds behind system time.
- need a click on desktop to display (rox bug - FIXED)
- some window managers may need the the window on top for the clock to display
- can flicker at times of heavy load or on slow machines
- a compositor such as xcompmgr is needed so artefacts do not display under clockface
- at larger sizes the second hand may flicker or disappear
