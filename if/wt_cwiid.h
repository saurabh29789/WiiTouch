#ifndef WT_CWIID_HELPER_H_
#define WT_CWIID_HELPER_H_

#include<stdbool.h>

#include "wt_types.h"


#define toggle_bit(bf,b)	\
	(bf) = ((bf) & b)		   \
	       ? ((bf) & ~(b))	\
	       : ((bf) | (b))

/* Detect wii mote and initiallise 'WT_wii' */
int wii_detect(WT_wii *wii);

/* Create SDL screen and fetch calibration data */
void wii_calibrate(WT_wii * wii);

/*Get IR state and according to it get its position */
enum WT_btn_state get_data(WT_wii *wii);

/* Get IR position, update FSM state */
enum WT_sys_states get_state(WT_wii *wii);

/* Simulate mouse corresponding to current FSM state */
void wii_act(WT_wii *);

/* Helper Functions */
void err(cwiid_wiimote_t*, const char*, va_list);

void nsleep(long);

void alarm_handler(int);

bool is_coordinate_valid(WT_IR_point);

#endif
