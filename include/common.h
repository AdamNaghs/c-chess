#ifndef _COMMON_H
#define _COMMON_H
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
        short int x, y;
} Vec2;

typedef struct
{
        Vec2 *arr;
        int len;
} Vec2Array;

typedef struct
{
        char **arr;
        int len;
} StrArray;

void free_str_array(StrArray str_array);

/* gets from stdin*/
char *input(char final_char, size_t *ret_len);

#endif