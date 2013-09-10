#include "mortalCoil.h"

int main(int argc, char** args)
{

    wchar_t* flashVarStr[MAX_SERMATRIX_STRLEN];
    unsigned short level = 20;
    FILE* matrixFile;

    SerMatrix sm;
    FieldMatrix m;
    wchar_t* mStr;
    Solution s;
    clock_t startTime, timeDiff;
    int mSecs;

    matrixFile = fopen("

    setlocale(LC_ALL, "");

    parseFlashVarStr(flashVarStr, &sm);
    parseMatrix(&sm, &m);

    wprintf(L"loaded level %d: {%d|%d}\n", level, m.width, m.height);

    mStr = malloc((m.size + m.height) * sizeof(wchar_t));
    if (mStr == NULL)
        fatalError(L"malloc() of mStr failed!");

    sprintMatrix(mStr, &m);
    wprintf(L"%ls\n\n", mStr);

    startTime = clock();
    if (solve(&m, &s))
    {
        sprintQPath(mStr, &m, &s);
        wprintf(L"%ls\n\nFound solution: {%d|%d}, %ls\n", mStr, s.startPoint.x, s.startPoint.y, s.qpath);
    }
    else
    {
        wprintf(L"No solution found!\n");
    }
    timeDiff = clock() - startTime;
    mSecs = timeDiff * 1000 / CLOCKS_PER_SEC;
    wprintf(L"Time taken: %d.%03dsec\n", mSecs / 1000, mSecs % 1000);

    /* clean up */
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

    for (y = 0; y < matrix->height; y++)
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
