#include "mortalCoil.h"
#define EMBEDDED_SERMATRIX

int
main(int argc, char** args)
{

    char* flashVarStr;
    unsigned short level = 20;

    SerMatrix sm;
    FieldMatrix m;
    wchar_t* mStr;
    Solution s;
    clock_t startTime, timeDiff;
    int mSecs;

    flashVarStr = malloc(MAX_SERMATRIX_STRLEN + 1);
    if (flashVarStr == NULL )
    {
        fatalError(L"malloc() of flashVarStr failed!");
    }

#ifdef EMBEDDED_SERMATRIX
    snprintf(flashVarStr, MAX_SERMATRIX_STRLEN,
            "x=11&y=9&board=XX............XX.........X..X.........XX......XXXXX..................X........X...X............X...");
#else
    char serMatrixFilePath[MAX_PATH];
    FILE* matrixFile;

#ifdef _WIN32
    snprintf(serMatrixFilePath, MAX_PATH, "%s\\level%d.matrix", getenv("USERPROFILE"), level);
#else
    snprintf(serMatrixFilePath, MAX_PATH, "%s/level%d.matrix", getenv("HOME"), level);
#endif
    matrixFile = fopen(serMatrixFilePath, "r");
    if (matrixFile == NULL )
    {
        fatalError(L"Couldn't open matrix file!");
    }
    if (fread(flashVarStr, sizeof(char), MAX_SERMATRIX_STRLEN, matrixFile) == 0)
    {
        fatalError(L"Couldn't read matrix file!");
    }
#endif

    setlocale(LC_ALL, "");

    parseFlashVarStr(flashVarStr, &sm);
    parseMatrix(&sm, &m);

    wprintf(L"loaded level %d: {%d|%d}\n", level, m.width, m.height);

    mStr = malloc((m.size + m.height) * sizeof(wchar_t));
    if (mStr == NULL )
        fatalError(L"malloc() of mStr failed!");

    wsprintMatrix(mStr, &m);
    wprintf(L"%ls\n\n", mStr);

    startTime = clock();
    if (solve(&m, &s))
    {
        wsprintQPath(mStr, &m, &s);
        wprintf(L"%ls\n\nFound solution: {%d|%d}, %ls\n", mStr, s.startPoint.x, s.startPoint.y,
                s.qpath);
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

bool
solve(FieldMatrix* matrix, Solution* sol)
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
