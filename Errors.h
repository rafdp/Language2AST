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
    E (380, PROCESS)
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

#define T(num, name) TOKEN_##name = num
enum TOKEN_TYPES : uint32_t
{
    T (1, UNTYPED)
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
