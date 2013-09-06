#ifndef H_BACKTRACKSOLVER
#define H_BACKTRACKSOLVER

#include "global.h"
#include "startPointQueue.h"
#include "fieldMatrix.h"

bool solveFromPoint(SolvingState* state, uint x, uint y);
bool solveByBacktracking(SolvingState* state);

#endif
