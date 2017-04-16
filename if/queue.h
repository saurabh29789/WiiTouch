#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdbool.h>

#include "wt_types.h"

struct queue {

    int front;
    int rear;

    WT_IR_point points[NUM_OF_POINTS];
};

typedef struct queue queue;

queue* init_queue(void);

void destroy_queue(queue*);

void reset_queue(queue*);

void queue_putpoint(queue*, WT_IR_point);

WT_IR_point queue_getpoint(queue*);

WT_IR_point peek_queue(queue*);

bool is_valid_point(WT_IR_point);

#endif
