#include "mortalCoil.h"

int main(int argc, char** args)
{

    wchar_t* flashVarStr =
            L"x=54&y=53&board=XXX....XX...XX.............X.....XXXX....X..XX.....X..XXX.XX....X....X.....XX....X.XXX.XX...XX....XX..XX......X.....XXXX...X...X.XX.X..X..X..XX.....X...XXX...X.........XXX..X.X.X...X....X..XX.X.XX..........X......XX....XXX.....X.X.X.....X..........XX.X...........X.X............XX...X....X..XXX....XX...............X...XX...XXXXX.X..XX....XX.X.XXXXXXX.X.....X....XXX......X...X...XX..XX.XX.....X......XXXX...XX....XX.XXX..X.X..........X........X.X....X...XXXXX.....X...X.XXXX.X.X..X...X.X......XXX...XXXXXX...XXX...XX....X...X......XX...X...........XXX...........XX.........X......X...XX.......X...XXXX...X........X.....X.X.....X..XX..XXX.X..X.XXXX..X.XX.....X..XXX........XX...X...XX....XX...X.XX...XX.......XXXXXX.....XX...X...XXXXXX...X...X..XX..X..X...XX...X...XXX..XX..XXXXXX............X...X...X.XX......X..X...X.XXX.XXX..XXXXXX..X.....X........XX...X..XXXXX..X..X...X...XXXXXXXXXXX..X...X...XX...X.X....X.....XX....X........XXXX.........XXX.....XXXX.....XX.....X.XXXXX...X.X....XXXX.X.......XXXX....XXXXX...X....XX.X.......XXX...........X...XXXX.....X..X.....X...XX..X...X....XX.......XX....XXX......XXXX...X.XXX...XX....XX........X.......XX..X........XXXX...X...X........X...X.X.X..X...XXXXX..XX......X......X..........X.......X.X.X.X..XX...X.....XX..X...X..X.....XX.....X.X....X..X.....XX....X...XXX...X...XXXXX...XX....XXXXX...X.....XX.X...X.XXX...XX...X......XXX..X...X....XX.......X.X.XX...X.X.....X.XX.X......X.....XXXX...XX.XX.......X..........XXX........XXXXXX....X..X..XX..XX.XXX.........X......XXXX....X......XXX.......X......X..X...X.....X......X..XXXXXX......X.........X.X......X....XX..X........X.XX.........XXX.XXXXXX......X.....X......X....XX........X..XX...X.XXX.X.....XX...XX.X.....X......XX.........X.......X...XX..X.XXX.X.....X....X.X....X...X..XXXXX..XX....XXX......X....X...X.X....X...X.X....X.X...X..X..XX..XX....XX..XX.XX...XXX..X....XX.....XX.X...X....X..X...X..XXXXX.....XXX....XX....X.....XX.........XXX....X.X.X.XXX..X.........X..XXX.X..XXXX....X.XXXXX...XX...X.......X...XXXXXXX..X......XX....XXX..X..XXXXX....XX....X.X....X...X...X....X.....X..X......XX..X....X......X.....XXX......X...X....XXX..X..........X.X..XX..X....XX........X...XXXX...X..XX...X.....X....X.XX.X..XX.X.......X..XXX....XX......XXX.........XXX....XX...X.....X.XXX.X......XX....X........XXXX........X...XXXX...X..X...X...XXXX....X......XX..........X......X......X...X....XX....XXX.....X....XX.XX.XXXXX....XX..XXXXX.....XX.XXXXX.XX.X..........X.....X....X...X.XX..XXX...XX.....X..XX....X..X.....X.....X...XX...X.....XXXX..XX...X..........XXXX...X.XXXXXX......XXX.....X....X..X...XXXXXX....XX..X.....X......X.X.....X....X...XX...X........X..X....X..........XXX.....X.X...X........X....X......X........XXX...XXX......XX...X..XX...XXXX...X....X...X..XXX..X.....X...X.......X......XX....X...XXX......X.X.......X..XX.....X.......X....XXXX..X...X.......XXXX.......";
    unsigned short level = 151;

    SerMatrix sm;
    FieldMatrix m;
    wchar_t* mStr;
    Solution s;
    clock_t startTime, timeDiff;
    int mSecs;

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
