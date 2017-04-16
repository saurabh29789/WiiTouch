#ifndef _WT_SDL_HELPER_H_
#define _WT_SDL_HELPER_H_

#include "SDL/SDL.h"

#define CHECK_and_call(x, max_x, y, max_y, color) \
                if ((x>0 && x<max_x) && (y>0 && y<max_y)) { \
                    draw_pixel(x,y,color);\
                }

#define BLACK   0xff000000
#define RED     0xffff0000
#define GREEN   0xff00ff00
#define BLUE    0xff0000ff
#define WHITE   0xffffffff

void init_sdl(void);

void draw_pixel(int,int,unsigned int);

void draw_circle(int, int, int, unsigned int);

void clear_screen(void);

void draw_marker(int);

void quit_sdl(void);

#endif
