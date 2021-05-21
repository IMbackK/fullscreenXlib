#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	bool ewmh = true;
	if(argc ==  2)
		ewmh = false;
	
	Display *dpy = XOpenDisplay(NULL);
	assert(dpy);
	
	Atom netWMFullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
	assert(netWMFullscreen != None);
	
	Atom netWMState =  XInternAtom(dpy, "_NET_WM_STATE", False);
	assert(netWMState != None);
	
	int black = BlackPixel(dpy, DefaultScreen(dpy));
	int white = WhitePixel(dpy, DefaultScreen(dpy));
	
	int sizeX = XWidthOfScreen(XDefaultScreenOfDisplay(dpy));
	int sizeY = XHeightOfScreen(XDefaultScreenOfDisplay(dpy));
	
	XSetWindowAttributes wa;
	wa.border_pixel = 0;
	wa.override_redirect = !ewmh;
	wa.background_pixel = white;
	
	Window w = XCreateWindow(dpy, DefaultRootWindow(dpy), 0, 0, sizeX, sizeY, 0, DefaultDepth(dpy, 0), InputOutput, DefaultVisual(dpy, 0), CWOverrideRedirect | CWBorderPixel | CWBackPixel, &wa);
	
	XSelectInput(dpy, w, StructureNotifyMask | ExposureMask | KeyPressMask);
	
	if(ewmh)
		XChangeProperty(dpy, w, netWMState, XA_ATOM, 32, PropModeReplace, (unsigned char*)&netWMFullscreen, 1);
	
	XMapWindow(dpy, w);
	
	GC gc = XCreateGC(dpy, w, 0, NULL);
	XSetForeground(dpy, gc, black);
	
	if(!ewmh) 
	{
		if(XGrabKeyboard(dpy, w, true, GrabModeAsync, GrabModeAsync, CurrentTime) != GrabSuccess)
			printf("Could not XGrabKeyboard\n");
		if(XGrabPointer(dpy, w, true, 0, GrabModeAsync, GrabModeAsync,  None, None, CurrentTime) != GrabSuccess)
			printf("Could not XGrabPointer\n");
	}
	
	while(true) 
	{
		XEvent event;
		XNextEvent(dpy, &event);
		
		if(!ewmh)
			XSetInputFocus(dpy, w, RevertToPointerRoot, CurrentTime);
		
		if(event.type == ConfigureNotify)
		{
			sizeX = event.xconfigure.width;
			sizeY = event.xconfigure.height;
		}
		else if (event.type == KeyPress)
		{
			break;
		}
		XClearWindow(dpy, w);
		XDrawArc(dpy, w, gc, 0, 0, sizeX, sizeY, 0, 365*64);
		XFlush(dpy);
	}
	
	if(!ewmh) 
	{
		XUngrabPointer(dpy, CurrentTime);
		XUngrabKeyboard(dpy, CurrentTime);
	}
	return 0;
}


