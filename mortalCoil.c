#include "mortalCoil.h"
/* #define EMBEDDED_SERMATRIX */

int main(int argc, char** args)
{
	char* flashVarStr;
	unsigned short level = 120;

	SerMatrix sm;
	FieldMatrix m;
	wchar_t* mStr;
	Solution s;
	clock_t startTime, timeDiff;
	int mSecs;
#ifndef EMBEDDED_SERMATRIX
	char* serMatrixFilePath;
	FILE* matrixFile;
#endif

#ifdef _WIN32
	/* see http://wiki.eclipse.org/CDT/User/FAQ#Eclipse_console_does_not_show_output_on_Windows */
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
#endif

	flashVarStr = malloc((MAX_SERMATRIX_STRLEN + 1) * sizeof(char));
	if (flashVarStr == NULL)
	{
		fatalError(L"malloc() of flashVarStr failed!");
	}

#ifdef EMBEDDED_SERMATRIX
	snprintf(flashVarStr, MAX_SERMATRIX_STRLEN,
			"x=21&y=19&board=X....X....X...XX...XX..XX...XX...X....X..X.X.......X....X.....X...X.......X...XX.X..X..X..X......X....X..X.XXX...........X....X......X....X.XX........X...XX.....XX.X....X..XX.........X.X....XX......X...X...XXX...X..XXX...X..X..XX....X.........X....X..X........XXX....X.X....X...XX.XX.....X..XX..X...XX....XX...X.X.....X.....X....X.X...X.......X...X..X.X..XX............XX..X..............X....XX....");
#else
	serMatrixFilePath = (char*) malloc(
			(MAX_SERMATRIX_STRLEN + 1) * sizeof(char));
	if (serMatrixFilePath == NULL)
	{
		fatalError(L"malloc() of serMatrixFilePath failed!");
	}

#ifdef _WIN32
	snprintf(serMatrixFilePath, MAX_PATH, "%s\\level%d.matrix", getenv("USERPROFILE"), level);
#else
	snprintf(serMatrixFilePath, MAX_SERMATRIX_STRLEN, "%s/level%d.matrix",
			getenv("HOME"), level);
#endif
	matrixFile = fopen(serMatrixFilePath, "r");
	if (matrixFile == NULL)
	{
		fatalError(L"Couldn't open matrix file!");
	}
	if (fread(flashVarStr, sizeof(char), MAX_SERMATRIX_STRLEN, matrixFile) == 0)
	{
		fatalError(L"Couldn't read matrix file!");
	}
#endif

	if (setlocale(LC_ALL, "") == NULL)
	    wprintf(L"setlocale() failed!");

	parseFlashVarStr(flashVarStr, &sm);
	parseMatrix(&sm, &m);

	wprintf(L"loaded level %d: {%d|%d}\n", level, m.width, m.height);

	mStr = malloc((m.size + m.height) * sizeof(wchar_t));
	if (mStr == NULL)
		fatalError(L"malloc() of mStr failed!");

	wsprintMatrix(mStr, &m);
	wprintf(mStr);
	wprintf(L"\n\n");

	startTime = clock();
	if (solve(&m, &s))
	{
		wsprintQPath(mStr, &m, &s);
		wprintf(mStr);
		wprintf(L"\n\nFound solution: {%d|%d}, %s\n", s.startPoint.x,
				s.startPoint.y, s.qpath);
	}
	else
	{
		wprintf(L"No solution found!\n");
	}
	timeDiff = clock() - startTime;
	mSecs = timeDiff * 1000 / CLOCKS_PER_SEC;
	wprintf(L"Time taken: %d.%03dsec\n", mSecs / 1000, mSecs % 1000);

	/* clean up */
	free(flashVarStr);
	free(mStr);
	freeSerMatrix(&sm);
	freeFieldMatrix(&m);

	return 0;
}

bool solve(FieldMatrix* matrix, Solution* sol)
{
	int x, y;
	Point p;
	SolvingState state;

	state.threadId = 0;
	state.matrix = matrix;

	for (y = matrix->firstFreeField.y; y < matrix->height; y++)
		for (x = 0; x < matrix->width; x++)
		{
			if (matrix->fields[x][y] == EMPTY)
			{
				p.x = x;
				p.y = y;
				enqeueStartPoint(&p);
			}
		}

	if (solveByBacktracking(&state))
	{
		clearStartPointQueue();
		sol->startPoint = state.startPoint;
		toQPath(sol->qpath, &state);
		return true;
	}

	return false;
}
