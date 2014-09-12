#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	static char *window_id_format = "0x%lx\n";
	int i, j, k; /* plasma-desktop is 3 deep! (KDE) */
	const char *rox = "ROX";
	const char *desk = "Desktop"; /* xfce */
	const char *plasma = "plasma-desktop";
	
	for (i = 0; i < nkids_ret; i++)	{
		XGetWindowAttributes(dpy, kids_ret[i], &attrib);
		XFetchName(dpy, kids_ret[i], &w_ret);
		
		if ((dpyWidth != attrib.width) && 
				(dpyHeight != attrib.height)) {
			continue;
		}
		if (w_ret != NULL) {
			if (strncmp(rox,w_ret,3) == 0) {
				fprintf(stdout,window_id_format, kids_ret[i]);
				rootwin = kids_ret[i];
				break;
			}
		}
		XQueryTree(dpy, kids_ret[i], &root_kid_ret, 
				&dad_kid_ret, &kids_kids_ret, &nkids_nkids_ret);
		for (j = 0; j < nkids_nkids_ret; j++) {
			XGetWindowAttributes(dpy, kids_kids_ret[j], &attrib);
			XFetchName(dpy, kids_kids_ret[j], &w_kids_ret);
			
			if ((dpyWidth != attrib.width) && 
						(dpyHeight != attrib.height)) {
				continue;
			}
			if (w_kids_ret != NULL) {
				if ((strcmp(desk,w_kids_ret) == 0) || 
						(strncmp(rox,w_kids_ret,3) == 0) ||
						(strcmp(plasma,w_kids_ret) == 0)) {
					fprintf(stdout, window_id_format, 
								kids_kids_ret[j]);
					rootwin = kids_kids_ret[j];
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
				if ((dpyWidth != attrib.width) && 
						(dpyHeight != attrib.height)) {
					continue;
				}
				if ((w_grkids_ret != NULL) && (i <= 2)){
					if (strcmp(plasma,w_grkids_ret) == 0) {
						fprintf(stdout, window_id_format, 
									kids_grkids_ret[k]);
						rootwin = kids_grkids_ret[k];
						break;
					}
				}
				XFree(w_grkids_ret);
			} 
			XFree(w_kids_ret);
		}
		XFree(w_ret);
	}
	return rootwin;
}