#ifndef _MOVE_H
#define _MOVE_H

#include "common.h"

typedef struct
{
        Vec2Array passive_moves; /* cannot attack ememy pieces */
        Vec2Array hostile_moves; /* can attack enemy pieces */
        unsigned int dist;       /* multiplier */
} MoveSet;

typedef struct
{
        Vec2 v;
        int take; /* 1 if the move will result in taking a hostile piece */
} Move;

/* used for move maps */
typedef struct
{
        Move *moves;
        int len;
} MoveArray;

typedef struct LegalMove
{
        Vec2 origin_sqaure;
        Move move;
        struct LegalMove* next;
} LegalMove;

typedef struct
{
        LegalMove *arr;
        int len;
} LegalMoveArray;

void legal_move_add_next(LegalMove* dest, LegalMove add);

void free_legal_move_array(LegalMoveArray *lma);

#endif