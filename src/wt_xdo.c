#include <errno.h>
#include <sys/types.h>
#include <xdo.h>

#include <wt_cwiid.h>
#include <wt_types.h>
#include <wt_xdo.h>
#include <queue.h>

extern enum WT_sys_states sys_state;
extern WT_screen screen;
extern queue* q_IR_points;
extern WT_IR_point null_point;

static xdo_t* xdo = NULL;       //xdo instance

int wii_init_xdo(void)
{
	xdo = xdo_new(NULL);
   if (NULL == xdo) {
      return ENOMEM;
   }

   return 0;
}

void wii_quit_xdo(void)
{
   if (xdo) {
   	xdo_free(xdo);
   }

	xdo = NULL;
}

void mouse_click(int button)
{

	WT_IR_point pt = {0,0}, avg = {0,0};
	int num = 0;
	while (is_valid_point( pt = queue_getpoint(q_IR_points) )) {

		num++;
		avg.x += pt.x;
		avg.y += pt.y;
	}
	avg.x /= num;
	avg.y /= num;

	if (!is_coordinate_valid(avg)) return;

	avg = map_to_screen(avg);

	xdo_move_mouse(xdo, avg.x, avg.y, 0);
	xdo_click_window(xdo, CURRENTWINDOW, button);

	reset_queue(q_IR_points);
}

void mouse_drag(WT_wii *wii)
{

	WT_IR_point pt = peek_queue(q_IR_points);

	if (is_valid_point(pt)) {

		pt = map_to_screen(pt);
		xdo_move_mouse(xdo, pt.x, pt.y, 0);
	}

	xdo_mouse_down(xdo, CURRENTWINDOW, LEFT_BUTTON);

	while (is_valid_point( (pt = queue_getpoint(q_IR_points)) )) {

		pt = map_to_screen(pt);
		move_line(pt);
		xdo_move_mouse(xdo, pt.x, pt.y, 0);
		get_data(wii);
	}

	reset_queue(q_IR_points);

	xdo_mouse_up(xdo, CURRENTWINDOW, LEFT_BUTTON);
}

void mouse_scroll(WT_wii *wii)
{

	WT_IR_point first, second;
	first = queue_getpoint(q_IR_points);

	while (is_valid_point( second = queue_getpoint(q_IR_points) )) {

		int dir = scroll_direction(first, second);
		if (dir != NO_SCROLL) {

			xdo_click_window(xdo, CURRENTWINDOW, dir);
			first = second;
		}
		get_data(wii);
	}

	reset_queue(q_IR_points);
}



void swap(int* a, int* b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

void move_line(WT_IR_point to)
{
	int scr;
	WT_IR_point from;
	xdo_get_mouse_location(xdo, &from.x, &from.y, &scr);

	if (from.x == to.x && from.y == to.y) return;

	bool steep = abs(to.y - from.y) > abs(to.x - from.x);
	if (steep){
		swap(&from.x, &from.y);
		swap(&to.x, &to.y);
	}

	int deltax = to.x - from.x;
	int deltay = abs(to.y - from.y);
	int error = deltax / 2;

	int y = from.y;

	int ystep = 1;
	if (from.y > to.y)
	ystep = -1;

	int xstep = 1;
	if (from.x > to.x){
		xstep = -1;
	}

	for ( int x = from.x; x != to.x; x += xstep){

		if (steep) xdo_move_mouse(xdo, y, x, 0);
		else xdo_move_mouse(xdo, x, y, 0);

		error = error - deltay;
		if (error < 0){
			y = y + ystep;
			error = error + deltax;
		}
	}
}

WT_IR_point map_to_screen(WT_IR_point pt)
{

	pt.x = (pt.x - screen.min.x)/screen.scale_x;
	pt.y = (pt.y - screen.min.y)/screen.scale_y;

	return pt;
}

int scroll_direction(WT_IR_point first, WT_IR_point second)
{

	double rel_diff = (double)abs(second.y - first.y) / first.y;

	if (rel_diff < 0.02) return NO_SCROLL;

   /* Go down for positive difference, else go up */
	return (second.y - first.y) > 0 ? WHEEL_DOWN : WHEEL_UP ;  
}
