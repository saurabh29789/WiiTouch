#ifndef _MOUSE_SIMULATE_H_
#define _MOUSE_SIMULATE_H_

#include <wt_types.h>

#define LEFT_BUTTON 		1
#define MIDDLE_BUTTON 	2
#define RIGHT_BUTTON 	3
#define WHEEL_UP 			4
#define WHEEL_DOWN 		5
#define NO_SCROLL 		0

int wii_init_xdo(void);

void wii_quit_xdo(void);

void mouse_click(int);

void mouse_drag(WT_wii *);

void mouse_scroll(WT_wii *);

void swap(int*, int*);

void move_line(WT_IR_point);

WT_IR_point map_to_screen(WT_IR_point);

int scroll_direction(WT_IR_point, WT_IR_point);

#endif
