/* vim:set ts=2 sw=2 et: */
/*
 * Tiny Triangle
 * A skeleton for 4k intros for Linux
 *
 * Alexander Rødseth <rodseth@gmail.com>
 * ZLIB licensed
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include "alsa/asoundlib.h"
#include <GL/glu.h>

#define APP_TITLE "Tiny Triangle"

/* Assembly functions */
extern int add(int a, int b);
extern void quit();

/* Used for audio */
static char *device = "default";
snd_output_t *output = NULL;
unsigned char buffer[16*1024];
const char *audio_error = "sound fail";

const char *X_error = "X fail";

int singleBufferAttributess[] = {
  GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
  GLX_RENDER_TYPE,   GLX_RGBA_BIT,
  GLX_RED_SIZE,      1,   /* Request a single buffered color buffer */
  GLX_GREEN_SIZE,    1,   /* with the maximum number of color bits  */
  GLX_BLUE_SIZE,     1,   /* for each component                     */
  None
};

int doubleBufferAttributes[] = {
  GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
  GLX_RENDER_TYPE,   GLX_RGBA_BIT,
  GLX_DOUBLEBUFFER,  True,  /* Request a double-buffered color buffer with */
  GLX_RED_SIZE,      1,     /* the maximum number of bits per component    */
  GLX_GREEN_SIZE,    1, 
  GLX_BLUE_SIZE,     1,
  None
};

void drawGraphics() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.33, 1.33, -1, 1, 1.5, 100);
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glShadeModel(GL_FLAT);
  
  glLoadIdentity();
  glColor3f(1.0, 1.0, 0.0);
  glBegin(GL_TRIANGLES);
  glVertex3i( 1,  1, -10);
  glVertex3i( 1, -1, -10);
  glVertex3i(-1,  1, -10);
  glEnd();
}

static Bool WaitForNotify( Display *dpy, XEvent *event, XPointer arg ) {
    return (event->type == MapNotify) && (event->xmap.window == (Window) arg);
}

  Display                 *dpy;
  int                     numReturned;

void _start()
{
  /* Used for graphics */
  Window                  xWin;
  XEvent                  event;
  XVisualInfo             *vInfo;
  XSetWindowAttributes    swa;
  XWindowAttributes       gwa;
  GLXFBConfig             *fbConfigs;
  GLXContext              context;
  GLXWindow               glxWin;
  int                     swaMask;
  int                     swapFlag;

  unsigned int i;
  int err;

  /* Used for audio */
  snd_pcm_t *handle;
  snd_pcm_sframes_t frames;
  swapFlag = True;
  int sound = True;

  for (i = 0; i < sizeof(buffer); i++)
  {
    /* Generate sound here */
    buffer[i] = random() & 0xff;
  }
  if ((err=snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
  {
    //printf("Playback open error: %s\n", snd_strerror(err));
    printf("%s\n", audio_error);
    //sound = False;
    exit(EXIT_FAILURE);
  }
  if (sound && ((err=snd_pcm_set_params(handle,SND_PCM_FORMAT_U8,
                             SND_PCM_ACCESS_RW_INTERLEAVED,
                             1, 48000, 1, 500000)) < 0))
  {
    //printf("Playback open error: %s\n", snd_strerror(err));
    printf("%s\n", audio_error);
    exit(EXIT_FAILURE);
  }

  dpy = XOpenDisplay(NULL);
  if (dpy == NULL) {
    printf("%s\n", X_error);
  }

  /* Request a suitable framebuffer configuration - try for a double 
  ** buffered configuration first */
  fbConfigs = glXChooseFBConfig( dpy, DefaultScreen(dpy), doubleBufferAttributes, &numReturned );
  if ( fbConfigs == NULL ) {  /* no double buffered configs available */
    fbConfigs = glXChooseFBConfig( dpy, DefaultScreen(dpy), singleBufferAttributess, &numReturned );
    swapFlag = False;
  }

  /* Create an X colormap and window with a visual matching the first
  ** returned framebuffer config */
  vInfo = glXGetVisualFromFBConfig( dpy, fbConfigs[0] );

  swa.border_pixel = 0;
  swa.event_mask = StructureNotifyMask;
  swa.colormap = XCreateColormap( dpy, RootWindow(dpy, vInfo->screen), vInfo->visual, AllocNone );

  swaMask = CWBorderPixel | CWColormap | CWEventMask;

  xWin = XCreateWindow( dpy, RootWindow(dpy, vInfo->screen), 0, 0, 256, 256,
                        0, vInfo->depth, InputOutput, vInfo->visual,
                        swaMask, &swa );

  /* Create a GLX context for OpenGL rendering */
  context = glXCreateNewContext( dpy, fbConfigs[0], GLX_RGBA_TYPE, NULL, True );

  /* Create a GLX window to associate the frame buffer configuration
  ** with the created X window */
  glxWin = glXCreateWindow( dpy, fbConfigs[0], xWin, NULL );
    
  /* Map the window to the screen, and wait for it to appear */
  XMapWindow( dpy, xWin );
  XIfEvent( dpy, &event, WaitForNotify, (XPointer) xWin );

  /* Bind the GLX context to the Window */
  glXMakeContextCurrent( dpy, glxWin, xWin, context );

  while (1) {
    XNextEvent(dpy, &event);
    if (event.type == Expose) {
      /* Play sound */
      if (sound) {
        frames=snd_pcm_writei(handle, buffer, sizeof(buffer));
        if (frames < 0) {
          frames = snd_pcm_recover(handle, frames, 0);
        }
        if (frames < 0) {
          //printf("snd_pcm_writei failed: %s\n", snd_strerror(err));
          printf("%s\n", audio_error);
          break;
        }
        if (frames > 0 && frames < (long)sizeof(buffer)) {
          //printf("Short write (expected %li, wrote %li)\n", (long)sizeof(buffer), frames);
          printf("%s\n", audio_error);
        }
      }
      /* Do stuff */
      /* moveAllSlidersRandomlyAround(); */
      /* Draw stuff */
      XGetWindowAttributes(dpy, xWin, &gwa);
      glViewport(0, 0, gwa.width, gwa.height);
      drawGraphics();
      glFlush();
      if ( swapFlag ) {
        glXSwapBuffers(dpy, xWin);
      }
    } else if (event.type == KeyPress) {
      break;
    }
  }

  /* Shutting down audio */
  snd_pcm_close(handle);

  /* Closing the window */
  glXMakeCurrent(dpy, None, NULL);
  glXDestroyContext(dpy, context);
  XDestroyWindow(dpy, xWin);
  XCloseDisplay(dpy);
  
  /* Assembly functions */
  printf("%d\n", add(19, 23));
  quit();
}
