#ifndef H_MORTALCOIL
#define H_MORTALCOIL

#include "global.h"
#include "textParser.h"
#include "fieldMatrix.h"
#include "startPointQueue.h"
#include "backtrackSolver.h"

int main(int argc, char** args);
bool solve(FieldMatrix* matrix, Solution* sol);

#endif
