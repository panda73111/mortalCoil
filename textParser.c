#include "textParser.h"

void
parseFlashVarStr(char* flashVarStr, SerMatrix* sm)
{
    size_t len = strlen(flashVarStr);
    int i;
    uint serLen = 0;

    sm->width = 0;
    sm->height = 0;
    sm->serMatrixStr = NULL;

    for (i = 0; i < len; i++)
    {
        if (flashVarStr[i] == '=')
        {
            if ((i > 1 && strncmp(flashVarStr + i - 2, "&x", 2) == 0)
                    || (i == 1 && flashVarStr[i - 1] == 'x'))
            {
                if (sscanf(flashVarStr + i + 1, "%u", &sm->width) != 1)
                    fatalError(L"Couldn't parse matrix width!");

                i++;
            }
            else if ((i > 1 && strncmp(flashVarStr + i - 2, "&y", 2) == 0)
                    || (i == 1 && flashVarStr[i - 1] == 'y'))
            {
                if (sscanf(flashVarStr + i + 1, "%u", &sm->height) != 1)
                    fatalError(L"Couldn't parse matrix height!");

                i++;
            }
            else if ((i > 5 && strncmp(flashVarStr + i - 6, "&board", 6) == 0)
                    || (i == 5 && strncmp(flashVarStr + i - 5, "board", 5) == 0))
            {
                i++;
                while (i + serLen < len && flashVarStr[i + serLen] != '&')
                    serLen++;
                sm->serMatrixStr = malloc((serLen + 1) * sizeof(char));
                if (sm == NULL )
                    fatalError(L"malloc() of sm->serMatrixStr failed!");
                strncpy(sm->serMatrixStr, flashVarStr + i, serLen);
                sm->serMatrixStr[serLen] = '\0';

                i += serLen;
            }
        }
    }

    if (sm->width == 0 || sm->height == 0 || sm->serMatrixStr == NULL )
        fatalError(L"Couldn't parse flashVarString!");
}

