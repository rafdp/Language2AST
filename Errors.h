#ifndef ERRORS_H_INCLUDED
#define ERRORS_H_INCLUDED

#define E(num, name) ERROR_##name = num

enum ERRORS
{
    E (357, BAD_ALLOC),
    E (358, UNKNOWN),
    E (359, NULL_THIS),
    E (360, NULL_PTR),
    E (361, OPEN_FILE),
    E (362, DESTROYED),
    E (363, REGISTER_STRING),
    E (364, DELETE_STRING),
    E (365, FILE_MODE),
    E (366, READ_LINE),
    E (367, CTOR),
    E (368, DTOR),
    E (369, NULL_CODE_PTR),
    E (370, LOAD_FILE),
    E (371, INVALID_ERROR_MODE),
    E (372, ADD_ERROR),
    E (373, EMPTY_STL),
    E (374, FILE_END),
    E (375, WRITE_ERROR),
    E (376, DUMP_ERRORS),
    E (377, COMPILE),
    E (378, NULL_CUMLATIVE_ERRORS_PTR),
    E (379, CLEAR),
    E (380, PROCESS),
    E (381, PARSE_LINE),
    E (382, GET_STRING)
};

#undef E

#define F(num, name) FLAG_##name = num

enum NODE_CONTENT : uint8_t
{
    F (1, NUMERIC),
    F (2, OPERATOR),
    F (3, FUNCTION),
    F (4, ARGUMENT),
    F (5, NUMERICD)
};

#undef F

enum NODE_DATA : uint64_t
{
     OP_PLUS   = '+',
     OP_MINUS  = '-',
     OP_MUL    = '*',
     OP_DIV    = '/',
     OP_POW    = '^',
     CMD_SIN   = 256,
     CMD_COS   = 257,
     CMD_TAN   = 258,
     CMD_CTG   = 259,
     CMD_EXP   = 260,
     CMD_LN    = 261,
     CMD_ROOT  = 262,
     CMD_UNMIN = 263
};

#define TOKEN_BLOCK \
    T ( 1, UNTYPED), \
    T ( 2, HAI), \
    T ( 3, KTHXBYE), \
    T ( 4, BTW), \
    T ( 5, OBTW), \
    T ( 6, SO), \
    T ( 7, TLDR), \
    T ( 8, SUM), \
    T ( 9, OF), \
    T (10, AN), \
    T (11, DIFF), \
    T (12, PRODUKT), \
    T (13, QUOSHUNT), \
    T (14, BIGGR), \
    T (15, SMALLR), \
    T (16, BOTH), \
    T (17, EITHER), \
    T (18, WON), \
    T (19, NOT), \
    T (20, ALL), \
    T (21, MKAY), \
    T (22, ANY), \
    T (23, SAEM), \
    T (24, DIFFRINT), \
    T (25, VISIBLE), \
    T (26, GIMMEH), \
    T (27, O), \
    T (28, RLY_QM), \
    T (29, YA), \
    T (30, RLY), \
    T (31, NO), \
    T (32, WAI), \
    T (33, OIC), \
    T (34, IM), \
    T (35, IN), \
    T (36, YR), \
    T (37, UPPIN), \
    T (38, DERFIN), \
    T (39, TIL), \
    T (40, OUTTA), \
    T (41, HOW), \
    T (42, IZ), \
    T (43, I), \
    T (44, IF), \
    T (45, YOU), \
    T (46, SAY), \
    T (47, HAS), \
    T (48, A), \
    T (49, R),\
    T (50, WILE), \
    T (51, THAN)


#define T(num, name) TOKEN_##name = num
enum TOKEN_TYPES : uint32_t
{
    T (100, NUM),
    TOKEN_BLOCK
};

#undef T

#define E(num, name) EM_##name = num
enum ERROR_MODES : uint32_t
{
    E (1, WARNING),
    E (2, ERROR)
};

#undef E

const uint32_t DEFAULT_STR_BUFFER_SIZE = 512;


#endif // ERRORS_H_INCLUDED
