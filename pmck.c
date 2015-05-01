#include <cairo.h>
#include <cairo-xlib.h>
#include <cairo-xlib-xrender.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/shape.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#ifdef _SHARED_BUILD
	#include <pmdesktop.h>
#else
	#include "pmdesktop.h"
#endif /* _SHARED_BUILD */

#define THIS_VERSION "0.6"
#define _GNU_SOURCE
#define CONFIG ".config/pmckrc"

const char *prog;
int secs;
int mins;
int hrs;
/** angles */
float secd;
float mind;
float hrd;

char *home;
char config[128];
FILE *fp;
int size;
int posx, posy;
int style;
int deco;

/** default colours */
/*background*/
double bg_r = 0.3;
double bg_g = 0.3;
double bg_b = 0.3;
/*background decorations*/
double bd_r = 0.7;
double bd_g = 0.7;
double bd_b = 0.7;
/*big and little hand*/
double blh_r = 0.8;
double blh_g = 0.8;
double blh_b = 0.8;
/*second hand*/
double sh_r = 1.0;
double sh_g = 0.0;
double sh_b = 0.0;
int AA = 0; /* CAIRO_ANTIALIAS_DEFAULT */

static int test_conf() {
	home = getenv("HOME");
	config[128] = sprintf(config,"%s/%s", home, CONFIG);
	fp = fopen(config, "r");
	if ( fp == NULL ) {
		return 1;
	}
	fclose(fp);
	return 0;
}
char *split_string(char *var) {
	char *buf = var;
	const char s[2] = "=";
	char *token;
	token = strtok(buf, s);
	token = strtok(NULL, s);
	return token;
}
void read_conf() {
	/* parse the config file */
	home = getenv("HOME");
	config[128] = sprintf(config,"%s/%s", home, CONFIG);
	char line[30];
	fp = fopen(config, "r");
	if ( fp != NULL ) {
		while (fgets(line, sizeof(line), fp)) {
			if (strstr(line, "#") != NULL) {
				continue; /* dodge comments */
			}
			else if (strstr(line, "style") != NULL) {
				char *res = split_string(line);
				style = atoi(res);
			}
			else if (strstr(line, "deco") != NULL) {
				char *res = split_string(line);
				deco = atoi(res);
			}
			else if (strstr(line, "size") != NULL) {
				char *res = split_string(line);
				size = atoi(res);
			}
			else if (strstr(line, "bg_r") != NULL) {
				char *res = split_string(line);
				bg_r = atof(res);
			}
			else if (strstr(line, "bg_g") != NULL) {
				char *res = split_string(line);
				bg_g = atof(res);
			}
			else if (strstr(line, "bg_b") != NULL) {
				char *res = split_string(line);
				bg_b = atof(res);
			}
			else if (strstr(line, "bd_r") != NULL) {
				char *res = split_string(line);
				bd_r = atof(res);
			}
			else if (strstr(line, "bd_g") != NULL) {
				char *res = split_string(line);
				bd_g = atof(res);
			}
			else if (strstr(line, "bd_b") != NULL) {
				char *res = split_string(line);
				bd_b = atof(res);
			}
			else if (strstr(line, "blh_r") != NULL) {
				char *res = split_string(line);
				blh_r = atof(res);
			}
			else if (strstr(line, "blh_g") != NULL) {
				char *res = split_string(line);
				blh_g = atof(res);
			}
			else if (strstr(line, "blh_b") != NULL) {
				char *res = split_string(line);
				blh_b = atof(res);
			}
			else if (strstr(line, "sh_r") != NULL) {
				char *res = split_string(line);
				sh_r = atof(res);
			}
			else if (strstr(line, "sh_g") != NULL) {
				char *res = split_string(line);
				sh_g = atof(res);
			}
			else if (strstr(line, "sh_b") != NULL) {
				char *res = split_string(line);
				sh_b = atof(res);
			}
			else if (strstr(line, "posx") != NULL) {
				char *res = split_string(line);
				posx = atoi(res);
			}
			else if (strstr(line, "posy") != NULL) {
				char *res = split_string(line);
				posy = atoi(res);
			}
			else if (strstr(line, "AA") != NULL) {
				char *res = split_string(line);
				AA = atoi(res);
			}
		}
	} else {
		exit(1);
	} 
	fclose(fp);
}
/** build the clock hand structure */
struct clock_hand {
    int lgth;
    double wdth;
    float rr, gg, bb;
    int angle;
};
struct clock_hand *clock_hand_create(int lgth, double wdth, 
				float rr, float gg, float bb, int angle) {
    struct clock_hand *what = malloc(sizeof(struct clock_hand));
    assert(what != NULL);
	what->lgth = lgth;
    what->wdth = wdth;
    what->rr = rr;
	what->gg = gg;
	what->bb = bb;
	what->angle = angle;

    return what;
}
void clock_hand_destroy(struct clock_hand *what) {
    assert(what != NULL);
	free(what);
}
void get_time() {
	struct tm *now;
	time_t t; 
	time(&t);
	now = localtime(&t);
	secs = now->tm_sec;
	mins = now->tm_min;
	hrs = now->tm_hour;
}
/** return some angles */
float get_secs() { 
	get_time();
	while (1) {
		if ((secs != 0) && (secs <= 59)){
			secd = (secs * 6.0) + 180.0;
		} else if ((secs == 60) || (secs == 61)){ /* leap seconds */
			secd = (59 * 6.0) + 180.0;
		} else if (secs == 0) { 
			secd = 180.0;
		} else {
			fprintf(stderr, "A sec error occured.\n");
			exit(1);
		}
		break;
	}
	return secd;
}
float get_mins() {
	get_time();
	while (1) {
		if ((mins != 0) && (mins <= 59)){
			mind = (mins * 6.0) + 180.0;
		} else if (mins == 0) { 
			mind = 180.0;
		} else {
			fprintf(stderr, "A min error occured.\n");
			exit(1);
		}
		break;
	}
	return mind;
}
float get_hrs() {
	get_time();
	float diff = 30.0; /* the angle on the full hour*/
	if (hrs >= 12) {
		hrs = hrs - 12;
	}
	while (1) {
		if (mins < 12) {
			hrd = hrs * diff;
		} else if ((mins >= 12) && (mins < 24)) {
			hrd = (hrs * diff) + 6.0;
		} else if ((mins >= 24) && (mins < 36)) {
			hrd = (hrs * diff) + 12.0;
		} else if ((mins >= 36) && (mins < 48)) {
			hrd = (hrs * diff) + 18.0;
		} else if (mins >= 48){
			hrd = (hrs * diff) + 24.0;
		} else {
			fprintf(stderr, "An hour error occured.\n");
			exit(1);
		}
		break;
	}
	if (hrd >= 180) {
		hrd -= 180;
	} else {
		hrd += 180;
	}
	return hrd;
}
/** our angles are in degrees - need radians */
double degrees2radians(int degrees) {
    return((double)((double)degrees * ( M_PI/180 )));
}
void paint_second_hand(cairo_t *cr, int w, int h, int deco) { 
	/** second */
	get_secs();
	struct clock_hand *second = clock_hand_create((92 * w/200), 
										1.0, sh_r, sh_g, sh_b, secd);
	double degs = degrees2radians(second->angle);
	cairo_set_source_rgb(cr, second->rr, second->gg, second->bb);
	if (deco == 3) {
		cairo_set_line_width(cr, second->wdth);
		cairo_rotate(cr, degs); /* angle */
		cairo_move_to(cr, 0, -(second->lgth / 3));
		cairo_line_to(cr, 0, (2 * second->lgth / 3));
		cairo_stroke(cr);
		cairo_translate(cr, 0, (2 * second->lgth / 3));
		cairo_arc(cr, 0, 0, (second->lgth / 10), 0, 2 * M_PI);
		cairo_fill(cr);
	} else if (deco == 1) {
		cairo_set_line_width(cr, second->wdth);
		cairo_rotate(cr, degs); /* angle */
		cairo_move_to(cr, 0, -(second->lgth / 3));
		cairo_line_to(cr, 0, second->lgth);
		cairo_move_to(cr, 0, (second->lgth / 3));
		cairo_line_to(cr, second->lgth / 32, ((second->lgth / 3) + (second->lgth / 8)));
		cairo_line_to(cr,   0, ((2 * second->lgth / 3) + second->lgth / 8));
		cairo_line_to(cr, -(second->lgth / 32), ((second->lgth / 3) + (second->lgth / 8)));
		cairo_line_to(cr, 0, (second->lgth / 3));
		cairo_close_path(cr);
		cairo_stroke_preserve(cr);
	} else {
		cairo_set_line_width(cr, second->wdth);
		cairo_rotate(cr, degs); /* angle */
		cairo_move_to(cr, 0, -(second->lgth / 4));
		cairo_line_to(cr, 0, second->lgth);
		cairo_stroke(cr);
	}
	clock_hand_destroy(second);
}
void paint_big_hand(cairo_t *cr, int w, int h, int deco) {
	/** minute */
	get_mins();
	struct clock_hand *big = clock_hand_create((87 * w/200), 
									w/50, blh_r, blh_g, blh_b, mind);
	double degs = degrees2radians(big->angle);
	cairo_set_source_rgb(cr, big->rr, big->gg, big->bb);
	
	if (deco >= 2) {
		cairo_set_line_width(cr, (w/33) );
		cairo_rotate(cr, degs); /* angle */
		cairo_move_to(cr, 0, -(w / 8));
	} else if (deco == 1) {
		cairo_set_line_width(cr, big->wdth );
		cairo_rotate(cr, degs); /* angle */
		cairo_move_to(cr, 0, -(w / 8));
		cairo_line_to(cr, -(w / 45), 0);
		cairo_line_to(cr, 0, big->lgth);
		cairo_line_to(cr, (w / 45), 0);
		cairo_line_to(cr, 0, -(w / 8));
		cairo_close_path (cr);
		cairo_fill(cr);
	} else {
		cairo_set_line_width(cr, big->wdth);
		cairo_rotate(cr, degs); /* angle */
		cairo_move_to(cr, 0, 0);
	}
	if (deco != 1 ) {
		cairo_line_to(cr, 0, big->lgth);
	}
	if ((deco == 0) || (deco == 2)) {
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	}
	if (deco != 1 ) {
		cairo_stroke(cr);
	}
	cairo_arc(cr, 0, 0, w/60, 0, 2 * M_PI);
	clock_hand_destroy(big);
}
void paint_little_hand(cairo_t *cr, int w, int h, int deco) {
	/** hour */
	get_hrs();
	struct clock_hand *little = clock_hand_create((w/3), 
							w/33, blh_r, blh_g, blh_b, hrd);
	double degs = degrees2radians(little->angle);
	cairo_set_source_rgb(cr, little->rr, little->gg, little->bb);
	cairo_set_line_width(cr, little->wdth);
	cairo_rotate(cr, degs); /* angle */
	if (deco >= 2) {
		cairo_move_to(cr, 0, -(w / 8));
	} else if (deco == 1) {
		cairo_move_to(cr, 0, -(w / 10));
		cairo_line_to(cr, -(w / 45), 0);
		cairo_line_to(cr, 0, little->lgth);
		cairo_line_to(cr, (w / 45), 0);
		cairo_line_to(cr, 0, -(w / 10));
		cairo_close_path (cr);
		cairo_fill(cr);
	} else {
		cairo_move_to(cr, 0, 0);
	}
	cairo_line_to(cr, 0, little->lgth);
	if ((deco == 0) || (deco == 2)) {
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	}
	if (deco != 1 ) {
		cairo_stroke(cr);
	}
	clock_hand_destroy(little);
}
void paint_face(cairo_t *cr, int sizex, int sizey, int style, int ctype) {
	/* ctype determines who called here */
	sizex = sizex - 2;
	sizey = sizey - 2;
	double drgs;
	int fnt = sizex / 10; /* font size */
	/** background */
	int rad = (sizex/2) - 2;
	cairo_save(cr);
	cairo_set_line_width(cr, sizex / 60);  
	if (ctype == 0) {
		cairo_set_source_rgb(cr, bd_r, bd_g, bd_b);
	} else {
		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
	}
	cairo_translate(cr, sizex/2, sizey/2);
	cairo_set_antialias(cr, AA);
	cairo_arc(cr, 0, 0, rad, 0, 2 * M_PI);
	cairo_stroke_preserve(cr);
	if (ctype == 0) {
		cairo_set_source_rgb(cr, bg_r, bg_g, bg_b);
	} else {
		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
	}
	cairo_fill(cr);
	if (style == 1) {
		if (ctype == 0) {
			cairo_set_source_rgb(cr, bd_r, bd_g, bd_b);
		} else {
			cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
		}
		cairo_arc(cr, 0, 0, (3 * rad / 4), 0, 2 * M_PI);
	}
	cairo_restore(cr);
	
	/** clock face numerals */
	char *fface; /* a bit of glam for different styles */
	
	char *twelve, *six, *nine, *three, *one, *two, *four, *five, *seven, *eight, *ten, *eleven;
	int shift_twlv, shift_six, shift_three;
	if (style == 0) {
		fface = "DejaVu Sans";
		twelve = "12";
		six = "6";
		nine = "9";
		three = "3";
		shift_twlv = 0;
		shift_six = 0;
		shift_three = 0;
	}
	if (style == 1) {
		fnt = sizex / 8;
		fface = "serif";
		one = "I";
		two = "II";
		three = "III";
		four = "IV";
		five = "V";
		six = "VI";
		seven = "VII";
		eight = "VIII";
		nine = "IX";
		ten = "X";
		eleven = "XI";
		twelve = "XII";
	}
	/* print the numerals */
	if (style < 2) {
		cairo_select_font_face(cr, fface, CAIRO_FONT_SLANT_NORMAL,
				CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(cr, fnt);
	}
	if (ctype == 0) {
		cairo_set_source_rgb(cr, bd_r, bd_g, bd_b);
	} else {
		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
	}
	if (style == 0) {
		cairo_move_to(cr, (sizex/2) - (3 * fnt / 5) - shift_twlv, fnt);
		cairo_show_text(cr, twelve);
		cairo_move_to(cr, 
			((sizex/2) - (fnt / 3)) - shift_six, sizey - (4 * fnt / 13));
		cairo_show_text(cr, six);
		cairo_move_to(cr, (sizex - (7 * fnt / 8)) - shift_three,
												(sizey/2) + (4 * fnt / 11));
		cairo_show_text(cr, three);
		cairo_move_to(cr, fnt / 4, (sizey/2) + (4 * fnt / 11));
		cairo_show_text(cr, nine);
	} else if (style == 1) {
		cairo_translate(cr, sizex/2, sizey/2);
		int z;
		double offby;
		char *numeral;
		for (z = 0; z < 60; z = z + 5) {
			int degrees = (z * 6) - 90;
			cairo_save(cr);
			if (z == 0) { numeral = twelve; offby = - (3 * fnt / 4); }
			if (z == 5) { numeral = one; offby =  - (fnt / 8); }
			if (z == 10) { numeral = two; offby = - (fnt / 3); }
			if (z == 15) { numeral = three; offby = - (fnt / 2); }
			if (z == 20) { numeral = four; offby = - (fnt / 2); }
			if (z == 25) { numeral = five; offby = - (fnt / 3); }
			if (z == 30) { numeral = six; offby = - (2 * fnt / 3); }
			if (z == 35) { numeral = seven; offby = - (4 * fnt / 5); }
			if (z == 40) { numeral = eight; offby = - fnt; }
			if (z == 45) { numeral = nine; offby = - (2 * fnt / 5); }
			if (z == 50) { numeral = ten; offby = - (fnt / 3); }
			if (z == 55) { numeral = eleven; offby =  - (2 * fnt / 3); }
			drgs = degrees2radians(degrees);
			if (ctype == 0) {
				cairo_set_source_rgb(cr, bd_r, bd_g, bd_b);
			} else {
				cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
			}
			cairo_rotate(cr, drgs);
			cairo_move_to(cr, (sizex/2) - fnt, offby);
			cairo_rotate(cr,degrees2radians(90));
			cairo_show_text(cr, numeral);
			cairo_restore(cr);
		}
	} else {
		cairo_translate(cr, sizex/2, sizey/2);
		int g;
		for (g = 0; g < 46; g++) {
			int degrees = g * 6;
			cairo_save(cr);
			if ((g == 15) || (g == 45) || (g == 0)) {
				drgs = degrees2radians(degrees);
				if (ctype == 0) {
					cairo_set_source_rgb(cr, bd_r, bd_g, bd_b);
				} else {
					cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
				}
				if (style == 3) {
					cairo_set_line_width(cr, sizex / 30);
				} else {
					cairo_set_line_width(cr, sizex / 50);
				}
				cairo_rotate(cr, drgs); /* angle */
				cairo_move_to(cr, 0, (sizex/2) - (sizex/7));
				cairo_line_to(cr, 0, (sizex/2) - (sizex/45));
				cairo_stroke(cr);
			} else if (g == 30) {
				drgs = degrees2radians(degrees);
				if (ctype == 0) {
					cairo_set_source_rgb(cr, bd_r, bd_g, bd_b);
				} else {
					cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
				}
				if (style == 3) {
					cairo_set_line_width(cr, sizex / 30);
					cairo_rotate(cr, drgs); /* angle */
					cairo_move_to(cr, 0, (sizex/2) - (sizex/7));
					cairo_line_to(cr, 0, (sizex/2) - (sizex/45));
					cairo_stroke(cr);
				} else {
					cairo_set_line_width(cr, sizex / 20);
					cairo_rotate(cr, drgs); /* angle */
					cairo_move_to(cr, 0, (sizex/2) - (sizex/7));
					cairo_line_to(cr, 0, (sizex/2) - (sizex/45));
					cairo_stroke(cr);
					if (ctype == 0) {
						cairo_set_source_rgb(cr, bg_r, bg_g, bg_b);
					} else {
						cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
					}
					cairo_set_line_width(cr, sizex / 80);
					cairo_rotate(cr, (drgs)); /* opposite, so do it twice */
					cairo_rotate(cr, (drgs)); 
					cairo_move_to(cr, 0, (sizex/2) - (sizex/7));
					cairo_line_to(cr, 0, (sizex/2) - (sizex/45));
					cairo_stroke(cr);
				}
			}
			cairo_restore(cr);
		}
	}
	/** clock face markings */
	int xint, yint;
	if (style == 0) {
		cairo_translate(cr, sizex/2, sizey/2);
		xint = 29;
		yint = 31;
	}
	int f = 0;
	for (f = 0; f < 60; f++) {
		int degrees = f * 6;
		cairo_save(cr);
		if ((f == 15) || (f == 30) || (f == 45) ||
					(f == xint) || (f == yint) || (f == 0)) {
			continue;
		} 
		else if ((f == 5) || (f == 10) || (f == 20) || (f == 25) ||
				(f == 35) || (f == 40) || (f == 50) || (f == 55)) {
			drgs = degrees2radians(degrees);
			if (ctype == 0) {
				cairo_set_source_rgb(cr, bd_r, bd_g, bd_b);
			} else {
				cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
			}
			if (style == 3) {
				cairo_set_line_width(cr, sizex / 30);
				cairo_rotate(cr, drgs); /* angle */
				cairo_move_to(cr, 0, (sizex/2) - (sizex/7));
				cairo_line_to(cr, 0, (sizex/2) - (sizex/45));
				cairo_stroke(cr);
			} else if (style == 1) {
				break;
			} else {
				cairo_set_line_width(cr, sizex / 50);
				cairo_rotate(cr, drgs); /* angle */
				cairo_move_to(cr, 0, (sizex/2) - (sizex/15));
				cairo_line_to(cr, 0, (sizex/2) - (sizex/45));
				cairo_stroke(cr);
			}
		} else {
			drgs = degrees2radians(degrees);
			if (ctype == 0) {
				cairo_set_source_rgb(cr, bd_r, bd_g, bd_b);
			} else {
				cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
			}
			cairo_set_line_width(cr, sizex / 80);
			cairo_rotate(cr, drgs); /* angle */
			cairo_move_to(cr, 0, (sizex/2) - (sizex/20));
			if (style == 1) {
				cairo_line_to(cr, 0, (sizex/2) - (sizex/20));
			} else {
				cairo_line_to(cr, 0, (sizex/2) - (sizex/45));
			}
			cairo_stroke(cr);
		}
		cairo_restore(cr);
	}
}
void paint_dial(cairo_surface_t *cs, int sizex, int sizey, int style) {
	cairo_t *c;
	c = cairo_create(cs);
	
	/** face */
	paint_face(c, sizex, sizey, style, 1);
	cairo_show_page(c);
	cairo_destroy(c);
}
/** this paints everything and is looped from showxlib() */
void paint(cairo_surface_t *cs, int sizex, int sizey, int style, int deco) {
	
	cairo_t *c;
	c = cairo_create(cs);
	
	/** face */
	cairo_save(c);
	paint_face(c, sizex, sizey, style, 0);
	/* cairo_surface_write_to_png (cs, "/tmp/test.png"); */
	cairo_restore(c);
	/** hands */
	/** big */
	cairo_save(c);
	paint_big_hand(c, sizex, sizey, deco);
	cairo_restore(c); 
	/** little */
	cairo_save(c);
	paint_little_hand(c, sizex, sizey, deco);
	cairo_restore(c);
	/** second */
	cairo_save(c);
	paint_second_hand(c, sizex, sizey, deco);
	cairo_restore(c);
	cairo_show_page(c);
	cairo_destroy(c);
	/*cairo_surface_write_to_png (cs, "/tmp/test.png");*/ /* debug */
}
void request_expose(Display *dpy, Window win) {
	XEvent e;
	e.xexpose.type = Expose;
	e.xexpose.serial = NextRequest(dpy);
	e.xexpose.display = dpy;
	e.xexpose.window = win;
	XSendEvent(dpy, win, False, ExposureMask, &e);
	XFlush(dpy);
}
void showxlib(int width, int height, int style, int deco, char *xwin) {
	
	Display *dpy;
	Window rootwin;
	Window win;
	XEvent e;
	
	int scr;
	cairo_surface_t *cs;
	cairo_surface_t *cw;
	static char *window_id_format = "0x%lx\n";
	int mydepth;
    
	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "ERROR: Could not open display\n");
		exit(1);
	}
	scr = DefaultScreen(dpy);
    mydepth  = DefaultDepth(dpy, scr);
    
    unsigned int dpyWidth, dpyHeight;
    dpyWidth = DisplayWidth(dpy, scr);
    dpyHeight = DisplayHeight(dpy, scr);
    
    /* this block sorts out the desktop window */
    if (xwin) {
		fprintf(stdout,"xwin is %s\n",xwin); /* passed param */
		rootwin = strtol(xwin, 0, 0);
	}else {
		/* see pmdesktop.c */
		rootwin = find_root(dpy, scr, dpyWidth, dpyHeight);
		if (rootwin) {
			fprintf(stdout, window_id_format, rootwin);
		} else {
			fprintf(stderr, "No desktop window found\n"
					"Try \"xwininfo\" to find"
					"the right HEX desktop ID.");
			exit (1);
		}
	}
	/* end block */
	
	XSetWindowAttributes attr;
	attr.colormap = XCreateColormap(dpy, XDefaultRootWindow(dpy), 
								DefaultVisual(dpy, scr), AllocNone);
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.override_redirect = True; /* no deco */
	
	if ((posx == 0) && (posy == 0)){
		posx = dpyWidth - (width + 50); /* default at top right */
		posy = 50;
	} 
	else if ((posx > (dpyWidth - width)) || (posy > (dpyHeight - height))) {
			fprintf(stderr,"%dx or %dy is too big for your screen\n", 
															posx, posy);
			exit(1);
	}
	
	win = XCreateWindow(dpy, rootwin, posx, posy, width, height, 0, mydepth, 
			InputOutput, DefaultVisual(dpy, scr), 
			CWOverrideRedirect | CWColormap | CWBackPixel | CWBorderPixel, 
			&attr);
	XStoreName(dpy, win, prog);
	XSelectInput(dpy, win, ExposureMask|KeyPressMask|ButtonPressMask);
	XMapWindow(dpy, win);
	
	
	Pixmap pix = XCreatePixmap(dpy, win, width, height, 1);
	GC gc_pix = XCreateGC(dpy, pix, 0, 0);
	XFillRectangle (dpy, pix, gc_pix, 0, 0, width, height); 
	XRenderPictFormat *fmtcs;
	cs = cairo_xlib_surface_create(dpy, win, 
				DefaultVisual(dpy, scr), width, height);
	fmtcs = cairo_xlib_surface_get_xrender_format(cs);
	cs = cairo_xlib_surface_create_with_xrender_format(dpy, win,
		DefaultScreenOfDisplay(dpy), fmtcs, width, height);
	cw = cairo_xlib_surface_create_for_bitmap(dpy, pix, 
                DefaultScreenOfDisplay(dpy), width, height);
    cairo_surface_t *csbuf = cairo_image_surface_create 
							(CAIRO_FORMAT_ARGB32, width, height);
	cairo_t *cspaint = cairo_create (cs); 
	cairo_set_source_surface (cspaint, csbuf, 0, 0);             
    
    paint_dial(cw, width, height, style); /* only paints face */
	XShapeCombineMask(dpy, win, ShapeBounding, 0, 0, 
				cairo_xlib_surface_get_drawable(cw), ShapeSet); 
				
	XLowerWindow(dpy, win);
	
	int x11_fd;
	fd_set in_fds;
	struct timeval tv;
	
	x11_fd = ConnectionNumber(dpy);
	XFlush(dpy);
	int run = 1;
	while (run) { 
		/* fire it every 1 second */
		tv.tv_usec = 0;
		tv.tv_sec = 1;
		/* reset fds */
		FD_ZERO(&in_fds);
		FD_SET(x11_fd, &in_fds);
		
		if (select(x11_fd+1, &in_fds, 0, 0, &tv)) {
			/* event */
			while (XPending (dpy)) { 
				XNextEvent(dpy, &e); 
				if (e.type == Expose) {
					paint(csbuf, width, height, style, deco); 
					cairo_paint (cspaint);
					/* printf("expose\n"); */
				} else if (e.type == KeyPress) { 
					char buf[128] = {0}; 
					KeySym keysym; 
					XLookupString(&e.xkey, buf, sizeof(buf), &keysym, NULL); 
					if (keysym == XK_q) { 
						run = 0;
						break;
					} 
				} else if (e.type == ButtonPress) { 
					XSetInputFocus (dpy, win, RevertToNone, CurrentTime); 
				} else {
					/* printf("unknown event: %d\n", e.type); */
				}
			}
		} else {
			/* timer */
			/* printf("timer fires\n"); */
			request_expose(dpy, win);
		}
	} 
	cairo_surface_destroy(cs);
	cairo_surface_destroy(cw);
	XFreePixmap(dpy, pix); 
	XCloseDisplay(dpy); 
}

/**************** main *******************************************************/
int main(int argc, char **argv) {
	prog = argv[0];
	char *rootwindow = NULL; /* work around */
	if (argc == 2) {
		rootwindow = argv[1];
	}
	int hsize, vsize;
	if (test_conf() != 0) {
		fprintf(stdout, "No config or invalid; using defaults\n");
		size = 128;
	} else {
		read_conf();
		if ((size > 320) || (size < 80)) {
			fprintf(stderr,"Insane entry, %d must be < 320 or > 80\n",size);
			return 1;
		}
	}
	fprintf(stdout,"%s-%s has started\n", prog, THIS_VERSION);
	hsize = size; vsize = size;
	showxlib(hsize, vsize, style, deco, rootwindow);
	
	return 0;
}

