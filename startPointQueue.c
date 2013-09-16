#include "startPointQueue.h"

uint inPointer = 0;
uint outPointer = 0;
Point queue[MAX_STARTPOINT_QUEUE_SIZE];

void enqeueStartPoint(Point* p)
{
	if ((inPointer + 1) % MAX_STARTPOINT_QUEUE_SIZE == outPointer)
		fatalError(L"startPointQueue overflow!");

	queue[inPointer] = *p;
	inPointer = (inPointer + 1) % MAX_STARTPOINT_QUEUE_SIZE;
}

bool tryDequeueStartPoint(Point* p)
{
	if (outPointer == inPointer)
		return false;

	*p = queue[outPointer];
	outPointer = (outPointer + 1) % MAX_STARTPOINT_QUEUE_SIZE;

	return true;
}

void clearStartPointQueue()
{
	inPointer = 0;
	outPointer = 0;
}

bool isStartPointQueueEmpty()
{
	return outPointer == inPointer;
}
