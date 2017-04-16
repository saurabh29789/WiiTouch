#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

#include <cwiid.h>
#include <wt_cwiid.h>
#include <wt_xdo.h>
#include <wt_types.h>
#include <queue.h>

extern enum WT_btn_state btn_state;
extern enum WT_sys_states sys_state;

extern queue* q_IR_points;

void free_resources(WT_wii *wii);

int resource_acquisition(WT_wii *wii)
{

	int ret = 0;

	fprintf(stdout, "Detecting Wiimote. Press Buttons (1+2) on Wiimote\n");

	ret = wii_detect(wii);
	if (ret) {

		fprintf(stderr, "Wiimote not detected.\n");
		return ret;
	}

	fprintf(stdout, "Wiimote detected.\n");

	if (SIG_ERR == signal(SIGALRM, alarm_handler)) {
      ret = errno;

		fprintf(stderr, "Error setting alarm handler\n");
		return ret;
	}

	q_IR_points = init_queue();
	if (NULL == q_IR_points) {
      free_resources(wii);
		return ENOMEM;
	}

	ret = wii_init_xdo();
   if (ret) {
      free_resources(wii);
      return ret;
   }

	return 0;
}

void free_resources(WT_wii *wii)
{
	wii_quit_xdo();

	destroy_queue(q_IR_points);

	cwiid_close(wii->wii_data);
}

int main(int argc, char *argv[])
{
	WT_wii wii;
	int done = 0;
   int ret;

	ret = resource_acquisition(&wii);
   if (ret) {
		return ret;
	}

	do {

		/* get remote data (IR data, button data) and
		 * set Wii state valid or invalid
       */
		btn_state = get_data(&wii);

		if (VALID == sys_state) {

			alarm(1);            // set timer of 1 second
			get_state(&wii);     // get and update FSMs
			wii_act(&wii);       // call xdo action according to FSM state
		}

		/* execute remote button roles by checking wii button state 
		 * HOME for recalibration and A for exiting application
       */
		switch(btn_state) {

			case BTN_HOME:
                     fprintf(stdout, "Calibrate\n");
							wii_calibrate(&wii);
							break;

			case BTN_A:
                     done = 1;
							break;

         // any other button has no effect
			default:    break;
		}

	} while (!done);

	fprintf(stdout, "Connection Closed. Quitting...\n");
	free_resources(&wii);

	return 0;
}
