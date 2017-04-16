#include<stdlib.h>

#include <wt_types.h>
#include <queue.h>

const WT_IR_point null_point = {-1, -1};

queue* init_queue(void)
{
	queue* que = malloc(sizeof(queue));

	if (NULL != que) {
		que->front = que->rear = -1;
	}
	return que;
}

void destroy_queue(queue* que)
{
	if (que) free(que);
}

void reset_queue(queue* que)
{
	que->front = que->rear = -1;
}

void queue_putpoint(queue* que, WT_IR_point pt)
{
	pt.y = CWIID_IR_Y_MAX - pt.y;
	int next = (que->rear + 1) % NUM_OF_POINTS;

	if (que->front == next) return;

   if (que->front == -1) que->front = 0;

   que->points[next] = pt;
   que->rear = next;
}

WT_IR_point queue_getpoint(queue* que)
{
	WT_IR_point pt = peek_queue(que);
   int next;

	if ( ! is_valid_point(pt)) return pt;

   next = (que->front + 1) % NUM_OF_POINTS;

   if (que->front == que->rear) reset_queue(que);
   else que->front = next;

  	return pt;
}

WT_IR_point peek_queue(queue* que)
{
	if (que->front == que->rear && que->front == -1)
      return null_point;

   return que->points[que->front];
}

bool is_valid_point(WT_IR_point pt)
{
	return pt.x!=null_point.x && pt.y!=null_point.y;
}
