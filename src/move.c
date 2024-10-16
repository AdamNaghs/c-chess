#include "../include/move.h"
#include "../include/common.h"

void legal_move_add_next(LegalMove* dest, LegalMove add)
{
        dest->next = (LegalMove*)malloc(sizeof(LegalMove));
        *dest->next = add;
}

void free_legal_move_array(LegalMoveArray *lma)
{
        int i;
        for (i = 0; i < lma->len; i++)
        {
                if (lma->arr[i].next)
                {
                        LegalMove* node = lma->arr[i].next, *tmp = NULL;
                        while (node)
                        {
                                tmp = node->next;
                                free(node);
                                node = tmp;
                        }
                }
        }
        lma->len = 0;
        free(lma->arr);
        free(lma);
}