
class LolcodeParser_t : NZA_t
{
    DISABLE_CLASS_COPY (LolcodeParser_t)

    VirtualCodeRepresentation_t* code_;
    CumulativeErrors_t*          cerrs_;
    void ok ();
    void ParseLine (std::string token, uint32_t lineN);
    void CheckIterStr (std::string::iterator& i, std::string& line, int lineN);
    bool CheckNum (std::string s, int64_t* num);
public:
    LolcodeParser_t (VirtualCodeRepresentation_t* code,
                     CumulativeErrors_t* cerrs);

    void LoadFile (std::string filename);
    void ParseTokens ();
    void ParseConstructs ();
};

void LolcodeParser_t::ok ()
{
    DEFAULT_OK_BLOCK
    if (code_ == nullptr)
        _EXC_N (NULL_CODE_PTR, "Null code pointer")
    if (cerrs_ == nullptr)
        _EXC_N (NULL_CUMLATIVE_ERRORS_PTR, "Null cumulative errors pointer")
}

LolcodeParser_t::LolcodeParser_t (VirtualCodeRepresentation_t* code,
                                  CumulativeErrors_t* cerrs)
try :
    code_  (code),
    cerrs_ (cerrs)
{

/*
}
*/

END (CTOR)


void LolcodeParser_t::LoadFile (std::string filename)
{
    BEGIN
    File_t file (filename, "r");
    std::string currentLine;
    uint32_t lineN = 1;
    while (!file.End ())
    {
        ParseLine (file.ReadLine (), lineN);
        lineN++;
    }

    END (LOAD_FILE)
}

bool LolcodeParser_t::CheckNum (std::string str, int64_t* num)
{
    int64_t numLocal = 0;
    bool negative = (str[0] == '-');
    for (auto i = str.begin(); i < str.end(); i++)
    {
        if (negative && i == str.begin()) continue;
        if (*i > '9' || *i < '0') return false;
        numLocal *= 10;
        numLocal += *i - '0';
    }
    if (negative) numLocal *= -1;
    *num = numLocal;
    return true;
}


void LolcodeParser_t::CheckIterStr (std::string::iterator& i, std::string& line, int lineN)
{
    if (!(*i >= 'a' && *i <= 'z') &&
        !(*i >= 'A' && *i <= 'Z') &&
        !(*i >= '0' && *i <= '9') &&
        *i != '_' &&
        *i != ' ' &&
        *i != '.' &&
        *i != ',' &&
        *i != '?')
        cerrs_->AddError (line,
                          "Deprecated character \'"s +
                          std::to_string (static_cast<int> (*i)) + "\'"s,
                          lineN,
                          i - line.begin (),
                          EM_ERROR);
}

void LolcodeParser_t::ParseLine (std::string line, uint32_t lineN)
{
    BEGIN
    std::string token;
    size_t found = line.find ('\n');
    if (found != line.npos)
        line.erase (line.begin () + found, line.end ());
    code_->AddLine (line);
    for (auto i = line.begin (); i < line.end (); i++)
    {

        for (; i < line.end () && *i == ' '; i++)
            CheckIterStr (i, line, lineN);

        if (i == line.end ()) break;

        token.clear ();
        for (; i < line.end () && *i != ' '; i++)
        {
            CheckIterStr (i, line, lineN);
            token += *i;
        }

        code_->AddPrimaryToken (token, lineN, i - line.begin ());
    }
    END (PARSE_LINE)
}

void LolcodeParser_t::ParseTokens ()
{
    BEGIN
    uint32_t oldToken = 0;
    uint32_t oldLine = 0;
    for (auto& currentToken : code_->tokens_)
    {
        if (currentToken.type != TOKEN_UNTYPED) continue;
        std::string currentStr = code_->GetString (currentToken.data);

        if (CheckNum (currentStr, &currentToken.data))
        {
            currentToken.type = TOKEN_NUM;
            continue;
        }

        #define T(ignored_val, str) \
        (void)0); \
        if (currentStr == #str##s) \
        {\
            currentToken.type = TOKEN_##str; \
            continue;\
        } \
        ((void)0

        ((void)0, TOKEN_BLOCK); //! Magic

        #undef T

        if (currentToken.type != TOKEN_UNTYPED)
            oldToken = currentToken.type;
        else
        {
            if (oldToken != TOKEN_YR &&
                oldToken != TOKEN_I &&
                oldToken != TOKEN_A &&
                oldToken != TOKEN_OF &&
                oldToken != TOKEN_AN &&
                oldToken != TOKEN_GIMMEH &&
                oldToken != TOKEN_VISIBLE &&
                oldToken != TOKEN_SAEM &&
                oldLine == currentToken.line)
                cerrs_->AddError (code_->program_[currentToken.line],
                                  "Unexpected token \""s +
                                  currentStr + "\""s,
                                  currentToken.line,
                                  currentToken.shift,
                                  EM_ERROR);
        }
        oldLine = currentToken.line;
    }
    END (PARSE_LINE)
}


