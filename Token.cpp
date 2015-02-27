
struct Token_t : NZA_t
{
    uint32_t type;
    uint32_t str;

    template<typename T1, typename T2>
    Token_t (T1 type_, T2 str_) :
        type (static_cast<uint32_t> (type_)),
        str  (static_cast<uint32_t> (str_))
    {}

    template<typename T>
    Token_t (T str_) :
        type (TOKEN_UNTYPED),
        str  (static_cast<uint32_t> (str_))
    {}
};

struct StringTable_t : NZA_t
{
    std::vector<std::string> data;

    StringTable_t () :
        data (),
        size ()
    {}

    void ok ()
    {
        DEFAULT_OK_BLOCK
    }

    uint32_t Register (std::string str)
    {
        BEGIN
        auto found = std::find (data.begin (), data.end (), str);
        if (found == data.end ())
        {
            for (uint32_t i = 0; i < data.size (); i++)
            {
                if (data[i].empty ())
                {
                    data[i] = str;
                    return i;
                }
            }
            data.push_back (str);
            return data.size () - 1;
        }
        else
            return found - data.begin ();
        END (REGISTER_STRING, "Failed to register string")
    }

    void Delete (uint32_t key)
    {
        BEGIN
        if (key < data.size ())
        {
            data[key].clear ();
            size--;
        }
        END (DELETE_STRING, "Failed to delete string")
    }
};

struct VirtualCodeRepresentation_t : NZA_t
{
    std::vector<Token_t> tokens_;
    StringTable_t strings_;

    void ok()
    {
        DEFAULT_OK_BLOCK
    }

    VirtualCodeRepresentation_t ()
    try :
        tokens_ (),
        strings_ ()
    {}
    END (CTOR)

    void AddPrimaryToken (std::string token)
    {
        tokens_.push_back (strings_.Register (token));
    }
};
