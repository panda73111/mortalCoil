#include "global.h"

void fatalError(const wchar_t* fmt, ...)
{
    va_list argp;

    fwprintf(stderr, L"ERROR: ");
    va_start(argp, fmt);
    vfwprintf(stderr, fmt, argp);
    va_end(argp);
    fwprintf(stderr, L"\n");

    exit(1);
}

void warning(const wchar_t* fmt, ...)
{

}

void debug(const wchar_t* fmt, ...)
{
    va_list argp;

    wprintf(L"DEBUG: ");
    va_start(argp, fmt);
    vwprintf(fmt, argp);
    va_end(argp);
    wprintf(L"\n");
}

void setCursorPos(uint x, uint y)
{
#ifdef WIN32
    HANDLE hStdout;
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
    csbiInfo.dwCursorPosition.X = x;
    csbiInfo.dwCursorPosition.Y = y;
    SetConsoleCursorPosition(hStdout, csbiInfo.dwCursorPosition);
#else
    wprintf(L"%c[%d;%df", 0x1B, y, x);
#endif
}

void**
alloc2dArray(uint w, uint h, size_t typeSize)
{
    int x;
    void* colP;

    size_t headSize = w * sizeof(void*);
    size_t colSize = h * typeSize;
    size_t bodySize = w * colSize;
    void** p = malloc(headSize + bodySize);
    if (p == NULL)
        fatalError(L"malloc() of 2D-Array failed");

    for (x = 0; x < w; x++)
    {
        colP = (uint8_t*) p + headSize + x * colSize;
        *(p + x) = colP;
    }

    return p;
}

void copy2dArray(void** target, void** source, uint w, uint h, size_t typeSize)
{
    size_t headSize = w * sizeof(void*);
    size_t bodySize = w * h * typeSize;
    void* targetBody = (uint8_t*) target + headSize;
    void* sourceBody = (uint8_t*) source + headSize;
    memcpy(targetBody, sourceBody, bodySize);
}

void clear2dArray(void** target, uint w, uint h, size_t typeSize)
{
    size_t headSize = w * sizeof(void*);
    size_t bodySize = w * h * typeSize;
    void* targetBody = (uint8_t*) target + headSize;
    memset(targetBody, 0, bodySize);
}
