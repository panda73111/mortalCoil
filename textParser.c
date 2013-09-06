#include "textParser.h"

void parseFlashVarStr(wchar_t* flashVarStr, SerMatrix* sm)
{
    int len = wcslen(flashVarStr);
    int i;
    int serLen = 0;

    sm->width = 0;
    sm->height = 0;
    sm->serMatrixStr = NULL;

    for (i = 0; i < len; i++)
    {
        if (flashVarStr[i] == L'=')
        {
            if ((i > 1 && wcsncmp(flashVarStr + i - 2, L"&x", 2) == 0) || (i == 1 && flashVarStr[i - 1] == L'x'))
            {
                if (swscanf(flashVarStr + i + 1, L"%u", &sm->width) != 1)
                    fatalError(L"Couldn't parse matrix width!");

                i++;
            }
            else if ((i > 1 && wcsncmp(flashVarStr + i - 2, L"&y", 2) == 0) || (i == 1 && flashVarStr[i - 1] == L'y'))
            {
                if (swscanf(flashVarStr + i + 1, L"%u", &sm->height) != 1)
                    fatalError(L"Couldn't parse matrix height!");

                i++;
            }
            else if ((i > 5 && wcsncmp(flashVarStr + i - 6, L"&board", 6) == 0)
                    || (i == 5 && wcsncmp(flashVarStr + i - 5, L"board", 5) == 0))
            {
                i++;
                while (i + serLen < len && flashVarStr[i + serLen] != L'&')
                    serLen++;
                sm->serMatrixStr = malloc((serLen + 1) * sizeof(wchar_t));
                if (sm == NULL)
                    fatalError(L"malloc() of sm->serMatrixStr failed!");
                wcsncpy(sm->serMatrixStr, flashVarStr + i, serLen);
                sm->serMatrixStr[serLen] = L'\0';

                i += serLen;
            }
        }
    }

    if (sm->width == 0 || sm->height == 0 || sm->serMatrixStr == NULL)
        fatalError(L"Couldn't parse flashVarString!");
}

