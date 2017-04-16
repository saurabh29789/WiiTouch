#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include <cwiid.h>
#include <wt_cwiid.h>
#include <wt_sdl.h>
#include <wt_types.h>
#include <queue.h>
#include <wt_xdo.h>

static bool one_second_elapsed  = false;
static bool two_seconds_elapsed = false;

extern enum WT_sys_states sys_state;
extern WT_screen screen;
extern queue* q_IR_points;
extern WT_IR_point null_point;

cwiid_err_t err_data;

/* Detect wii mote and initiallise 'WT_wii' */
int wii_detect(WT_wii *wii)
{
	cwiid_set_err(err);

	wii->rpt_mode = 0;

	/* Connect to bluetooth device with any hardware address */
	wii->bd_address = *BDADDR_ANY;

	wii->wii_data = cwiid_open(&wii->bd_address, 0 /* flags */);
	if (NULL == wii->wii_data) {
		return ENODEV;
	}

	/* Enable Infrared and Button-reporting mode */
	toggle_bit(wii->rpt_mode, CWIID_RPT_IR);
	toggle_bit(wii->rpt_mode, CWIID_RPT_BTN);

	return cwiid_set_rpt_mode(wii->wii_data, wii->rpt_mode);
}

/* 
 * Note: Invalid calibration is not handled.
 * User needs to re-calibrate by pressing Home
 */

//Create SDL screen and fetch calibration data
void wii_calibrate(WT_wii *wii)
{
	/* 0% percent smoothing [calibration_data +/- 0%]
	 * To be used for error-correction
	 */
	const double smoothing = 0.0;

	init_sdl();

	fprintf(stdout, "Screen Resolution : %dx%d\n", screen.res_x, screen.res_y);
	clear_screen();

   // One calibration for each corner
	for (int i = 0; i < 4; ++i){

		draw_marker(i+1);

		do {
			SDL_Event event;

			get_data(wii);

			while (SDL_PollEvent(&event) && event.type == SDL_KEYUP){
				// If escape is pressed, return (and thus, quit)
				if (event.key.keysym.sym == SDLK_ESCAPE){
					quit_sdl();
					return;
				}
			}
		} while(sys_state!=VALID);

		sys_state = INVALID;

      /* Draws a white circle at center to indicate success */
		draw_marker(-1);
		sleep(2);
		clear_screen();
	}

	for (int i = 0; i < 4; ++i) {

		WT_IR_point pt = queue_getpoint(q_IR_points);
		int x = pt.x; //(wii.wii_state).ir_src[i].pos[CWIID_X];
		int y = pt.y; //(wii.wii_state).ir_src[i].pos[CWIID_Y];

		screen.calibration[i].x = x;
		screen.calibration[i].y = y;
		fprintf(stdout, "Point: %d => %d %d\n", i+1, x, y);

	}

	int x_min = (screen.calibration[0].x + screen.calibration[3].x ) /2;
	int y_min = (screen.calibration[0].y + screen.calibration[1].y ) /2;

	int x_max = (screen.calibration[1].x + screen.calibration[2].x ) /2;
	int y_max = (screen.calibration[2].y + screen.calibration[3].y ) /2;

	screen.min.x = x_min * (1 - smoothing);
	screen.min.y = y_min * (1 - smoothing);

	screen.max.x = x_max * (1 + smoothing);
	screen.max.y = y_max * (1 + smoothing);

	screen.scale_x = ((double)( screen.max.x - screen.min.x)) / screen.res_x;
	screen.scale_y = ((double)( screen.max.y - screen.min.y)) / screen.res_y;

	fprintf(stdout, "Screen MIN      (%d, %d)\n"
						 "Screen MAX      (%d, %d)\n"
					  	 "Scale Factor    (%f %f)\n",
						  screen.min.x, screen.min.y,\
						  screen.max.x, screen.max.y,\
						  screen.scale_x, screen.scale_y);

	quit_sdl();
}

//Get IR state and according to it update system state and wii structure
enum WT_btn_state get_data(WT_wii *wii)
{
	if (cwiid_get_state(wii->wii_data, &wii->wii_state)) {
		fprintf(stderr, "Error getting cwiid_state\n");
		return NONE;
	}

	sys_state = INVALID;

	if (wii->wii_state.ir_src[0].valid) {

	   WT_IR_point point;
		point.x = wii->wii_state.ir_src[0].pos[CWIID_X];
		point.y = wii->wii_state.ir_src[0].pos[CWIID_Y];

		queue_putpoint(q_IR_points, point);

		sys_state = VALID;
	}

	if(wii->wii_state.buttons & CWIID_BTN_A){
		return BTN_A;
	}

	if(wii->wii_state.buttons & CWIID_BTN_HOME){
		return BTN_HOME;
	}

	return NONE;
}

//Get IR position, update FSM state
enum WT_sys_states get_state(WT_wii *wii)
{
	volatile int ir_num = 0;

	do {
		ir_num++;
		get_data(wii);
		nsleep( 1000 * 1000 );

	} while( VALID == sys_state &&
			 (!two_seconds_elapsed) &&
			 (ir_num < (0.5)*NUM_OF_POINTS) );

	alarm(0);
	printf("Number of points : %d, valid_state %d\n",
         ir_num, VALID == sys_state);

	/* If the LED turns on only for an instant,
	 * then assume a left-click
	 */

	if(ir_num < (0.06* NUM_OF_POINTS))
		return sys_state = LEFT_CLICK;

	/* If the LED turned off before the two seconds elapsed,
	 * then it is either a Left Click or Right Click,
	 * depending on the time it was on for
	 */
	if(INVALID == sys_state){

		if(is_coordinate_valid( peek_queue(q_IR_points) ) ){

         sys_state = (one_second_elapsed || two_seconds_elapsed) ?
		   		      RIGHT_CLICK :
                     INVALID;
		}
	}

/* If the 2nd alarm went off and LED was still active,
 * then it is either Drag or Scroll,
 * depending on whether the IR was within valid screen area
 */
	else if(one_second_elapsed || two_seconds_elapsed){

		one_second_elapsed = two_seconds_elapsed = false;

		sys_state = is_coordinate_valid( peek_queue(q_IR_points) ) ?
                  DRAG:
                  SCROLL;
	}

	return sys_state;
}

void wii_act(WT_wii *wii)
{                         //Simulate mouse according to current FSM state

	switch(sys_state){
		case SCROLL:            fprintf(stdout, "SCROLL\n");
										mouse_scroll(wii);
										break;

		case LEFT_CLICK:        fprintf(stdout, "LEFT CLICK\n");
										mouse_click(LEFT_BUTTON);
										break;

		case RIGHT_CLICK:       fprintf(stdout, "RIGHT CLICK\n");
										mouse_click(RIGHT_BUTTON);
										break;

		case DRAG:              fprintf(stdout, "DRAG\n");
										mouse_drag(wii);
										break;

		default:                break;
	}

	reset_queue(q_IR_points);

	fflush(stdout);
}


void err(cwiid_wiimote_t *wiimote, const char *s, va_list ap)
{

	if (wiimote)
		printf("%d:", cwiid_get_id(wiimote));
	else
		printf("-1:");

	vprintf(s, ap);
	printf("\n");
}

void nsleep(long nsec)
{

#if 0
struct timespec sleepTime;
struct timespec returnTime = {0,0};

sleepTime.tv_sec = 0;
sleepTime.tv_nsec = nsec;

nanosleep(&sleepTime, &returnTime);
#endif
}

void alarm_handler(int sig_type)
{

	one_second_elapsed  = !one_second_elapsed;
	two_seconds_elapsed = !one_second_elapsed;
	fprintf(stderr, "%d %d\n", one_second_elapsed, two_seconds_elapsed);

	if(one_second_elapsed) alarm(1);
}

bool is_coordinate_valid(WT_IR_point p)
{

	bool check_x = p.x >= screen.min.x && p.x <= screen.max.x;
	bool check_y = p.y >= screen.min.y && p.y <= screen.max.y;

	if(check_x && check_y) return 1;

	fprintf(stderr, "Coordinates Invalid: %d %d, (%d, %d)\n ", p.x, p.y, check_x, check_y);
	return 0;
}
