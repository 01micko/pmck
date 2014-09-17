#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int caja = 0; /* work around for MATE */
	
Window find_root(Display *dpy, int scr, int dpyWidth, int dpyHeight) {
	
	Window rootwin = RootWindow(dpy, scr);
	Window root_ret, root_kid_ret, root_grkid_ret;
	Window dad_ret, dad_kid_ret, dad_grkid_ret;
	Window *kids_ret, *kids_kids_ret, *kids_grkids_ret;
	unsigned int nkids_ret, nkids_nkids_ret, nkids_grnkids_ret;
	XWindowAttributes attrib;
	char *w_ret, *w_kids_ret, *w_grkids_ret;
	XQueryTree(dpy, rootwin, &root_ret, &dad_ret,
			&kids_ret, &nkids_ret);
	
	int i, j, k; /* plasma-desktop is 3 deep! (KDE) */
	int run = 1;
	
	for (i = 0; i < nkids_ret; i++)	{
		XGetWindowAttributes(dpy, kids_ret[i], &attrib);
		XFetchName(dpy, kids_ret[i], &w_ret);
	
		if (dpyWidth != attrib.width) {
			continue;
		}
		if (dpyHeight != attrib.height) {
			continue;
		}
		if (w_ret != NULL) {
			if (strcmp(w_ret, "x-caja-desktop") == 0) {
				caja = 1;
			} else {
				rootwin = kids_ret[i];
				run = 0;
				break;
			}
		}
		XQueryTree(dpy, kids_ret[i], &root_kid_ret, 
			&dad_kid_ret, &kids_kids_ret, &nkids_nkids_ret);
		for (j = 0; j < nkids_nkids_ret; j++) {
			XGetWindowAttributes(dpy, kids_kids_ret[j], &attrib);
			XFetchName(dpy, kids_kids_ret[j], &w_kids_ret);
		
			if (dpyWidth != attrib.width) {
				continue;
			}
			if (dpyHeight != attrib.height) {
				continue;
			}
			if (caja != 1) {
				if (w_kids_ret != NULL) {
					rootwin = kids_kids_ret[j];
					run = 0;						
					break;
				}
			}
			XQueryTree(dpy, kids_kids_ret[j], &root_grkid_ret, 
					&dad_grkid_ret, &kids_grkids_ret, 
					&nkids_grnkids_ret);
			for (k = 0; k < nkids_grnkids_ret; k++) {
				XGetWindowAttributes(dpy, 
						kids_grkids_ret[k], &attrib);
				XFetchName(dpy, kids_grkids_ret[k], &w_grkids_ret);
				if (dpyWidth != attrib.width) {
					continue;
				}
				if (dpyHeight != attrib.height) {
					continue;
				}
				if (caja == 1) {
					rootwin = kids_grkids_ret[k];
					run = 0;
					break;
				} else if (w_grkids_ret != NULL) {
					rootwin = kids_grkids_ret[k];
					run = 0;
					break;
				}
				XFree(w_grkids_ret);
			}
			if (run == 0) {
				break;
			} 
			XFree(w_kids_ret);
		}
		if (run == 0) {
			break;
		}
		XFree(w_ret);
	}
	return rootwin;
}
