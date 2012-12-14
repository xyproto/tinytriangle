/* vim:set ts=2 sw=2 et: */
/*
 * Concert Master
 * A skeleton for 4k intros for Linux
 *
 * Alexander Rødseth <rodseth@gmail.com>
 * ZLIB licensed
 *
 */

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <math.h>

/* Assembly functions */
extern int add(int a, int b);
extern void quit();

/* Used for audio */
static char *device = "default";
snd_output_t *output = NULL;
unsigned char buffer[16*1024];

static float PI = 3.14;

float skew = 0.0;

void torus()
{
  float r = 0.0;
  float x = sin(r+skew) * 2.0f;
  float y = cos(r+skew) * 2.0f;
  float prevx = x;
  float prevy = y;
  while (r < (2.0f*PI))
  {
    x = sin(r+skew) * 2.0f;
    y = cos(r+skew) * 2.0f;

    printf("%f %f %f\n", x, y, r);

    glBegin(GL_TRIANGLES);
    glVertex3i( 0.0, 0.0, -10);
    glVertex3i( prevx, prevy, -10);
    glVertex3i( x,  y, -10);
    glEnd();

    r += 0.1;
    prevx = x;
    prevy = y;
  }
  skew += 0.01;
  printf(".");

}

void _start()
{
  SDL_Event event;
  
  int err;
  unsigned int i;
  snd_pcm_t *handle;
  snd_pcm_sframes_t frames;

  for (i=0; i<sizeof(buffer); i++)
  {
    /* Generate sound here */
    buffer[i] = i ^ i; //random() & 0xff;
  }
  if ((err=snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
  {
    printf("Playback open error: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }
  if ((err=snd_pcm_set_params(handle,SND_PCM_FORMAT_U8,
                             SND_PCM_ACCESS_RW_INTERLEAVED,
                             1, 48000, 1, 500000)) < 0)
  {
    printf("Playback open error: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }

  SDL_SetVideoMode(640, 480, 0, SDL_OPENGL | SDL_FULLSCREEN);
  SDL_ShowCursor(SDL_DISABLE);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.33, 1.33, -1, 1, 1.5, 100);
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glShadeModel(GL_FLAT);
  
  glLoadIdentity();
  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_TRIANGLES);
  glVertex3i( 1,  1, -10);
  glVertex3i( 1, -1, -10);
  glVertex3i(-1,  1, -10);
  glEnd();
  SDL_GL_SwapBuffers();

  do {
    frames=snd_pcm_writei(handle, buffer, sizeof(buffer));
    if (frames < 0) {
      frames = snd_pcm_recover(handle, frames, 0);
    }
    if (frames < 0) {
      printf("snd_pcm_writei failed: %s\n", snd_strerror(err));
      break;
    }
    if(frames > 0 && frames < (long)sizeof(buffer)) {
      printf("Short write (expected %li, wrote %li)\n",
          (long)sizeof(buffer), frames);
    }

    glLoadIdentity();
    glColor3f(1.0, 0.0, 0.0);
    torus();
    SDL_GL_SwapBuffers();

    SDL_PollEvent(&event);
  } while (event.type != SDL_KEYDOWN);

  /* Shutting down */
  snd_pcm_close(handle);
  SDL_Quit();
  
  /* Assembly functions */
  printf("%d\n", add(19, 23));
  quit();
}
