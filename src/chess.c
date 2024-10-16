#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "chess.h"

/*  Define movement vectors for each piece type */
static const Vec2 PAWN_PASSIVE_MOVES[] = {{0, 1}};
static const Vec2 PAWN_HOSTILE_MOVES[] = {{1, 1}, {-1, 1}};
static const Vec2 BISHOP_MOVES[] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
static const Vec2 KNIGHT_MOVES[] = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};
static const Vec2 ROOK_MOVES[] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
static const Vec2 QUEEN_MOVES[] = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
static const Vec2 KING_MOVES[] = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};

/*  Define the move sets for each piece type */
static const MoveSet MOVE_SETS[] = {
    {{NULL, 0}, {NULL, 0}, 0},                                                 /*  NONE */
    {{(Vec2 *)PAWN_PASSIVE_MOVES, 1}, {(Vec2 *)PAWN_HOSTILE_MOVES, 2}, 1},     /*  PAWN */
    {{(Vec2 *)BISHOP_MOVES, 4}, {(Vec2 *)BISHOP_MOVES, 4}, CHESS_BOARD_WIDTH}, /*  BISHOP */
    {{(Vec2 *)KNIGHT_MOVES, 8}, {(Vec2 *)KNIGHT_MOVES, 8}, 1},                 /*  KNIGHT */
    {{(Vec2 *)ROOK_MOVES, 4}, {(Vec2 *)ROOK_MOVES, 4}, CHESS_BOARD_WIDTH},     /*  ROOK */
    {{(Vec2 *)QUEEN_MOVES, 8}, {(Vec2 *)QUEEN_MOVES, 8}, CHESS_BOARD_WIDTH},   /*  QUEEN */
    {{(Vec2 *)KING_MOVES, 8}, {(Vec2 *)KING_MOVES, 8}, 1},                     /*  KING */
};

#ifdef _WIN32

#define strdup(s) _strdup(s)
#define strnicmp(a, b, len) _strnicmp(a, b, len)

#else

#define strdup(s) strcpy(malloc(strlen(s) + 1),s)
#define strnicmp(a,b,len) strncmp(a,b,len)

#endif


int char_to_file(char c)
{
    if (c >= 'a')
        c -= 'a';
    else
        c -= 'A';
    if (c > (CHESS_BOARD_WIDTH - 1) || c < 0)
    {
        return -1;
    }
    return c;
}

void chess_board_populate(ChessBoard b, unsigned int i, unsigned int j)
{
    if (j == 6)
    {
        CS_SET_PIECE(b[i][j], PAWN);
        CP_SET_COLOR(b[i][j], BLACK);
    }
    else if (j == 1)
    {
        CS_SET_PIECE(b[i][j], PAWN);
        CP_SET_COLOR(b[i][j], WHITE);
    }
    else if (j == 0 || j == 7)
    {
        ChessColor piece_color = (j != 0) ? BLACK : WHITE;
        switch (i)
        {
        case 0:
        case 7:
            CS_SET_PIECE(b[i][j], ROOK);
            break;
        case 1:
        case 6:
            CS_SET_PIECE(b[i][j], KNIGHT);
            break;
        case 2:
        case 5:
            CS_SET_PIECE(b[i][j], BISHOP);
            break;
        case 3:
            CS_SET_PIECE(b[i][j], QUEEN);
            break;
        case 4:
            CS_SET_PIECE(b[i][j], KING);
            break;
        default:
            break;
        }
        CP_SET_COLOR(b[i][j], piece_color);
    }
    else
    {
        CS_SET_PIECE(b[i][j], NONE);
    }
}

void chess_board_init(ChessBoard b)
{
    unsigned int i, j;
    for (i = 0; i < CHESS_BOARD_WIDTH; i++)
    {
        for (j = 0; j < CHESS_BOARD_HEIGHT; j++)
        {
            b[i][j] = 0;
            ChessColor square_color = (i + j) % 2 ? WHITE : BLACK;
            CS_SET_COLOR(b[i][j], square_color);
            chess_board_populate(b, i, j);
        }
    }
}

void chess_board_visualize(ChessBoard b)
{
    int i, j;
    printf("Entire Map\n");
    for (j = CHESS_BOARD_HEIGHT - 1; j >= 0; j--)
    {
        for (i = 0; i < CHESS_BOARD_WIDTH; i++)
        {
            printf("%u\t", b[i][j]);
        }
        printf("\n");
    }
    printf("Threat Map\n");
    for (j = CHESS_BOARD_HEIGHT - 1; j >= 0; j--)
    {
        for (i = 0; i < CHESS_BOARD_WIDTH; i++)
        {
            printf("%u\t", CP_GET_IS_IN_THREAT(b[i][j]));
        }
        printf("\n");
    }
}

#ifndef CHESS_DISABLE_COLOR_TEXT

#define BG_BLK "\e[0;100m"
#define BG_WHT "\e[0;107m"

#define FG_BLK "\e[1;91m"
#define FG_WHT "\e[1;96m"

#define RST "\e[0m"

#else

#define BG_BLK
#define BG_WHT

#define FG_BLK
#define FG_WHT

#define RST

#endif

void chess_board_print(ChessBoard b)
{
    int i, j;
    for (j = CHESS_BOARD_HEIGHT - 1; j >= 0; j--) /*  Print from bot to top */
    {
        printf("%d ", j + 1);
        for (i = 0; i < CHESS_BOARD_WIDTH; i++) /*  Print from left to right */
        {
#ifndef CHESS_DISABLE_COLOR_TEXT
            switch (CS_GET_COLOR(b[i][j]))
            {
            case BLACK:
                printf(BG_BLK);
                break;
            case WHITE:
            default:
                printf(BG_WHT);
            }
            switch (CP_GET_COLOR(CS_GET_PIECE(b[i][j])))
            {
            case BLACK:
                printf(FG_BLK);
                break;
            case WHITE:
            default:
                printf(FG_WHT);
            }
#endif
            printf("%c" RST, type_to_char(CP_GET_TYPE(b[i][j])));
        }
        printf(RST "\n");
    }
    printf("  ");
    for (i = 0; i < CHESS_BOARD_WIDTH; i++)
        printf("%c", i + 'A');
    printf("\n");
}

void chess_board_clear_threats(ChessBoard cb)
{
    int x, y;
    for (x = 0; x < CHESS_BOARD_WIDTH; x++)
    {
        for (y = 0; y < CHESS_BOARD_HEIGHT; y++)
        {
            CP_SET_THREAT(cb[x][y], 0);
        }
    }
}

void chess_board_set_threats(ChessBoard cb, LegalMoveArray *lma)
{
    int i;
    for (i = 0; i < lma->len; i++)
    {
        Move move = lma->arr[i].move;
        Vec2 v = move.v;
        if (!move.take)
            continue;
        if (CP_GET_TYPE(cb[v.x][v.y]) == NONE)
            continue;
        if (CP_GET_COLOR(cb[v.x][v.y]) == CP_GET_COLOR(cb[lma->arr[i].origin_sqaure.x][lma->arr[i].origin_sqaure.y]))
            continue;
        CP_SET_THREAT(cb[v.x][v.y], 1);
    }
}

/* doesn't set has moved variable */
void chess_board_move_piece(ChessBoard board, LegalMove move)
{
    LegalMove *tmp = &move;
    while (tmp != NULL)
    {
        ChessPiece piece = CS_GET_PIECE(board[tmp->origin_sqaure.x][tmp->origin_sqaure.y]);
        CP_SET_TYPE(board[tmp->origin_sqaure.x][tmp->origin_sqaure.y], NONE);
        CS_SET_PIECE(board[tmp->move.v.x][tmp->move.v.y], piece);
        tmp = tmp->next;
    }
}

Vec2 chess_board_find_king(ChessBoard cb, ChessColor king_color)
{
    int x, y;
    for (x = 0; x < CHESS_BOARD_WIDTH; x++)
    {
        for (y = 0; y < CHESS_BOARD_HEIGHT; y++)
        {
            ChessPiece piece = CS_GET_PIECE(cb[x][y]);
            if (CP_GET_COLOR(piece) != king_color)
                continue;
            if (CP_GET_TYPE(piece) == KING)
                return (Vec2){x, y};
        }
    }
    perror("Could not find king on the board.\n");
    return (Vec2){-1, -1};
}

/* generate all moves that a piece at x,y on the board can make depending only on the piece move set */
void generate_moves(ChessBoard b, int x, int y, Move *return_moves, int *return_len)
{
    const ChessSquare square = b[x][y];
    if (CP_GET_TYPE(square) == NONE)
        return;
    if (!(return_moves && return_len))
        return;
    const ChessPiece piece = CS_GET_PIECE(square);
    const ChessPieceType type = CP_GET_TYPE(piece);
    const ChessColor piece_color = CP_GET_COLOR(piece);
    MoveSet move_set = MOVE_SETS[type];
    unsigned char first_pawn_move = (type == PAWN) && (CP_GET_HAS_MOVED(piece) == 0);
    /*  Generate passive moves */
    if (first_pawn_move)
        move_set.dist++;
    int i, step;
    for (i = 0; i < move_set.passive_moves.len; i++)
    {
        Vec2 vector = move_set.passive_moves.arr[i];
        for (step = 1; step <= move_set.dist; step++)
        {
            int new_x, new_y;
            if (type == PAWN && piece_color == BLACK)
                new_y = y - step * vector.y;
            else
                new_y = y + step * vector.y;
            new_x = x + step * vector.x;

            /*  Check if the new position is within the board limits */
            if (new_x < 0 || new_x >= CHESS_BOARD_WIDTH || new_y < 0 || new_y >= CHESS_BOARD_HEIGHT)
                break;

            /*  Check if the move is valid (no piece at the new position) */
            ChessPieceType dest_type = CP_GET_TYPE(b[new_x][new_y]);
            if (dest_type == NONE)
            {
                /*  Add the valid passive move */
                return_moves[(*return_len)].v = (Vec2){new_x, new_y};
                return_moves[(*return_len)++].take = 0;
            }
            else
            {
                break;
            }
        }
    }
    if (first_pawn_move)
        move_set.dist--;

    /*  Generate hostile moves */
    for (i = 0; i < move_set.hostile_moves.len; i++)
    {
        Vec2 vector = move_set.hostile_moves.arr[i];
        for (int step = 1; step <= move_set.dist; step++)
        {
            int new_x, new_y;
            if (type == PAWN && piece_color == BLACK)
                new_y = y - step * vector.y;
            else
                new_y = y + step * vector.y;
            new_x = x + step * vector.x;

            /*  Check if the new position is within the board limits */
            if (new_x < 0 || new_x >= CHESS_BOARD_WIDTH || new_y < 0 || new_y >= CHESS_BOARD_HEIGHT)
                break;

            /*  Check if the move is valid (an enemy piece at the new position) */
            if (CP_GET_TYPE(b[new_x][new_y]) != NONE)
            {
                if (CP_GET_COLOR(b[new_x][new_y]) != CP_GET_COLOR(piece))
                {
                    /*  Add the valid hostile move */
                    return_moves[(*return_len)].v = (Vec2){new_x, new_y};
                    return_moves[(*return_len)++].take = 1;
                }
                break;
            }
        }
    }
}

char type_to_char(ChessPieceType t)
{
    switch (t)
    {
    case PAWN:
        return 'P';
        break;
    case BISHOP:
        return 'B';
        break;
    case KNIGHT:
        return 'N';
        break;
    case ROOK:
        return 'R';
        break;
    case QUEEN:
        return 'Q';
        break;
    case KING:
        return 'K';
        break;
    case NONE:
    default:
        return ' ';
    }
}

static char move_label_str[128];

const char *generate_move_label(ChessBoard board, LegalMove lm, int force_include_origin_rank, int force_include_origin_file)
{
    Vec2 start_loc = lm.origin_sqaure;
    Move move = lm.move;
    int len = 0;
    char p = type_to_char(CP_GET_TYPE(board[start_loc.x][start_loc.y]));
    if (p == 'P' && lm.next)
    {
        /* en passant */
        move = lm.next->move;
        start_loc = lm.next->origin_sqaure;
    }
    char take = move.take ? 'x' : 0;
    char file = start_loc.x + 'a';
    char rank = start_loc.y + '1';
    char file_dest = move.v.x + 'a';
    char rank_dest = move.v.y + '1';

    if (p != 'P')
    {
        if (p == 'K' && lm.next)
        {
            /* castling */
            move_label_str[len++] = 'O';
            move_label_str[len++] = '-';
            move_label_str[len++] = 'O';
            if (lm.next->origin_sqaure.x == 0)
            {
                /* queen side */
                move_label_str[len++] = '-';
                move_label_str[len++] = 'O';
            }
            move_label_str[len++] = 0;
            return move_label_str;
        }
        move_label_str[len++] = p;
    }
    else if (take)
    {
        /* pawn take move */
        move_label_str[len++] = file;
        if (force_include_origin_rank)
            move_label_str[len++] = rank;
        move_label_str[len++] = take;
        move_label_str[len++] = file_dest;
        move_label_str[len++] = rank_dest;
        move_label_str[len++] = 0;
        return move_label_str;
    }
    if (force_include_origin_file)
        move_label_str[len++] = file;
    if (force_include_origin_rank)
        move_label_str[len++] = rank;
    if (take)
    {
        /* take move */
        move_label_str[len++] = take;
    }
    move_label_str[len++] = file_dest;
    move_label_str[len++] = rank_dest;
    move_label_str[len++] = 0;
    return move_label_str;
}

#define MAX_MOVES 300

LegalMoveArray *generate_legal_moves(ChessGame *game, ChessColor turn_color)
{
    int x, y;
    LegalMoveArray *ret = (LegalMoveArray *)malloc(sizeof(LegalMoveArray));
    int cap = 10;
    ret->arr = (LegalMove *)malloc(sizeof(LegalMove) * cap);
    ret->len = 0;
    Move moves[MAX_MOVES];
    int moves_len = 0;
    for (x = 0; x < CHESS_BOARD_WIDTH; x++)
    {
        for (y = 0; y < CHESS_BOARD_HEIGHT; y++)
        {
            if (CP_GET_TYPE(game->board[x][y]) == NONE)
                continue;
            if (CP_GET_COLOR(game->board[x][y]) != turn_color)
                continue;
            moves_len = 0;
            generate_moves(game->board, x, y, moves, &moves_len);
            if (!moves_len)
                continue;
            int i;
            for (i = 0; i < moves_len; i++)
            {
                if (ret->len >= cap)
                {
                    cap *= 2;
                    LegalMove *tmp = (LegalMove *)realloc(ret->arr, sizeof(LegalMove) * cap);
                    if (!tmp)
                    {
                        perror("Could not allocate more memory in 'generate_legal_moves'\n");
                        exit(1);
                    }
                    ret->arr = tmp;
                }
                ret->arr[ret->len].move = moves[i];
                ret->arr[ret->len].next = NULL;
                ret->arr[ret->len++].origin_sqaure = (Vec2){x, y};
            }
            /* en passant */
            if (CP_GET_TYPE(game->board[x][y]) != PAWN)
                continue;
            if ((turn_color == WHITE && y == 4) || (turn_color == BLACK && y == 3))
            {
                int dir = (turn_color == BLACK ? -1 : 1);
                ChessSquare sq;
                /* check if there is a pawn to the left or right */
                if (x - 1 >= 0)
                { /* check the left */
                    sq = game->board[x - 1][y];
                    if (CP_GET_TYPE(sq) == PAWN && CP_GET_HAS_MOVED(sq) == 0 && CP_GET_COLOR(sq) != turn_color)
                    {
                        /* can en passant */
                        LegalMove move_enemy_back = {.move = {.take = 0, .v = {x - 1, y + dir}}, .origin_sqaure = {x - 1, y}};
                        LegalMove take_pawn = {.move = move_enemy_back.move, .origin_sqaure = {x, y}};
                        take_pawn.move.take = 1;
                        legal_move_add_next(&move_enemy_back, take_pawn);
                        if (ret->len >= cap)
                        {
                            cap *= 2;
                            LegalMove *tmp = (LegalMove *)realloc(ret->arr, sizeof(LegalMove) * cap);
                            if (!tmp)
                            {
                                perror("Could not allocate more memory in 'generate_legal_moves'\n");
                                exit(1);
                            }
                            ret->arr = tmp;
                        }
                        ret->arr[ret->len++] = move_enemy_back;
                    }
                }
                if (x + 1 < CHESS_BOARD_WIDTH)
                { /* check the right */
                    sq = game->board[x + 1][y];
                    if (CP_GET_TYPE(sq) == PAWN && CP_GET_HAS_MOVED(sq) == 0 && CP_GET_COLOR(sq) != turn_color)
                    {
                        /* can en passant */
                        LegalMove move_enemy_back = {.move = {.take = 0, .v = {x + 1, y + dir}}, .origin_sqaure = {x + 1, y}};
                        LegalMove take_pawn = {.move = move_enemy_back.move, .origin_sqaure = {x, y}};
                        take_pawn.move.take = 1;
                        legal_move_add_next(&move_enemy_back, take_pawn);
                        if (ret->len >= cap)
                        {
                            cap *= 2;
                            LegalMove *tmp = (LegalMove *)realloc(ret->arr, sizeof(LegalMove) * cap);
                            if (!tmp)
                            {
                                perror("Could not allocate more memory in 'generate_legal_moves'\n");
                                exit(1);
                            }
                            ret->arr = tmp;
                        }
                        ret->arr[ret->len++] = move_enemy_back;
                    }
                }
            }
        }
    }
    LegalMove *tmp = (LegalMove *)realloc(ret->arr, sizeof(LegalMove) * ret->len);
    if (!tmp)
    {
        perror("Could not clamp memory in 'generate_legal_moves'\n");
        exit(1);
    }
    ret->arr = tmp;
    return ret;
}

/* returns index in labels of input, returns -1 if can't find */
int chess_game_parse_input(char *_input, StrArray labels)
{
    if (!_input)
        return -1;
    size_t input_len = strlen(_input);
    if (!input_len)
        return -1;
    if (isdigit(_input[0]))
        return atoi(_input) - 1;
    int i;
    /* O(n^2) */
    for (i = 0; i < labels.len; i++)
    {
        if (strnicmp(_input, labels.arr[i], input_len) == 0)
            return i;
    }
    return -1;
}

void chess_game_print_turn_flair(ChessGame *game)
{
    printf("Turn %d, %s to move.\n", game->data.num_turns + 1, game->data.turn_color == WHITE ? "White" : "Black");
}

void chess_game_print_moves(StrArray *labels, int row_max)
{
    int i, tmp = 0;
    for (i = 0; i < labels->len; i++)
    {
        tmp = 0;
        printf("%3d. %-5s ", i + 1, labels->arr[i]);
        if (i && ((i + 1) % row_max) == 0)
        {
            printf("\n");
        }
        else
        {
            printf("| ");
            tmp = 1;
        }
    }
    if (tmp)
        printf("\n");
}

typedef struct
{
    int a, b;
} IntPair;

/* returns indexes of first value with duplicates */
IntPair *find_duplicates(StrArray *labels, int *ret_len)
{
    IntPair *ret = (IntPair *)malloc(sizeof(int) * labels->len);
    int i, j, len = 0;
    for (i = 0; i < labels->len; i++)
    {
        for (j = i + 1; j < labels->len; j++)
        {
            if (i == j)
                continue;
            if (strcmp(labels->arr[i], labels->arr[j]) == 0)
            {
                ret[len++] = (IntPair){i, j};
            }
        }
    }
    *ret_len = len;
    if (!len)
    {
        free(ret);
        return NULL;
    }
    return ret;
}

/* expects labels array to have the capacity to hold lma and that the len is equal
        This function can undoubtably be optimized
*/
void populate_labels(StrArray *labels, ChessGame *game, LegalMoveArray *lma)
{
    int i;
    for (i = 0; i < lma->len; i++)
    {
        labels->arr[i] = strdup(generate_move_label(game->board, lma->arr[i], 0, 0));
    }
    int len = -1;
    IntPair *dups = find_duplicates(labels, &len);
    while (dups != NULL)
    { /* check if the pieces are in the same file,
        if they are not then just add the files,
        if they are then add the rank also */
        for (i = 0; i < len; i++)
        {
            IntPair pair = dups[i];
            if (strcmp(labels->arr[pair.a], labels->arr[pair.b]) != 0)
                continue;
            Vec2 v0 = lma->arr[pair.a].origin_sqaure;

            Vec2 v1 = lma->arr[pair.b].origin_sqaure;
            free(labels->arr[pair.a]);
            free(labels->arr[pair.b]);
            if (v0.x == v1.x) /* compare files */
            {
                labels->arr[pair.a] = strdup(generate_move_label(game->board, lma->arr[pair.a], 1, 1));
                labels->arr[pair.b] = strdup(generate_move_label(game->board, lma->arr[pair.b], 1, 1));
            }
            else
            {
                labels->arr[pair.a] = strdup(generate_move_label(game->board, lma->arr[pair.a], 0, 1));
                labels->arr[pair.b] = strdup(generate_move_label(game->board, lma->arr[pair.b], 0, 1));
            }
        }
        free(dups);
        dups = find_duplicates(labels, &len);
    }
}

/* returns the index of the move, which is the same for lma and labels.
    Function also populates the labels array.
*/
int chess_game_get_move_idx_from_user(ChessGame *game, LegalMoveArray **lma, StrArray *labels)
{
    const int row_max = 5;
load_game:
    populate_labels(labels, game, *lma);
select_move:
    chess_game_print_moves(labels, row_max);
    chess_board_print(game->board);
    printf("Choose a move: ");
    size_t len;
    char *inp = input('\n', &len);
    if (inp == strstr(inp, "quit"))
    {
        free_legal_move_array(*lma);
        free_str_array(*labels);
        return -1;
    }
    if (inp == strstr(inp, "rand"))
    {
        return rand() % (*lma)->len;
    }
    if (inp == strstr(inp, "save "))
    {
        chess_game_serialize(game, inp + 5);
        goto select_move;
    }
    if (inp == strstr(inp, "load "))
    {
        chess_game_deserialize(game, inp + 5);
        printf("Turn %d, %s to move.\n", game->data.num_turns, game->data.turn_color == WHITE ? "White" : "Black");
        chess_board_print(game->board);
        free_legal_move_array(*lma);
        *lma = generate_legal_moves(game, game->data.turn_color);
        int check = chess_game_is_king_in_check(game, game->data.turn_color);
        if (check)
        {
            remove_illegal_moves_while_in_check(game, *lma);
        }
        else
        {
            add_castle_move(game, *lma);
        }
        free_str_array(*labels);
        labels->len = (*lma)->len;
        labels->arr = (char **)malloc(sizeof(char *) * (*lma)->len);
        goto load_game;
    }
    int x = chess_game_parse_input(inp, *labels);
    if (x == -1)
    {
        printf("Please choose a move from the list.\n");
        goto select_move;
    }
    return x;
}

void chess_game_update(ChessGame *game, StrArray labels, int x, FILE *pgn_file)
{
    static int i = 0;
    if (game->data.turn_color == WHITE)
        fprintf(pgn_file, "%d. %s ", game->data.num_turns + 1, labels.arr[x]);
    else
        fprintf(pgn_file, "%s\n", labels.arr[x]);
    fflush(pgn_file);
    if (++i % 2 == 0)
    {
        game->data.num_turns++;
        i = 0;
    }
    game->data.turn_color = !game->data.turn_color;
    Vec2 white_king_loc = chess_board_find_king(game->board, WHITE);
    ChessPiece white_king = game->board[white_king_loc.x][white_king_loc.y];
    if (CP_GET_IS_IN_THREAT(white_king))
    {
        game->data.king_in_check.white++;
        game->data.king_was_checked.white = 1;
    }
    else
    {
        game->data.king_in_check.white = 0;
    }
    Vec2 black_king_loc = chess_board_find_king(game->board, BLACK);
    ChessPiece black_king = game->board[black_king_loc.x][black_king_loc.y];
    if (CP_GET_IS_IN_THREAT(black_king))
    {
        game->data.king_in_check.black++;
        game->data.king_was_checked.black = 1;
    }
    else
    {
        game->data.king_in_check.black = 0;
    }
}

int chess_game_is_king_in_check(ChessGame *game, ChessColor c)
{
    Vec2 loc = chess_board_find_king(game->board, c);
    return CP_GET_IS_IN_THREAT(game->board[loc.x][loc.y]);
}

/* copies game to ret_saved_game*/
void chess_game_save_state(ChessGame *game, ChessGame *ret_saved_game)
{
    ret_saved_game->data = game->data;
    int i, j;
    for (i = 0; i < CHESS_BOARD_WIDTH; i++)
    {
        for (j = 0; j < CHESS_BOARD_HEIGHT; j++)
        {
            ret_saved_game->board[i][j] = game->board[i][j];
        }
    }
}

/* copies saved game to game. */
void chess_game_load_state(ChessGame *game, ChessGame *saved_game)
{
    memcpy(game, saved_game, sizeof(ChessGame));
}

/* doesn't clear threats */
void chess_game_update_threats(ChessGame *game, ChessColor color)
{
    /* update the threats to see if we are in threat now */
    LegalMoveArray *lma_tmp = generate_legal_moves(game, color);
    chess_board_set_threats(game->board, lma_tmp);
    free_legal_move_array(lma_tmp);
}

void chess_game_handle_pawn_promotion(ChessGame *game)
{
    int i, row = (game->data.turn_color == BLACK) ? 0 : (CHESS_BOARD_HEIGHT - 1);
    for (i = 0; i < CHESS_BOARD_WIDTH; i++)
    {
        if (CP_GET_COLOR(game->board[i][row]) != game->data.turn_color)
            continue;
        if (CP_GET_TYPE(game->board[i][row]) == PAWN)
            CP_SET_TYPE(game->board[i][row], QUEEN);
    }
}

void remove_illegal_moves_while_in_check(ChessGame *game, LegalMoveArray *lma)
{
    LegalMoveArray new_lma = {(LegalMove *)malloc(sizeof(LegalMove) * lma->len), 0};
    int i;
    for (i = 0; i < lma->len; i++)
    {
        ChessGame tmp_game;
        chess_game_load_state(&tmp_game, game);
        chess_board_move_piece(tmp_game.board, lma->arr[i]);
        chess_board_clear_threats(tmp_game.board);
        chess_game_update_threats(&tmp_game, !tmp_game.data.turn_color);
        chess_game_update_threats(&tmp_game, tmp_game.data.turn_color);
        /* printf("Move %d\n",i);
         chess_board_visualize(tmp_game.board);
        chess_board_print(tmp_game.board); */
        int check = chess_game_is_king_in_check(&tmp_game, tmp_game.data.turn_color);
        if (!check)
            new_lma.arr[new_lma.len++] = lma->arr[i];
    }
    free_legal_move_array(lma);
    if (!new_lma.len) 
    {
        *lma = new_lma;
        return;
    }
    LegalMove *tmp = (LegalMove *)realloc(new_lma.arr, new_lma.len * sizeof(LegalMove));
    if (!tmp)
    {
        perror("Could not clamp lma in 'remove_illegal_moves_while_in_check'\n");
        exit(1);
    }
    new_lma.arr = tmp;
    *lma = new_lma;
}

typedef struct
{
    int found;
    Vec2 v;
} OptionalVec2;

/* returns the first instance of a piece with matching data */
OptionalVec2 chess_board_find_piece(ChessBoard cb, ChessPieceType t, ChessColor c)
{
    OptionalVec2 ret = {0};
    int i, j;
    for (i = 0; i < CHESS_BOARD_WIDTH; i++)
    {
        for (j = 0; j < CHESS_BOARD_HEIGHT; j++)
        {
            ChessSquare sq = cb[i][j];
            if (CP_GET_TYPE(sq) == t && CP_GET_COLOR(sq) == c)
            {
                ret.v.x = i;
                ret.v.y = j;
                ret.found = 1;
                return ret;
            }
        }
    }
    return ret;
}

void add_castle_move(ChessGame *game, LegalMoveArray *lma)
{
    Vec2 king_loc = chess_board_find_king(game->board, game->data.turn_color);
    ChessSquare king_sq = game->board[king_loc.x][king_loc.y];
    if (CP_GET_HAS_MOVED(king_sq))
        return;
    if (CP_GET_WAS_IN_THREAT(king_sq))
        return;
    int y = 0;
    if (game->data.turn_color == BLACK)
        y = CHESS_BOARD_HEIGHT - 1;
    int i;
    /* queen side */
    ChessSquare rook_sq = game->board[0][y];
    if (CP_GET_TYPE(rook_sq) == ROOK && CP_GET_HAS_MOVED(rook_sq) == 0)
    {
        /* check if there are pieces between us and the king */
        for (i = 1; i < king_loc.x; i++)
        {
            if (CP_GET_TYPE(game->board[i][y]) != NONE)
                goto skip_queen_side;
        }
        LegalMove king_move = {.move = {.take = 0, .v = {2, y}}, .origin_sqaure = king_loc},
                  rook_move = {.move = {.take = 0, .v = {3, y}}, .origin_sqaure = {0, y}};
        legal_move_add_next(&king_move, rook_move);
        LegalMove *tmp = (LegalMove *)realloc(lma->arr, (lma->len + 1) * sizeof(LegalMove));
        if (!tmp)
        {
            perror("Could not realloc lma in 'add_castle_move'\n");
            exit(1);
        }
        lma->arr = tmp;
        lma->arr[lma->len++] = king_move;
        /*
                TODO:
                        add castle queen side to lma
        */
    }
skip_queen_side:
    /* king side */
    rook_sq = game->board[CHESS_BOARD_WIDTH - 1][y];
    if (CP_GET_TYPE(rook_sq) == ROOK && CP_GET_HAS_MOVED(rook_sq) == 0)
    {
        /* check if there are pieces between us and the king */
        for (i = king_loc.x + 1; i < CHESS_BOARD_WIDTH - 1; i++)
        {
            if (CP_GET_TYPE(game->board[i][y]) != NONE)
                return;
        }
        LegalMove king_move = {.move = {.take = 0, .v = {6, y}}, .origin_sqaure = king_loc},
                  rook_move = {.move = {.take = 0, .v = {5, y}}, .origin_sqaure = {CHESS_BOARD_WIDTH - 1, y}};
        legal_move_add_next(&king_move, rook_move);
        LegalMove *tmp = (LegalMove *)realloc(lma->arr, (lma->len + 1) * sizeof(LegalMove));
        if (!tmp)
        {
            perror("Could not realloc lma in 'add_castle_move'\n");
            exit(1);
        }
        lma->arr = tmp;
        lma->arr[lma->len++] = king_move;
        /*
                TODO:
                        add castle king side to lma
        */
    }
}

void chess_game_set_moved(ChessGame *game, LegalMove lm)
{
    Vec2 v = lm.move.v;
    CP_SET_HAS_MOVED(game->board[v.x][v.y], 1);
    if (lm.next)
    {
        LegalMove *node = lm.next;
        while (node != NULL)
        {
            v = node->move.v;
            CP_SET_HAS_MOVED(game->board[v.x][v.y], 1);
            node = node->next;
        }
    }
}

void chess_game_serialize(ChessGame *game, char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (!f)
    {
        fprintf(stderr, "Failed to open file '%s'\n", filename);
        return;
    }
    fwrite(&game->data, sizeof(game->data), 1, f);
    fwrite(game->board, sizeof(ChessSquare), CHESS_BOARD_LEN, f);
    fclose(f);
}

void chess_game_deserialize(ChessGame *ret_game, char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        fprintf(stderr, "Failed to open file '%s'\n", filename);
        return;
    }
    size_t len = fread(&(ret_game->data), sizeof(ChessBoardData), 1, f);
    if (len != 1)
    {
        perror("Failed to read ChessBoardData from file.\n");
        goto file_clean_up;
    }
    len = fread((ret_game->board), sizeof(ChessSquare), CHESS_BOARD_LEN, f);
    if (len != CHESS_BOARD_LEN)
    {
        perror("Failed to read ChessBoard from from.\n");
        goto file_clean_up;
    }
file_clean_up:
    fclose(f);
}

void update_enemy_pawn_has_moved(ChessGame *game)
{
    int rank = (!game->data.turn_color) == WHITE ? 1 : 6;
    int i, j;
    for (i = 0; i < CHESS_BOARD_WIDTH; i++)
    {
        for (j = 0; j < CHESS_BOARD_HEIGHT; j++)
        {
            if (j == rank)
                continue;
            if (CP_GET_TYPE(game->board[i][j]) == PAWN && CP_GET_COLOR(game->board[i][j]) != game->data.turn_color)
            {
                CP_SET_HAS_MOVED(game->board[i][j], 1);
            }
        }
    }
}

void chess_game_start(ChessGame *start_data, int enable_ai, ChessColor ai_color)
{
    ChessGame game = start_data ? *start_data : (ChessGame){0}, tmp_game = {0};
    FILE *pgn_file = fopen("move_history.pgn", "w");
    chess_board_init(game.board);
    game.data.turn_color = WHITE;
    printf("Input 'quit' to close.\n");
    while (1)
    {
        /* chess_board_print(game.board); */
        int in_check_before_move = chess_game_is_king_in_check(&game, game.data.turn_color);
        LegalMoveArray *lma = generate_legal_moves(&game, game.data.turn_color);
        if (lma->len == 0)
        {
            if (in_check_before_move)
            {
                printf("Checkmate, %s wins.\n", game.data.turn_color != WHITE ? "White" : "Black");
            }
            printf("Stalemate; No moves left for %s.\n", game.data.turn_color == WHITE ? "White" : "Black");
            break;
        }
        if (in_check_before_move)
        {
            remove_illegal_moves_while_in_check(&game, lma);
        }
        else
        {
            add_castle_move(&game, lma);
        }
        if (lma->len == 0)
        {
            printf("Checkmate, %s wins.\n",game.data.turn_color != WHITE ? "White" : "Black");
            break;
        }

        StrArray labels = {(char **)malloc(sizeof(char *) * lma->len), lma->len};
        chess_game_save_state(&game, &tmp_game);
        /* update late to enable en passant */
        update_enemy_pawn_has_moved(&game);
    get_move:
        chess_game_print_turn_flair(&game);
        if (in_check_before_move)
            printf("Your King is in check.\n");
        int x;
        if (ai_color == game.data.turn_color && enable_ai)
        {
            x = rand() % lma->len;
            populate_labels(&labels, &game, lma);
        }
        else
        {
            x = chess_game_get_move_idx_from_user(&game, &lma, &labels);
            if (x == -1)
                break;
        }
        printf("Chose %d.\n", x + 1);
        chess_board_move_piece(game.board, lma->arr[x]);
        chess_board_print(game.board);
        /* update the threats to see if we are in threat now */
        chess_board_clear_threats(game.board);
        chess_game_update_threats(&game, !game.data.turn_color);
        int in_check_after_move = chess_game_is_king_in_check(&game, game.data.turn_color);
        if (in_check_after_move)
        {
            chess_game_load_state(&game, &tmp_game);
            if (in_check_before_move)
            {
                printf("You must move out of check or block it before you can move other pieces.\n");
                goto get_move;
            }
            printf("You cannot move your King into check.\n");
            goto get_move;
        }
        /* set has moved */
        Vec2 loc = lma->arr[x].move.v;
        if (!(CP_GET_TYPE(game.board[loc.x][loc.y]) == PAWN && CP_GET_HAS_MOVED(game.board[loc.x][loc.y]) == 0))
        {
            chess_game_set_moved(&game, lma->arr[x]);
        }

        /* promote pawns to queens */
        chess_game_handle_pawn_promotion(&game);
        /* update how the current players pieces threaten the others on the board */
        chess_game_update_threats(&game, game.data.turn_color);
        /* turn color changes */
        chess_game_update(&game, labels, x, pgn_file);

        printf("\t%s\n", labels.arr[x]);

        free_legal_move_array(lma);
        free_str_array(labels);
    }
    fclose(pgn_file);
}

/*
    TODO:
        1. Pawn Promotion (Done? Queen only.)
        2. Castling (Done?)
        3. Check (Done?)
        *4. Point counting (Piece values)
        *5. Move Timer
        6. Checkmate (Done?)
        7. Edge cases with pgn notation (Done?)
                i.e. Two bishops on the same diagonal
                that can move to the same location. (Bfe2)
        8. En Passant (Done?)
                Change the way we decide when pawns have moved.
                Pawns have only moved if they are not on their home row.
                Update the data of whether pawns have moved after we check for en passant.
                (This way it will move, then the next player will check if they can en passant that pawn)
                Checking for en passant:
                        If a black pawn is in the fourth rank
                        and there is an enemy pawn next to it that has not moved
                        then we can en passant

                The move should be enemy pawn move back one square, then friendly pawn move to where the enemy pawn is.

*/