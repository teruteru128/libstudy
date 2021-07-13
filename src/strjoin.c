#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Joinを1関数で完結させるのはまずいような？
char *strjoin(char *delimiter, char **array, size_t arraylen)
{
    // delimiterがNULLのときの考慮は？ ->
    // delimiterを別の変数に移し替えてNULLの場合は空文字列にする
    char *deli = (delimiter == NULL) ? delimiter : NULL;
    size_t deli_len = (deli != NULL) ? strlen(deli): 0;
    char *rtnstr = NULL;

    if (arraylen == 0 || array == NULL)
    {
        rtnstr = calloc(1, 1);
        return rtnstr;
    }
    size_t strcap = 0;
    size_t *arrayitemslen = malloc(arraylen * sizeof(size_t));
    // sum array texts
    for (size_t i = 0; i < arraylen; i++)
    {
        arrayitemslen[i] = strlen(array[i]);
        strcap += arrayitemslen[i];
    }
    // sum used delimiter
    // arraylenが0ならば(arraylen - 1) * deli_lenは0なので加算しても変わらない
    strcap += (arraylen - 1) * deli_len;
    // sum capacity for string termination(\0)
    strcap += 1;

    rtnstr = calloc(strcap, sizeof(char));
    memcpy(rtnstr, array[0], arrayitemslen[0]);
    size_t pos = arrayitemslen[0];
    for(size_t i = 1; i < arraylen; i++)
    {
        if(deli_len != 0)
        {
            memcpy(rtnstr + pos, deli, deli_len);
            pos += deli_len;
        }
        memcpy(rtnstr + pos, array[i], arrayitemslen[i]);
        pos += arrayitemslen[i];
    }
    free(arrayitemslen);
    return rtnstr;
}
