#include "countStack.h"

uint pointer;
DirectedPoint stack[MAX_COUNT_STACK_SIZE];

void pushCountField(DirectedPoint* point)
{
	if (pointer == MAX_COUNT_STACK_SIZE)
		fatalError(L"countStack overflow!");

	stack[pointer++] = *point;
}

bool tryPopCountField(DirectedPoint* point)
{
	if (pointer == 0)
		return false;

	*point = stack[--pointer];
	return true;
}

void clearCountStack()
{
	pointer = 0;
}

bool isCountStackEmpty()
{
	return pointer == 0;
}
