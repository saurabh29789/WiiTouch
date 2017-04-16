#include <SDL/SDL.h>

#include <wt_sdl.h>
#include <wt_types.h>

extern WT_screen screen;

void init_sdl(void) 
{
   SDL_Surface* surface;

	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}

	/*
    * Register SDL_Quit to be called at exit; makes sure things are
	 * cleaned up when we quit.
    */
	atexit(SDL_Quit);

	/* Attempt to create a x*y window with 32bit pixels. */
	screen.screen_buffer = SDL_SetVideoMode(screen.res_x, screen.res_y,
                                           32, SDL_SWSURFACE|SDL_FULLSCREEN);
	surface = screen.screen_buffer;

	if (NULL == surface){
		fprintf(stderr, "Unable to set %dx%d video: %s\n",
              screen.res_x, screen.res_y, SDL_GetError());
		exit(1);
	}

	screen.res_x = surface->w;
	screen.res_y = surface->h;
}

void draw_pixel(int x, int y, unsigned int color)
{
	SDL_Surface* surface = screen.screen_buffer;

	unsigned int* ptr = (unsigned int*)surface->pixels;
	int lineoffset = y*(surface->pitch / 4);

	ptr[lineoffset+x] = color;
}

void draw_circle(int cx, int cy, int radius, unsigned int color)
{

	int f     = 1 - radius;
	int ddf_x = 1;
	int ddf_y = -2 * radius;
	int x     = 0;
   int y=radius;

	SDL_LockSurface(screen.screen_buffer);

	CHECK_and_call(cx,        screen.res_x, cy+radius, screen.res_y, color);
	CHECK_and_call(cx,        screen.res_x, cy-radius, screen.res_y, color);
	CHECK_and_call(cx+radius, screen.res_x, cy,        screen.res_y, color);
	CHECK_and_call(cx-radius, screen.res_x, cy,        screen.res_y, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddf_y += 2;
			f += ddf_y;
		}
		x++;
		ddf_x += 2;
		f += ddf_x;

		CHECK_and_call(cx+x, screen.res_x, cy+y, screen.res_y, color);
		CHECK_and_call(cx-x, screen.res_x, cy+y, screen.res_y, color);
		CHECK_and_call(cx+x, screen.res_x, cy-y, screen.res_y, color);
		CHECK_and_call(cx-x, screen.res_x, cy-y, screen.res_y, color);
		CHECK_and_call(cx+y, screen.res_x, cy+x, screen.res_y, color);
		CHECK_and_call(cx-y, screen.res_x, cy+x, screen.res_y, color);
		CHECK_and_call(cx+y, screen.res_x, cy-x, screen.res_y, color);
		CHECK_and_call(cx-y, screen.res_x, cy-x, screen.res_y, color);
	}

	SDL_Surface* surface = screen.screen_buffer;
	SDL_UpdateRect(surface, 0, 0, screen.res_x, screen.res_y);

	SDL_UnlockSurface(screen.screen_buffer);
}

void clear_screen()
{

	SDL_Surface* surface = screen.screen_buffer;

	SDL_FillRect(surface, NULL, BLUE);      /* FIll complete screen with BLUE color */
	SDL_Flip(surface);
}

void draw_marker(int N)
{

	int r = 10;
	for(int i = 1; i < r; i++) {

		switch (N){
			case -1:
					  	draw_circle(screen.res_x/2, screen.res_y/2, i, WHITE);
						break;
			case 1:
					  	draw_circle(r, r, i, RED);
						break;

			case 2:
						draw_circle(screen.res_x - r, r, i, RED);
						break;

			case 3:
						draw_circle(screen.res_x -r, screen.res_y -r, i, RED);
						break;

			case 4:
						draw_circle(r, screen.res_y - r, i, RED);
						break;
			}
	}
}

void quit_sdl(void)
{
	SDL_FreeSurface(screen.screen_buffer);
	SDL_Quit();
}
