#ifndef H_FIELDMATRIX
#define H_FIELDMATRIX

#include "global.h"

void set(uint x, uint y, FieldType value, FieldMatrix* matrix);
void allocMatrix(FieldMatrix* matrix);
void freeFieldMatrix(FieldMatrix* matrix);
void freeSerMatrix(SerMatrix* matrix);
void parseMatrix(SerMatrix* source, FieldMatrix* target);
void sprintMatrix(wchar_t* str, FieldMatrix* matrix);
void setInitConnections(FieldMatrix* matrix);
void checkInitDeadEnds(FieldMatrix* matrix);
Directions getConnections(uint x, uint y, FieldMatrix* matrix);
void setConnections(uint x, uint y, FieldMatrix* matrix);
void printConnections(uint x, uint y, FieldMatrix* matrix);
void checkDeadEnd(uint x, uint y, FieldMatrix* matrix);
bool isSingleDirection(Directions dir);
bool isValidQPath(wchar_t* qpath);
bool isValidPath(wchar_t* path);
wchar_t getDirectionChar(Directions from, Directions to);
void sprintQPath(wchar_t* qpath, FieldMatrix* matrix, Solution* s);
bool isSolved(FieldMatrix* matrix);
void toQPath(wchar_t* qpath, SolvingState* s);
bool isHalfed(FieldMatrix* matrix);
uint countFromPoint(uint x, uint y, FieldMatrix* matrix);

#endif