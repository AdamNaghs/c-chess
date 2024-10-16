#ifndef _CHESS_H
#define _CHESS_H
#include <stdlib.h>
#include "common.h"
#include "move.h"

/* #define CHESS_DISABLE_COLOR_TEXT */

#define CHESS_COLOR_BIT_LEN 1
/*
    Total Bits: 1
*/
typedef enum
{
        BLACK,
        WHITE,
} ChessColor;

#define CHESS_PIECE_TYPE_BIT_LEN 3
/* only requires 3 bits of memory (0-7) and can fit one more value */
typedef enum
{
        NONE,
        PAWN,
        BISHOP,
        KNIGHT,
        ROOK,
        QUEEN,
        KING,
} ChessPieceType;

#define CHESS_PIECE_BIT_LEN 7
/*
    Total Bits: 7
    0-2 - ChessPieceType
    3 - ChessColor
    4 - 0 = hasn't moved, 1 = has moved,
    5 - 0 = not threatened, 1 = is threatened
    6 - 0 = has never been threatened, 1 = has been threatened
*/
typedef unsigned char ChessPiece;

#define CHESS_SQUARE_BIT_LEN 8
/*
    Total Bits: 8
    0-6 ChessPiece
    7 - ChessColor
*/
typedef unsigned char ChessSquare;

#define CHESS_BOARD_WIDTH 8
#define CHESS_BOARD_HEIGHT 8
#define CHESS_BOARD_LEN (CHESS_BOARD_WIDTH * CHESS_BOARD_HEIGHT)
typedef ChessSquare ChessBoard[CHESS_BOARD_WIDTH][CHESS_BOARD_HEIGHT];

typedef struct
{
        ChessColor turn_color;
        int num_turns;
        int fifty_move_rule_turn_count;
        /* contains moves, not turns. Moves are one move from either player */
        struct
        {
                /* number of moves the king has been in check */
                int black, white;
        } king_in_check;
        struct 
        {
                int black, white;
        } king_was_checked;
        /* file of the last pawn to move 2 squares*/
} ChessBoardData;

typedef struct
{
        ChessBoard board;
        ChessBoardData data;
} ChessGame;

/* you can pass NULL if you dont have any data */
void chess_game_start(ChessGame *start_data, int enable_ai, ChessColor ai_color);

void chess_game_serialize(ChessGame* game, char* filename);

void chess_game_deserialize(ChessGame* ret_game, char* filename);

void chess_board_init(ChessBoard b);

void chess_board_visualize(ChessBoard b);

void chess_board_print(ChessBoard b);

void generate_moves(ChessBoard b, int x, int y, Move *return_moves, int *return_len);

const char *generate_move_label(ChessBoard board, LegalMove lm, int force_include_origin_rank, int force_include_origin_file);

int char_to_file(char c);

char type_to_char(ChessPieceType t);

void chess_board_populate(ChessBoard b, unsigned int i, unsigned int j);

/* Generate all legal moves for this turn */
LegalMoveArray *generate_legal_moves(ChessGame* game, ChessColor turn_color);

void chess_board_move_piece(ChessBoard board, LegalMove move);

/*  Getter and Setter for ChessPiece Type */
#define CP_GET_TYPE(piece) ((piece) & ((1 << CHESS_PIECE_TYPE_BIT_LEN) - 1))

#define CP_SET_TYPE(piece, type)                                           \
        do                                                                 \
        {                                                                  \
                piece = (piece & ~((1 << CHESS_PIECE_TYPE_BIT_LEN) - 1)) | \
                        ((type) & ((1 << CHESS_PIECE_TYPE_BIT_LEN) - 1));  \
        } while (0)

/*  Getter and Setter for ChessPiece Color */
#define CP_GET_COLOR(piece) (((piece) >> CHESS_PIECE_TYPE_BIT_LEN) & ((1 << CHESS_COLOR_BIT_LEN) - 1))

#define CP_SET_COLOR(piece, color)                                                                  \
        do                                                                                          \
        {                                                                                           \
                piece = (piece & ~(((1 << CHESS_COLOR_BIT_LEN) - 1) << CHESS_PIECE_TYPE_BIT_LEN)) | \
                        (((color) & ((1 << CHESS_COLOR_BIT_LEN) - 1)) << CHESS_PIECE_TYPE_BIT_LEN); \
        } while (0)

/*  Getter for Has Moved */
#define CP_GET_HAS_MOVED(piece) (((piece) >> (CHESS_PIECE_TYPE_BIT_LEN + CHESS_COLOR_BIT_LEN)) & 0x01)

/*  Setter for Has Moved */
#define CP_SET_HAS_MOVED(piece, hasMoved)                                                        \
        do                                                                                       \
        {                                                                                        \
                piece = (piece & ~(1 << (CHESS_PIECE_TYPE_BIT_LEN + CHESS_COLOR_BIT_LEN))) |     \
                        ((hasMoved & 0x01) << (CHESS_PIECE_TYPE_BIT_LEN + CHESS_COLOR_BIT_LEN)); \
        } while (0)

#define CP_GET_IS_IN_THREAT(piece) (((piece) >> 5) & 0x01)
#define CP_GET_WAS_IN_THREAT(piece) (((piece) >> 6) & 0x01)

#define CP_SET_THREAT(piece, threat)                                     \
        do                                                               \
        {                                                                \
                if (threat)                                              \
                {                                                        \
                        piece |= (1 << 5); /* Set is_in_threat bit */    \
                        piece |= (1 << 6); /* Set was_in_threat bit */   \
                }                                                        \
                else                                                     \
                {                                                        \
                        piece &= ~(1 << 5); /* Clear is_in_threat bit */ \
                }                                                        \
        } while (0)

/*  Getter and Setter for ChessSquare Piece */
#define CS_GET_PIECE(square) ((square) & ((1 << CHESS_PIECE_BIT_LEN) - 1))

#define CS_SET_PIECE(square, piece)                                         \
        do                                                                  \
        {                                                                   \
                (square) = ((square) & ~((1 << CHESS_PIECE_BIT_LEN) - 1)) | \
                           ((piece) & ((1 << CHESS_PIECE_BIT_LEN) - 1));    \
        } while (0)

/*  Getter and Setter for ChessSquare Color */
#define CS_GET_COLOR(square) (((square) >> CHESS_PIECE_BIT_LEN) & ((1 << CHESS_COLOR_BIT_LEN) - 1))

#define CS_SET_COLOR(square, color)                                                              \
        do                                                                                       \
        {                                                                                        \
                square = (square & ~(((1 << CHESS_COLOR_BIT_LEN) - 1) << CHESS_PIECE_BIT_LEN)) | \
                         (((color) & ((1 << CHESS_COLOR_BIT_LEN) - 1)) << CHESS_PIECE_BIT_LEN);  \
        } while (0)


void add_castle_move(ChessGame *game, LegalMoveArray *lma);

void remove_illegal_moves_while_in_check(ChessGame *game, LegalMoveArray *lma);

int chess_game_is_king_in_check(ChessGame *game, ChessColor c);

#endif