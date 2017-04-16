#ifndef _WT_TYPES_H_

#define _WT_TYPES_H_

#include "cwiid.h"
#include "bluetooth/bluetooth.h"
#include "SDL/SDL.h"

#define NUM_OF_POINTS 2000

/* Enumeration to define FSM States */

enum WT_sys_states {
	INVALID=1,
   VALID,
   TIMER,
	SCROLL,
   DRAG,
	LEFT_CLICK,
   RIGHT_CLICK,
};

enum WT_btn_state  {
	NONE = 1,
	BTN_HOME,
	BTN_A,
};

/* struct to hold an IR point */
struct _WT_IR_point{
	int x;
	int y;
};

typedef struct _WT_IR_point WT_IR_point;

/* struct to hold all cwiid data */
struct _WT_wii{
    cwiid_wiimote_t*    wii_data;
    struct cwiid_state  wii_state;

    unsigned char       rpt_mode;
    bdaddr_t            bd_address;
};


typedef struct _WT_wii WT_wii;

/* struct to hold screen resolution and calibration data */
struct _WT_screen{
	int res_x;
	int res_y;

    struct points{
        int x, y;
    } calibration[4], min, max;

    double scale_x, scale_y;
    SDL_Surface* screen_buffer;
};

typedef struct _WT_screen WT_screen;

#endif
