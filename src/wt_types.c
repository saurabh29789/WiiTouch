#include <wt_types.h>

#include <queue.h>

volatile enum WT_sys_states sys_state = INVALID;

enum WT_btn_state  btn_state = NONE;

volatile queue* q_IR_points = NULL;

WT_wii wii = { .wii_data  = NULL,
               .rpt_mode  = 0
             };

WT_screen screen = { .res_x = 0,
                     .res_y = 0,
                     .scale_x = 0,
                     .scale_y = 0
                   };
