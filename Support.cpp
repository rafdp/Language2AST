
std::string* CreateStringOnFail (const char* text, ...)
{
    std::string* strPtr = new std::string;
    if ( !text )
        return strPtr;

    char str [512]  = "";

    va_list arguments;
    va_start (arguments, text);

    vsnprintf (str, 510, text, arguments);

    va_end (arguments);

    *strPtr += str;

    return strPtr;
}

#define _EXC_N(code, msg) { NAT_EXCEPTION (EXPN, CreateStringOnFail (msg), ERROR_code) }
#define _ ,

#define BEGIN \
try { ok ();

#define END(code)\
ok (); } \
DETAILED_CONSECUTIVE_CATCH ("Error occurred", ERROR_code, expn_) \
DETAILED_CATCH             ("Error occurred", ERROR_BAD_ALLOC, std::bad_alloc, expn_) \
DETAILED_UNKNOWN_CATCH     ("Error occurred", ERROR_UNKNOWN, expn_)

#define DEFAULT_OK_BLOCK \
NZA_t::ok (); \
if (this == nullptr) \
    _EXC_N (NULL_THIS, "Null this")\

class NZA_t // no zombies allowed
{
protected:
    int not_yet_destroyed;

public:
    NZA_t () :
        not_yet_destroyed (1)
    {}

    virtual ~NZA_t ()
    {
        not_yet_destroyed = 0;
    }

    virtual void ok ()
    {
        if (this == nullptr)
            _EXC_N (NULL_THIS, "Null this")
        if (not_yet_destroyed == 0)
            _EXC_N (DESTROYED, "Trying to access destroyed object")
    }

};


class File_t : NZA_t
{
    std::string filename_;
    FILE* file_;
    std::string mode_;

    void ok ()
    {
        DEFAULT_OK_BLOCK
    }

public:
    File_t (std::string filename, const char* mode)
    try :
        filename_ (filename),
        file_     (fopen (filename_, mode)),
        mode_     (mode)
    {
        if (!file_)
            _EXC_N (OPEN_FILE, "Failed to open file \"%s\"" _ filename_.c_str())

        if (CheckMode ())
            _EXC_N (FILE_MODE, "Invalid file open mode \"%s\"" _ mode_.c_str())
    }
    END (CTOR)

    bool CheckMode ()
    {
        if (mode_.size() > 2 || mode_.size() < 1)
            return false;
        if (mode_[0] != 'a' && mode_[0] != 'w' && mode_[0] != 'r')
            return false;
        if (mode_.size() == 2 && mode_[1] != '+' && mode_[1] != 'b')
            return false;
        return true;
    }

    std::string ReadLine ()
    {
        BEGIN
        char str [DEFAULT_STR_BUFFER_SIZE] = "";
        fgets (str, DEFAULT_STR_BUFFER_SIZE - 1, file_);
        return str;
        END (READ_LINE)
    }

    bool End ()
    {
        return feof (file_);
    }
};

struct CumulativeErrors_t
{

};

