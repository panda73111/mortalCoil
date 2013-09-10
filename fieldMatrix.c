#include "fieldMatrix.h"

wchar_t debugStr[1024];

void set(uint x, uint y, FieldType value, FieldMatrix* matrix)
{
    if (matrix->fields[x][y] == value)
        debug(L"double set: {%u|%u}", x, y);

    matrix->fields[x][y] = value;
    setConnections(x, y, matrix);

    /* assuming that only changes are set! */
    if (value == EMPTY)
    {
        matrix->freeFieldCount++;
    }
    else
    {
        matrix->freeFieldCount--;
    }

    checkDeadEnd(x, y, matrix);

    /* range 2, diagonally
     if (x + 2 < matrix->width && y + 2 < matrix->height)
     checkDeadEnd(x + 2, y + 2, matrix);
     if (x > 1 && y + 2 < matrix->height)
     checkDeadEnd(x - 2, y + 2, matrix);
     if (x > 1 && y > 1)
     checkDeadEnd(x - 2, y - 2, matrix);
     if (x + 2 < matrix->width && y > 1)
     checkDeadEnd(x + 2, y - 2, matrix);
     */

    if (x + 2 < matrix->width)
        checkDeadEnd(x + 2, y, matrix);
    if (y + 2 < matrix->height)
        checkDeadEnd(x, y + 2, matrix);
    if (x > 1)
        checkDeadEnd(x - 2, y, matrix);
    if (y > 1)
        checkDeadEnd(x, y - 2, matrix);

    if (x + 1 < matrix->width && y + 1 < matrix->height)
        checkDeadEnd(x + 1, y + 1, matrix);
    if (x > 0 && y + 1 < matrix->height)
        checkDeadEnd(x - 1, y + 1, matrix);
    if (x > 0 && y > 0)
        checkDeadEnd(x - 1, y - 1, matrix);
    if (x + 1 < matrix->width && y > 0)
        checkDeadEnd(x + 1, y - 1, matrix);

    if (x + 1 < matrix->width)
        checkDeadEnd(x + 1, y, matrix);
    if (y + 1 < matrix->height)
        checkDeadEnd(x, y + 1, matrix);
    if (x > 0)
        checkDeadEnd(x - 1, y, matrix);
    if (y > 0)
        checkDeadEnd(x, y - 1, matrix);

}

void allocMatrix(FieldMatrix* matrix)
{
    uint w = matrix->width;
    uint h = matrix->height;

    matrix->fields = (FieldType**) alloc2dArray(w, h, sizeof(FieldType));
    matrix->initConnections = (bool**) alloc2dArray(2 * w - 1, h, sizeof(bool));
    matrix->connections = (bool**) alloc2dArray(2 * w - 1, h, sizeof(bool));
    matrix->initDeadEnds = (bool**) alloc2dArray(w, h, sizeof(bool));
    matrix->deadEnds = (bool**) alloc2dArray(w, h, sizeof(bool));
    matrix->countedFields = (bool**) alloc2dArray(w, h, sizeof(bool));
}

void freeFieldMatrix(FieldMatrix* matrix)
{
    free(matrix->fields);
    free(matrix->initConnections);
    free(matrix->connections);
    free(matrix->initDeadEnds);
    free(matrix->deadEnds);
    free(matrix->countedFields);
    matrix->fields = NULL;
    matrix->initConnections = NULL;
    matrix->connections = NULL;
    matrix->initDeadEnds = NULL;
    matrix->deadEnds = NULL;
    matrix->countedFields = NULL;
}

void freeSerMatrix(SerMatrix* matrix)
{
    free(matrix->serMatrixStr);
}

void parseMatrix(SerMatrix* source, FieldMatrix* target)
{
    uint w = source->width;
    uint h = source->height;
    uint x, y, i = 0;
    uint freeFieldCount = 0;
    bool firstFreeSet = false;
    double suboptPerc, prevSuboptPerc;

    target->width = w;
    target->height = h;
    target->size = w * h;
    target->fields = NULL;

    allocMatrix(target);

    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
        {
            if (source->serMatrixStr[i] == L'X')
            {
                target->fields[x][y] = BLOCKED;
            }
            else
            {
                target->fields[x][y] = EMPTY;
                if (!firstFreeSet)
                {
                    target->firstFreeField.x = x;
                    target->firstFreeField.y = y;
                    firstFreeSet = true;
                }
                freeFieldCount++;
            }
            i++;
        }
    target->initFreeFieldCount = freeFieldCount;
    target->freeFieldCount = freeFieldCount;

    setInitConnections(target);
    suboptPerc = getSuboptimalFieldPercentage(target);
    debug(L"Percentage of not optimal fields: %0.02f", suboptPerc * 100.0);
    optimizeInitConnections(target);
    debug(L"After optimization: %0.02f", suboptPerc * 100.0);
    copy2dArray((void**) target->connections, (void**) target->initConnections, 2 * w - 1, h, sizeof(bool));

    checkInitDeadEnds(target);
    copy2dArray((void**) target->deadEnds, (void**) target->initDeadEnds, w, h, sizeof(bool));
}

void sprintMatrix(wchar_t* str, FieldMatrix* matrix)
{
    int x, y, i = 0;
    for (y = 0; y < matrix->height; y++)
    {
        for (x = 0; x < matrix->width; x++)
        {
            switch (matrix->fields[x][y])
            {
                case EMPTY:
                    str[i] = EMPTY_CHAR;
                    break;
                case VISITED:
                    str[i] = VISITED_CHAR;
                    break;
                case BLOCKED:
                    str[i] = BLOCKED_CHAR;
                    break;
            }
            i++;
        }
        if (y != matrix->height - 1)
            str[i++] = L'\n';
    }
    str[i] = L'\0';
}

void setInitConnections(FieldMatrix* matrix)
{
    int x, y;
    for (y = 0; y < matrix->height; y++)
        for (x = 0; x < matrix->width; x++)
        {
            if (matrix->fields[x][y] != EMPTY)
                continue;

            /* to the right? */
            if (x + 1 < matrix->width && matrix->fields[x + 1][y] == EMPTY)
                matrix->initConnections[2 * x + 1][y] = true;
            /* to the bottom? */
            if (y + 1 < matrix->height && matrix->fields[x][y + 1] == EMPTY)
                matrix->initConnections[2 * x][y] = true;
        }
}

void checkInitDeadEnds(FieldMatrix* matrix)
{
    uint deadEndCount = 0;
    int x, y;
    for (y = 0; y < matrix->height; y++)
        for (x = 0; x < matrix->width; x++)
        {
            if (isSingleDirection(getConnections(x, y, matrix)))
            {
                matrix->initDeadEnds[x][y] = true;
                deadEndCount++;
            }
        }
    matrix->deadEndCount = deadEndCount;
}

Directions getConnections(uint x, uint y, FieldMatrix* matrix)
{
    byte ret = NONE;
    /* to the right? */
    if (x + 1 < matrix->width && matrix->connections[2 * x + 1][y])
        ret |= RIGHT;
    /* to the bottom? */
    if (y + 1 < matrix->height && matrix->connections[2 * x][y])
        ret |= DOWN;
    /* to the left? */
    if (x > 0 && matrix->connections[2 * x - 1][y])
        ret |= LEFT;
    /* to the top? */
    if (y > 0 && matrix->connections[2 * x][y - 1])
        ret |= UP;
    return ret;
}

void setConnections(uint x, uint y, FieldMatrix* matrix)
{
    if (matrix->fields[x][y] == EMPTY)
    {
        /* set new connections */

        /* right? */
        if (x + 1 < matrix->width && matrix->fields[x + 1][y] == EMPTY)
            matrix->connections[2 * x + 1][y] = true;
        /* down? */
        if (y + 1 < matrix->height && matrix->fields[x][y + 1] == EMPTY)
            matrix->connections[2 * x][y] = true;
        /* left? */
        if (x > 0 && matrix->fields[x - 1][y] == EMPTY)
            matrix->connections[2 * x - 1][y] = true;
        /* up? */
        if (y > 0 && matrix->fields[x][y - 1] == EMPTY)
            matrix->connections[2 * x][y - 1] = true;
    }
    else
    {
        /* unset old connections */

        /* right */
        if (x + 1 < matrix->width)
            matrix->connections[2 * x + 1][y] = false;
        /* down */
        if (y + 1 < matrix->height)
            matrix->connections[2 * x][y] = false;
        /* left */
        if (x > 0)
            matrix->connections[2 * x - 1][y] = false;
        /* up */
        if (y > 0)
            matrix->connections[2 * x][y - 1] = false;
    }
}

void printConnections(uint x, uint y, FieldMatrix* matrix)
{
    int i = 6;
    Directions dirs = getConnections(x, y, matrix);

    if (dirs == NONE)
    {
        wprintf(L"{%d|%d} none\n", x, y);
    }
    else
    {
        wchar_t ret[31];
        swprintf(ret, 13, L"{%u|%u} ", x, y);

        if (x >= 1000)
            i += 3;
        else if (x >= 100)
            i += 2;
        else if (x >= 10)
            i += 1;

        if (y >= 1000)
            i += 3;
        else if (y >= 100)
            i += 2;
        else if (y >= 10)
            i += 1;

        if ((dirs & RIGHT) != 0)
        {
            swprintf(ret + i, 7, L"right ");
            i += 6;
        }
        if ((dirs & DOWN) != 0)
        {
            swprintf(ret + i, 6, L"down ");
            i += 5;
        }
        if ((dirs & LEFT) != 0)
        {
            swprintf(ret + i, 6, L"left ");
            i += 5;
        }
        if ((dirs & UP) != 0)
        {
            swprintf(ret + i, 3, L"up");
        }
        wprintf(L"%ls\n", ret);
    }
}

void checkDeadEnd(uint x, uint y, FieldMatrix* matrix)
{
    if (isSingleDirection(getConnections(x, y, matrix)))
    {
        if (!matrix->deadEnds[x][y])
        {
            matrix->deadEnds[x][y] = true;
            matrix->deadEndCount++;
        }
    }
    else if (matrix->deadEnds[x][y])
    {
        matrix->deadEnds[x][y] = false;
        matrix->deadEndCount--;
    }
}

bool isSingleDirection(Directions dir)
{
    return (dir & 0x0F) != NONE && (dir & (dir - 1)) == 0;
}

bool isValidQPath(wchar_t* qpath)
{
    int i, len = wcslen(qpath);
    for (i = 0; i < len; i++)
    {
        switch (qpath[i])
        {
            case L'R':
            case L'D':
            case L'L':
            case L'U':
                break;
            default:
                return false;
        }
    }
    return true;
}

bool isValidPath(wchar_t* path)
{
    int i;
    size_t len = wcslen(path);
    Directions prevDir = NONE;

    if (len == 0)
        return false;

    for (i = 0; i < len; i++)
    {
        switch (path[i])
        {
            case L'R':
                if (prevDir == LEFT || prevDir == RIGHT)
                    return false;
                prevDir = RIGHT;
                break;
            case L'D':
                if (prevDir == UP || prevDir == DOWN)
                    return false;
                prevDir = DOWN;
                break;
            case L'L':
                if (prevDir == RIGHT || prevDir == LEFT)
                    return false;
                prevDir = LEFT;
                break;
            case L'U':
                if (prevDir == DOWN || prevDir == UP)
                    return false;
                prevDir = UP;
                break;
            default:
                return false;
        }
    }
    return true;
}

wchar_t getDirectionChar(Directions from, Directions to)
{
    switch (from)
    {
        case RIGHT:
            switch (to)
            {
                case RIGHT:
                    return L'\x2500';
                case DOWN:
                    return L'\x2510';
                case UP:
                    return L'\x2518';
                default:
                    fatalError(L"Invalid 'to' direction!");
            }
            break;
        case DOWN:
            switch (to)
            {
                case RIGHT:
                    return L'\x2514';
                case DOWN:
                    return L'\x2502';
                case LEFT:
                    return L'\x2518';
                default:
                    fatalError(L"Invalid 'to' direction!");
            }
            break;
        case LEFT:
            switch (to)
            {
                case DOWN:
                    return L'\x250C';
                case LEFT:
                    return L'\x2500';
                case UP:
                    return L'\x2514';
                default:
                    fatalError(L"Invalid 'to' direction!");
            }
            break;
        case UP:
            switch (to)
            {
                case RIGHT:
                    return L'\x250C';
                case LEFT:
                    return L'\x2510';
                case UP:
                    return L'\x2502';
                default:
                    fatalError(L"Invalid 'to' direction!");
            }
            break;
        case NONE:
            switch (to)
            {
                case RIGHT:
                    return L'\x2500';
                case DOWN:
                    return L'\x2502';
                case LEFT:
                    return L'\x2500';
                case UP:
                    return L'\x2502';
                default:
                    fatalError(L"Invalid 'to' direction!");
            }
            break;
        default:
            fatalError(L"Invalid 'from' direction!");
    }
    return 0; /* not gonna happen */
}

void sprintQPath(wchar_t* str, FieldMatrix* matrix, Solution* s)
{
    uint x, y, w, h, startX, startY;
    int i, pathLen;
    Directions dir, firstDir, prevDir;
    bool moved;

    if (!isValidQPath(s->qpath))
        fatalError(L"Not a valid qpath!");

    x = s->startPoint.x;
    y = s->startPoint.y;
    if (matrix->fields[x][y] != EMPTY)
        fatalError(L"qpath starting point is not empty!");

    sprintMatrix(str, matrix);

    startX = x;
    startY = y;
    w = matrix->width;
    h = matrix->height;
    pathLen = wcslen(s->qpath);
    firstDir = NONE;
    prevDir = NONE;

    for (i = 0; i < pathLen + 1; i++)
    {
        while (isSingleDirection(dir = getConnections(x, y, matrix)))
        {
            /* move in that single direction */
            moved = false;
            switch (dir)
            {
                case RIGHT:
                    while (x + 1 < w && matrix->fields[x + 1][y] == EMPTY)
                    {
                        moved = true;
                        set(x, y, VISITED, matrix);
                        str[y * (w + 1) + x] = getDirectionChar(prevDir, dir);
                        x++;
                        prevDir = dir;
                    }
                    break;
                case DOWN:
                    while (y + 1 < h && matrix->fields[x][y + 1] == EMPTY)
                    {
                        moved = true;
                        set(x, y, VISITED, matrix);
                        str[y * (w + 1) + x] = getDirectionChar(prevDir, dir);
                        y++;
                        prevDir = dir;
                    }
                    break;
                case LEFT:
                    while (x > 0 && matrix->fields[x - 1][y] == EMPTY)
                    {
                        moved = true;
                        set(x, y, VISITED, matrix);
                        str[y * (w + 1) + x] = getDirectionChar(prevDir, dir);
                        x--;
                        prevDir = dir;
                    }
                    break;
                case UP:
                    while (y > 0 && matrix->fields[x][y - 1] == EMPTY)
                    {
                        moved = true;
                        set(x, y, VISITED, matrix);
                        str[y * (w + 1) + x] = getDirectionChar(prevDir, dir);
                        y--;
                        prevDir = dir;
                    }
                    break;
                default:
                    break;
            }
            if (!moved)
                fatalError(L"qpath is blocked!");

            if (firstDir == NONE)
                firstDir = dir;
        }

        if (i < pathLen)
        {
            moved = false;

            switch (s->qpath[i])
            {
                case L'R':
                    dir = RIGHT;
                    while (x + 1 < w && matrix->fields[x + 1][y] == EMPTY)
                    {
                        moved = true;
                        set(x, y, VISITED, matrix);
                        str[y * (w + 1) + x] = getDirectionChar(prevDir, dir);
                        x++;
                        prevDir = dir;
                    }
                    break;
                case L'D':
                    dir = DOWN;
                    while (y + 1 < h && matrix->fields[x][y + 1] == EMPTY)
                    {
                        moved = true;
                        set(x, y, VISITED, matrix);
                        str[y * (w + 1) + x] = getDirectionChar(prevDir, dir);
                        y++;
                        prevDir = dir;
                    }
                    break;
                case L'L':
                    dir = LEFT;
                    while (x > 0 && matrix->fields[x - 1][y] == EMPTY)
                    {
                        moved = true;
                        set(x, y, VISITED, matrix);
                        str[y * (w + 1) + x] = getDirectionChar(prevDir, dir);
                        x--;
                        prevDir = dir;
                    }
                    break;
                case L'U':
                    dir = UP;
                    while (y > 0 && matrix->fields[x][y - 1] == EMPTY)
                    {
                        moved = true;
                        set(x, y, VISITED, matrix);
                        str[y * (w + 1) + x] = getDirectionChar(prevDir, dir);
                        y--;
                        prevDir = dir;
                    }
                    break;
                default:
                    break;
            }
            if (!moved)
                fatalError(L"qpath is blocked!");

            if (firstDir == NONE)
                firstDir = dir;
        }
    }

    str[y * (w + 1) + x] = L'\x25FE'; /* end point */

    switch (firstDir)
    {
        case RIGHT:
            str[startY * (w + 1) + startX] = L'\x25BA';
            break;
        case DOWN:
            str[startY * (w + 1) + startX] = L'\x25BC';
            break;
        case LEFT:
            str[startY * (w + 1) + startX] = L'\x25C0';
            break;
        case UP:
            str[startY * (w + 1) + startX] = L'\x25B2';
            break;
        default:
            break;
    }

    /* clean up */
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
        {
            if (matrix->fields[x][y] == VISITED)
                set(x, y, EMPTY, matrix);
        }
}

bool isSolved(FieldMatrix* matrix)
{
    return matrix->freeFieldCount == 0;
}

void toQPath(wchar_t* qpath, SolvingState* s)
{
    uint x, y, w, h, startX, startY;
    int i, moveI = 0;
    bool moved;
    wchar_t* path = s->path;
    FieldMatrix* m = s->matrix;
    FieldType** f = m->fields;
    size_t pathLen;

    if (!isValidPath(path))
        fatalError(L"Not a valid path!");

    x = s->startPoint.x;
    y = s->startPoint.y;
    if (f[x][y] != EMPTY)
        fatalError(L"path starting point is not empty!");

    startX = x;
    startY = y;
    w = m->width;
    h = m->height;
    pathLen = wcslen(path);

    for (i = 0; i < pathLen; i++)
    {
        if (!isSingleDirection(getConnections(x, y, m)))
        {
            qpath[moveI++] = path[i];
        }

        moved = false;

        switch (path[i])
        {
            case L'R':
                while (x + 1 < w && f[x + 1][y] == EMPTY)
                {
                    moved = true;
                    set(x, y, VISITED, m);
                    x++;
                }
                break;
            case L'D':
                while (y + 1 < h && f[x][y + 1] == EMPTY)
                {
                    moved = true;
                    set(x, y, VISITED, m);
                    y++;
                }
                break;
            case L'L':
                while (x > 0 && f[x - 1][y] == EMPTY)
                {
                    moved = true;
                    set(x, y, VISITED, m);
                    x--;
                }
                break;
            case L'U':
                while (y > 0 && f[x][y - 1] == EMPTY)
                {
                    moved = true;
                    set(x, y, VISITED, m);
                    y--;
                }
                break;
            default:
                break;
        }

        if (!moved)
            fatalError(L"path is blocked!");
    }

    qpath[moveI] = L'\0';

    /* clean up */
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
        {
            if (f[x][y] == VISITED)
                set(x, y, EMPTY, m);
        }
}

bool isHalfed(FieldMatrix* matrix)
{
    DirectedPoint p, nextP;
    uint count = 0;

    for (p.y = matrix->firstFreeField.y; p.y < matrix->height; p.y++)
        for (p.x = 0; p.x < matrix->width; p.x++)
        {
            if (matrix->fields[p.x][p.y] == EMPTY)
            {
                /* found the first free field */

                p.dirs = getConnections(p.x, p.y, matrix);
                pushCountField(&p);
                matrix->countedFields[p.x][p.y] = true;

                while (tryPopCountField(&p))
                {
                    count++;

                    if ((p.dirs & RIGHT) != 0 && !matrix->countedFields[p.x + 1][p.y])
                    {
                        nextP.x = p.x + 1;
                        nextP.y = p.y;
                        nextP.dirs = getConnections(p.x + 1, p.y, matrix) & ~LEFT;
                        pushCountField(&nextP);
                        matrix->countedFields[nextP.x][nextP.y] = true;
                    }

                    if ((p.dirs & DOWN) != 0 && !matrix->countedFields[p.x][p.y + 1])
                    {
                        nextP.x = p.x;
                        nextP.y = p.y + 1;
                        nextP.dirs = getConnections(p.x, p.y + 1, matrix) & ~UP;
                        pushCountField(&nextP);
                        matrix->countedFields[nextP.x][nextP.y] = true;
                    }

                    if ((p.dirs & LEFT) != 0 && !matrix->countedFields[p.x - 1][p.y])
                    {
                        nextP.x = p.x - 1;
                        nextP.y = p.y;
                        nextP.dirs = getConnections(p.x - 1, p.y, matrix) & ~RIGHT;
                        pushCountField(&nextP);
                        matrix->countedFields[nextP.x][nextP.y] = true;
                    }

                    if ((p.dirs & UP) != 0 && !matrix->countedFields[p.x][p.y - 1])
                    {
                        nextP.x = p.x;
                        nextP.y = p.y - 1;
                        nextP.dirs = getConnections(p.x, p.y - 1, matrix) & ~DOWN;
                        pushCountField(&nextP);
                        matrix->countedFields[nextP.x][nextP.y] = true;
                    }
                }

                clear2dArray((void**) matrix->countedFields, matrix->width, matrix->height, sizeof(bool));

                return count != matrix->freeFieldCount;
            }
        }

    fatalError(L"Matrix is already solved!");
    return false;
}

double getSuboptimalFieldPercentage(FieldMatrix* matrix)
{
    double suboptimalCount = 0;
    uint x, y;

    for (y = matrix->firstFreeField.y; y < matrix->height; y++)
        for (x = 0; x < matrix->width; x++)
        {
            if (countInitConnections(x, y, matrix) > 2)
                suboptimalCount++;
        }

    return suboptimalCount / matrix->initFreeFieldCount;
}

uint countInitConnections(uint x, uint y, FieldMatrix* matrix)
{
    uint count = 0;
    if (x + 1 < matrix->width && matrix->initConnections[2 * x + 1][y])
        count++;
    if (y + 1 < matrix->height && matrix->initConnections[2 * x][y])
        count++;
    if (x > 0 && matrix->initConnections[2 * x - 1][y])
        count++;
    if (y > 0 && matrix->initConnections[2 * x][y - 1])
        count++;
    return count;
}

void optimizeInitConnections(FieldMatrix* matrix)
{
    debugStr[0] = L'\n';
    sprintMatrix(debugStr + 1, matrix);
    sprintSuboptimalFields(debugStr + 1, matrix);
    debug(debugStr);
}

void sprintSuboptimalFields(wchar_t* str, FieldMatrix* matrix)
{
    uint x, y, w, h;

    w = matrix->width;
    h = matrix->height;

        for (y = matrix->firstFreeField.y; y < h; y++)
            for (x = 0; x < w; x++)
            {
                if (countInitConnections(x, y, matrix) > 2)
                    str[y * (w + 1) + x] = SUBOPTIMAL_CHAR;
            }
}
