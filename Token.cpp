
struct Token_t : NZA_t
{
    uint32_t type;
    int64_t data;
    uint32_t line;
    uint32_t shift;

    Token_t () :
        type  (),
        data   (),
        line  (),
        shift ()
    {}

    Token_t (int64_t data_, uint32_t line_, uint32_t shift_) :
        type  (TOKEN_UNTYPED),
        data  (data_),
        line  (line_),
        shift (shift_)
    {}
};

struct StringTable_t : NZA_t
{
    std::vector<std::string> data;

    StringTable_t ()
    try :
        data ()
    {
//  }
    END (CTOR)

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
        END (REGISTER_STRING)
    }

    void Delete (uint32_t key)
    {
        BEGIN
        if (key < data.size ())
        {
            data[key].clear ();
        }
        END (DELETE_STRING)
    }

    std::string GetString (uint32_t key)
    {
        BEGIN
        if (key < data.size ())
        {
            return data[key];
        }
        else
            return ""s;
        END (GET_STRING)
    }

    StringTable_t& operator = (const StringTable_t& that)
    {
        data = that.data;
        return *this;
    }

    int32_t Registered (std::string str)
    {
        auto found = std::find (data.begin (), data.end (), str);
        if (found == data.end ()) return -1;
        return found - data.begin ();
    }

    uint32_t size ()
    {
        return data.size ();
    }
};

struct VirtualCodeRepresentation_t : NZA_t
{
    std::vector<Token_t> tokens_;
    StringTable_t strings_;
    std::vector<std::string> program_;

    void ok()
    {
        DEFAULT_OK_BLOCK
    }

    VirtualCodeRepresentation_t ()
    try :
        tokens_  (),
        strings_ (),
        program_ ()
    {
//  }
    END (CTOR)

    ~VirtualCodeRepresentation_t () {}

    void AddPrimaryToken (std::string token, uint32_t line, uint32_t shift)
    {
        tokens_.push_back (Token_t (strings_.Register (token), line, shift));
    }

    void AddLine (std::string line)
    {
        program_.push_back (line);
    }

    std::string GetString (uint32_t key)
    {
        return strings_.GetString (key);
    }

};
