#ifndef H_COUNTSTACK
#define H_COUNTSTACK

#include "global.h"

void pushCountField(DirectedPoint* point);
bool tryPopCountField(DirectedPoint* point);
void clearCountStack();
bool isCountStackEmpty();

#endif
