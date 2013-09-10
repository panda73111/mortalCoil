#include "backtrackSolver.h"
//#define DEBUG_OUT_MOVES
wchar_t debugStr[1024];

bool solveFromPoint(SolvingState* state, uint x, uint y)
{
    FieldMatrix* m = state->matrix;
    uint tx = x, ty = y;
    uint w = m->width;
    uint h = m->height;
    FieldType** f = m->fields;
    Directions dir;
    bool solved = false;
    bool moved;

    for (dir = 1; dir <= 8; dir *= 2)
    {
        moved = false;
        switch (dir)
        {
            case RIGHT:
                while (tx + 1 < w && f[tx + 1][y] == EMPTY)
                {
                    moved = true;
                    tx++;
                    set(tx, y, VISITED, m);
                }
                break;
            case DOWN:
                while (ty + 1 < h && f[x][ty + 1] == EMPTY)
                {
                    moved = true;
                    ty++;
                    set(x, ty, VISITED, m);
                }
                break;
            case LEFT:
                while (tx > 0 && f[tx - 1][y] == EMPTY)
                {
                    moved = true;
                    tx--;
                    set(tx, y, VISITED, m);
                }
                break;
            case UP:
                while (ty > 0 && f[x][ty - 1] == EMPTY)
                {
                    moved = true;
                    ty--;
                    set(x, ty, VISITED, m);
                }
                break;
            default:
                break;
        }

        if (!moved)
            continue;

#ifdef DEBUG_OUT_MOVES
        switch (dir)
        {
            case RIGHT:
                debug(L"RIGHT");
                break;
            case DOWN:
                debug(L"DOWN");
                break;
            case LEFT:
                debug(L"LEFT");
                break;
            case UP:
                debug(L"UP");
                break;
            default:
                break;
        }
        debugStr[0] = L'\n';
        sprintMatrix(debugStr + 1, m);
        debug(debugStr);
#endif

        switch (dir)
        {
            case RIGHT:
                state->path[state->moveI++] = 'R';
                break;
            case DOWN:
                state->path[state->moveI++] = 'D';
                break;
            case LEFT:
                state->path[state->moveI++] = 'L';
                break;
            case UP:
                state->path[state->moveI++] = 'U';
                break;
            default:
                break;
        }

#ifdef DEBUG_OUT_MOVES
        debug(L"dead ends: %u", m->deadEndCount);
#endif

        if (isSolved(m))
        {
            state->path[state->moveI] = L'\0';
            solved = true;
        }
        else if (m->deadEndCount <= 2 && !isHalfed(m))
        {
            /* still solvable */
            if (solveFromPoint(state, tx, ty))
                solved = true;
        }

        /* revert the steps */
        switch (dir)
        {
            case RIGHT:
                for (; tx > x; tx--)
                    set(tx, y, EMPTY, m);
                break;
            case DOWN:
                for (; ty > y; ty--)
                    set(x, ty, EMPTY, m);
                break;
            case LEFT:
                for (; tx < x; tx++)
                    set(tx, y, EMPTY, m);
                break;
            case UP:
                for (; ty < y; ty++)
                    set(x, ty, EMPTY, m);
                break;
            default:
                break;
        }

#ifdef DEBUG_OUT_MOVES
        switch (dir)
        {
            case RIGHT:
                debug(L"BACK LEFT");
                break;
            case DOWN:
                debug(L"BACK UP");
                break;
            case LEFT:
                debug(L"BACK RIGHT");
                break;
            case UP:
                debug(L"BACK DOWN");
                break;
            default:
                break;
        }
        debugStr[0] = L'\n';
        sprintMatrix(debugStr + 1, m);
        debug(debugStr);
#endif

        if (solved)
        {
            return true;
        }

        state->moveI--;
    }

    /* moved in all directions without finding a solution */
    return false;
}

bool solveByBacktracking(SolvingState* state)
{
    Point sp;

    state->finished = false;
    state->moveI = 0;
    while (tryDequeueStartPoint(&sp))
    {
        state->startPoint = sp;
        set(sp.x, sp.y, VISITED, state->matrix);
        if (solveFromPoint(state, sp.x, sp.y))
        {
            set(sp.x, sp.y, EMPTY, state->matrix);
            state->finished = true;
            return true;
        }
        set(sp.x, sp.y, EMPTY, state->matrix);
    }
    state->finished = true;
    return false;
}
