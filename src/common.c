#include "common.h"

/* gets from stdin*/
char *input(char final_char, size_t *ret_len)
{
        size_t len = 0, cap = 10;
        char *ret = (char *)malloc(sizeof(char) * cap), *tmp;

        char c;
        while ((c = getchar()) != EOF)
        {
                if (c == 0)
                        break;
                if (c == final_char)
                        break;
                if (len >= cap)
                {
                        cap *= 2;
                        tmp = (char *)realloc(ret, sizeof(char) * cap);
                        if (!tmp)
                        {
                                perror("Cannot allocate for memory for string in 'input'\n");
                                exit(1);
                        }
                        ret = tmp;
                }
                ret[len++] = c;
        }
        if (len >= cap)
        {
                tmp = (char *)realloc(ret, sizeof(char) * len + 1);
                if (!tmp)
                {
                        perror("Cannot allocate for memory for string in 'input'\n");
                        exit(1);
                }
                ret = tmp;
        }
        ret[len] = 0;
        *ret_len = len;
        return ret;
}

void free_str_array(StrArray str_array)
{
        int i;
        for (i = 0; i < str_array.len; i++)
        {
                free(str_array.arr[i]);
        }
        free(str_array.arr);
}