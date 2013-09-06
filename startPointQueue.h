#ifndef H_STARTPOINTQUEUE
#define H_STARTPOINTQUEUE

#include "global.h"

void enqeueStartPoint(Point* p);
bool tryDequeueStartPoint(Point* p);
void clearStartPointQueue();
bool isStartPointQueueEmpty();

#endif
