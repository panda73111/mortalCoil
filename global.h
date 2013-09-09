#ifndef H_GLOBAL
#define H_GLOBAL

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <fcntl.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define swprintf _snwprintf
#endif

#define MAX_STARTPOINT_QUEUE_SIZE 5000
#define MAX_COUNT_STACK_SIZE 10000
#define MAX_QPATH_LEN 100

typedef uint8_t byte;
typedef unsigned int uint;

typedef struct
{
    uint x;
    uint y;
} Point;

typedef enum
{
    EMPTY = 0, VISITED = 1, BLOCKED = 2
} FieldType;

typedef enum
{
    EMPTY_CHAR = L'-', VISITED_CHAR = L'O', BLOCKED_CHAR = L'X'
} FieldTypeChar;

typedef enum
{
    NONE = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 4,
    UP = 8,
    LEFT_RIGHT = LEFT | RIGHT,
    UP_DOWN = UP | DOWN,
    LEFT_UP = LEFT | UP,
    LEFT_DOWN = LEFT | DOWN,
    RIGHT_UP = RIGHT | UP,
    RIGHT_DOWN = RIGHT | DOWN,
    DOWN_LEFT = DOWN | LEFT,
    UP_RIGHT_DOWN = UP | RIGHT | DOWN,
    RIGHT_DOWN_LEFT = RIGHT | DOWN | LEFT,
    DOWN_LEFT_UP = DOWN | LEFT | UP,
    LEFT_UP_RIGHT = LEFT | UP | RIGHT,
    ALL = RIGHT | DOWN | LEFT | RIGHT
} Directions;

typedef struct
{
    wchar_t* serMatrixStr;
    uint width;
    uint height;
} SerMatrix;

typedef struct
{
    uint width;
    uint height;
    uint size;
    FieldType** fields;
    uint initFreeFieldCount;
    uint freeFieldCount;
    uint deadEndCount;
    bool** initConnections;
    bool** connections;
    bool** initDeadEnds;
    bool** deadEnds;
    bool** countedFields;
    Point firstFreeField;
} FieldMatrix;

typedef struct
{
    Point startPoint;
    wchar_t qpath[MAX_QPATH_LEN + 1];
} Solution;

typedef struct
{
    uint threadId;
    FieldMatrix* matrix;
    Point startPoint;
    wchar_t path[MAX_QPATH_LEN + 1];
    uint moveI;
    bool finished;
} SolvingState;

typedef struct
{
        uint x;
        uint y;
        Directions dirs;
} DirectedPoint;

void
fatalError(const wchar_t* fmt, ...);
void
warning(const wchar_t* fmt, ...);
void
debug(const wchar_t* fmt, ...);
void
setCursorPos(uint x, uint y);
void**
alloc2dArray(uint w, uint h, size_t typeSize);
void
copy2dArray(void** target, void** source, uint w, uint h, size_t typeSize);
void clear2dArray(void** target, uint w, uint h, size_t typeSize);

#endif

