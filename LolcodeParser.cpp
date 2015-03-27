
class LolcodeParser_t : NZA_t
{
    DISABLE_CLASS_COPY (LolcodeParser_t)

    VirtualCodeRepresentation_t* code_;
    CumulativeErrors_t*          cerrs_;
    void ok ();
    void ParseLine (std::string token, uint32_t lineN);
    bool CheckIterStr (std::string::iterator& i, std::string& line, int lineN);
    bool CheckNum (std::string s, int64_t* num);
    void AddError (uint32_t line,
                   std::string msg,
                   uint32_t shift,
                   uint32_t mode);
    void AddErrorUnexpectedToken (const Token_t& tok);
    void AddErrorUndeclared (const Token_t& tok);
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
    uint32_t lineN = 0;
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

void LolcodeParser_t::AddError (uint32_t line,
                                std::string msg,
                                uint32_t shift,
                                uint32_t mode)
{
    cerrs_->AddError (code_->program_[line],
                      msg,
                      line + 1,
                      shift,
                      mode);
}

void LolcodeParser_t::AddErrorUnexpectedToken (const Token_t& tok)
{
    std::string msg ("Unexpected token");
    if (tok.type != TOKEN_NUM)
    {
        msg += " \""s + code_->GetString (tok.data) + "\""s;
    }
    AddError (tok.line, msg, tok.shift, EM_ERROR);
}

void LolcodeParser_t::AddErrorUndeclared (const Token_t& tok)
{
    std::string msg ("\""s);
    if (tok.type != TOKEN_NUM)
    {
        msg += code_->GetString (tok.data) + "\" has not been declared"s;
    }
    AddError (tok.line, msg, tok.shift, EM_ERROR);
}

bool LolcodeParser_t::CheckIterStr (std::string::iterator& i, std::string& line, int lineN)
{
    if (!(*i >= 'a' && *i <= 'z') &&
        !(*i >= 'A' && *i <= 'Z') &&
        !(*i >= '0' && *i <= '9') &&
        *i != '_' &&
        *i != ' ' &&
        *i != '.' &&
        *i != ',' &&
        *i != '?')
    {
        AddError (lineN,
                  "Deprecated character \'\\"s +
                  std::to_string (static_cast<int> (*i)) + "\'"s,
                  i - line.begin (),
                  EM_ERROR);
        return false;
    }
    return true;
}

void LolcodeParser_t::ParseLine (std::string line, uint32_t lineN)
{
    BEGIN
    std::string token;
    size_t found = line.find ('\n');
    bool noErr = true;
    uint32_t shift = 0;
    static bool ignoring = false;


    if (found != line.npos)
        line.erase (line.begin () + found, line.end ());

    code_->AddLine (line);

    for (auto i = line.begin (); i < line.end (); i++)
    {
        if (ignoring)
        {
            for (; i < line.end () - 3; i++)
            {
                if (*(i + 0) == 'T' &&
                    *(i + 1) == 'L' &&
                    *(i + 2) == 'D' &&
                    *(i + 3) == 'R')
                {
                    ignoring = false;
                    i += 4;
                }
            }
            if (ignoring) return;
        }

        noErr = true;
        for (; i < line.end () && *i == ' '; i++)
        {
            if (noErr)
                noErr = CheckIterStr (i, line, lineN);
        }

        if (i < line.end () - 2 &&
            *(i + 0) == 'B' &&
            *(i + 1) == 'T' &&
            *(i + 2) == 'W')
        {
            return;
        }
        if (i < line.end () - 3 &&
            *(i + 0) == 'O' &&
            *(i + 1) == 'B' &&
            *(i + 2) == 'T' &&
            *(i + 3) == 'W')
        {
            ignoring = true;
            return;
        }

        if (i == line.end ()) break;

        token.clear ();
        noErr = true;
        shift = i - line.begin ();
        for (; i < line.end () && *i != ' '; i++)
        {
            if (noErr)
                noErr = CheckIterStr (i, line, lineN);
            if (noErr)
                token += *i;
        }

        code_->AddPrimaryToken (token, lineN, shift);
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
            oldToken = currentToken.type;
            oldLine = currentToken.line;
            continue;
        }

        #define T(ignored_val, str) \
        (void)0); \
        if (currentStr == #str##s) \
        {\
            currentToken.type = TOKEN_##str; \
            break;\
        } \
        ((void)0

        do {((void)0, TOKEN_BLOCK); } //! Magic
        while (false);
        if (currentStr == "RLY?")
        {
            currentToken.type = TOKEN_RLY_QM;
        }

        #undef T

        if (currentToken.type != TOKEN_UNTYPED)
            oldToken = currentToken.type;
        else
        {
            if (oldToken != TOKEN_YR &&
                oldToken != TOKEN_I &&
                oldToken != TOKEN_IZ &&
                oldToken != TOKEN_A &&
                oldToken != TOKEN_OF &&
                oldToken != TOKEN_AN &&
                oldToken != TOKEN_GIMMEH &&
                oldToken != TOKEN_VISIBLE &&
                oldToken != TOKEN_SAEM &&
                oldLine == currentToken.line)
                AddErrorUnexpectedToken (currentToken);
            oldToken = currentToken.type = TOKEN_VAR;
        }
        oldLine = currentToken.line;
    }
    END (PARSE_TOKENS)
}

void LolcodeParser_t::ParseConstructs ()
{
    BEGIN
    StringTable_t table; // var, funcs, loops
    //ServiceCopyOnDestroy_t (&table, &code_->strings_);
    uint8_t nVars = 0;

    bool foundStart = false;

    for (auto currentToken = code_->tokens_.begin ();
         currentToken < code_->tokens_.end ();
         currentToken++)
    {
        if (currentToken->type == TOKEN_UNTYPED)
            _EXC_N (UNTYPED_ON_CONSTRUCTS,
                    "Found token_untyped %s" _ code_->GetString (currentToken->data).c_str ())

        if (currentToken == code_->tokens_.end () - 1 && currentToken->type != TOKEN_KTHXBYE)
        {
            AddError (currentToken->line,
                      "Expected token \"KTHXBYE\" at the end of input"s,
                      currentToken->shift,
                      EM_ERROR);
        }

        if (currentToken->type == TOKEN_KTHXBYE)
        {
            code_->tokens_.erase (currentToken, code_->tokens_.end ());
            code_->strings_ = table;
            return;
        }

        if (!foundStart &&
            currentToken == code_->tokens_.begin () &&
            currentToken->type != TOKEN_HAI)
        {
            AddError (currentToken->line,
                      "Expected token \"HAI\" at the beginning of input"s,
                      currentToken->shift,
                      EM_ERROR);
        }

        if (!foundStart && currentToken == code_->tokens_.begin () && currentToken->type == TOKEN_HAI)
        {
            code_->tokens_.erase (currentToken);
            foundStart = true;
        }



        #define CHECK_END \
        if (currentToken == code_->tokens_.end () - 1) \
        { \
            AddError (currentToken->line, \
                      "Expected token after \""s + \
                      code_->GetString (currentToken->data) + \
                      "\""s, \
                      currentToken->shift, \
                      EM_ERROR); \
            return; \
        }

        #define DEFAULT_CASE default: AddErrorUnexpectedToken (*(currentToken + 1)); break;
        #define DROP_NEXT currentToken = code_->tokens_.erase (currentToken + 1); currentToken--;

        #define IF(tok) if (currentToken->type == TOKEN_##tok) \
        { CHECK_END switch ((currentToken + 1)->type) {

        #define ENDIF DEFAULT_CASE } continue; }

        #define DEPRECATED(tok) IF(tok) ENDIF

        #define MERGE(cur, tok) case TOKEN_##tok: DROP_NEXT currentToken->type = TOKEN_##cur##_##tok; currentToken--; break;

        #define ARITHMETIC_CASES \
            case TOKEN_SUM: \
            case TOKEN_DIFF: \
            case TOKEN_PRODUKT: \
            case TOKEN_QUOSHUNT: \
            case TOKEN_BIGGR: \
            case TOKEN_SMALLR: \
            case TOKEN_SQRT: \
            case TOKEN_VAR: \
            case TOKEN_NUM: \
                break;

        #define LOGIC_CASES \
            case TOKEN_VAR: \
            case TOKEN_WON: \
            case TOKEN_EITHER: \
            case TOKEN_BOTH: \
            case TOKEN_NOT: \
            case TOKEN_DIFFRINT: \
            case TOKEN_NUM: \
                break;

        #define MERGE_OF(tok) IF(tok) MERGE (tok, OF) ENDIF

        #define CHECK_VAR \
        if (table.Registered (code_->GetString ((currentToken + 1)->data)) == -1) \
            AddErrorUndeclared (*(currentToken + 1));

        #define REG_LOCAL_TABLE(type)\
        case TOKEN_VAR: \
        { \
            currentToken++; \
            if (table.Registered (code_->GetString (currentToken->data)) == -1) \
            { \
                if (TOKEN_##type == TOKEN_VAR) \
                { \
                    if (nVars == 8) \
                    { \
                        AddError (currentToken->line, \
                                  "Only 8 vars allowed", \
                                  currentToken->shift, \
                                  EM_ERROR); \
                        currentToken--; \
                        break; \
                    } \
                    else nVars++; \
                } \
                currentToken->data = table.Register (code_->GetString (currentToken->data)); \
                code_->SetType (currentToken->data, TOKEN_##type); \
            } \
            else  \
                AddError (currentToken->line, \
                          "Token \""s + code_->GetString (currentToken->data) + "\" has already been registered",\
                          currentToken->shift, \
                          EM_ERROR); \
            currentToken--; \
            break; \
        }

        #undef IN


        IF (I_HAS_A)
            REG_LOCAL_TABLE (VAR)
        ENDIF

        IF (HOW_IZ_I)
            REG_LOCAL_TABLE (FUNC)
        ENDIF

        IF (IM_IN_YR)
            REG_LOCAL_TABLE (LOOP)
        ENDIF

        if ((currentToken + 1)->type == TOKEN_VAR)
        {
            CHECK_VAR
            else
            {
                currentToken++;
                int32_t dataT = table.Registered (code_->GetString (currentToken->data));
                if (dataT != -1)
                {
                    currentToken->data = table.Registered (code_->GetString (currentToken->data));
                    currentToken->type = code_->GetType (currentToken->data);
                }
                currentToken--;
            }
        }

        IF (WILE)
            LOGIC_CASES
        ENDIF


        DEPRECATED (HAS)

        IF (R)
            ARITHMETIC_CASES
        ENDIF

        DEPRECATED (OF)

        IF (AN)
            ARITHMETIC_CASES
        ENDIF

        MERGE_OF (SUM)
        MERGE_OF (DIFF)
        MERGE_OF (PRODUKT)
        MERGE_OF (QUOSHUNT)
        MERGE_OF (PWR)
        MERGE_OF (BIGGR)
        MERGE_OF (SMALLR)
        MERGE_OF (WON)
        MERGE_OF (EITHER)
        MERGE_OF (SQRT)
        MERGE_OF (COS)
        MERGE_OF (SIN)
        MERGE_OF (DER)

        DEPRECATED (A)


        IF (I)
            MERGE(I, HAS)
            MERGE(I, IZ)
        ENDIF

        IF (NOT)
            LOGIC_CASES
        ENDIF

        IF (BOTH)
            MERGE (BOTH, OF)
            MERGE (BOTH, SAEM)
        ENDIF

        DEPRECATED (SAEM)

        IF (DIFFRINT)
            ARITHMETIC_CASES
        ENDIF

        IF (VISIBLE)
            ARITHMETIC_CASES
        ENDIF

        IF (COS_OF)
            ARITHMETIC_CASES
        ENDIF

        IF (SIN_OF)
            ARITHMETIC_CASES
        ENDIF

        IF (DER_OF)
            ARITHMETIC_CASES
        ENDIF

        IF (SQRT_OF)
            ARITHMETIC_CASES
        ENDIF

        IF (GIMMEH)
            case TOKEN_VAR:
                break;
        ENDIF

        IF (ITZ)
            ARITHMETIC_CASES
        ENDIF

        IF (O)
            MERGE (O, RLY_QM)
        ENDIF

        DEPRECATED (RLY_QM)

        IF (YA)
            MERGE (YA, RLY)
        ENDIF

        DEPRECATED (RLY)

        IF (NO)
            MERGE (NO, WAI)
        ENDIF

        DEPRECATED (WAI)

        IF (IM)
            MERGE (IM, IN)
            MERGE (IM, OUTTA)
        ENDIF

        DEPRECATED (IN)

        DEPRECATED (YR)

        IF (UPPIN)
            MERGE (UPPIN, YR)
        ENDIF

        IF (NERFIN)
            MERGE (NERFIN, YR)
        ENDIF

        IF (TIL)
            LOGIC_CASES
        ENDIF

        DEPRECATED (OUTTA)

        IF (HOW)
            MERGE (HOW, IZ)
        ENDIF

        DEPRECATED (IZ)

        IF (IF)
            MERGE (IF, YOU)
        ENDIF

        DEPRECATED (YOU)
        DEPRECATED (SAY)
        DEPRECATED (SO)

        IF (IF_YOU)
            MERGE (IF_YOU, SAY)
        ENDIF

        IF (BOTH_SAEM)
            ARITHMETIC_CASES
        ENDIF

        IF (I_HAS)
            MERGE (I_HAS, A)
        ENDIF

        IF (HOW_IZ)
            MERGE (HOW_IZ, I)
        ENDIF

        IF (IM_IN)
            MERGE (IM_IN, YR)
        ENDIF

        IF (IM_OUTTA)
            MERGE (IM_OUTTA, YR)
        ENDIF

        IF (UPPIN_YR)
            case TOKEN_VAR:
                break;
        ENDIF

        IF (NERFIN_YR)
            case TOKEN_VAR:
                break;
        ENDIF

        IF (SUM_OF)
            ARITHMETIC_CASES
        ENDIF

        IF (DIFF_OF)
            ARITHMETIC_CASES
        ENDIF

        IF (PRODUKT_OF)
            ARITHMETIC_CASES
        ENDIF

        IF (QUOSHUNT_OF)
            ARITHMETIC_CASES
        ENDIF

        IF (PWR_OF)
            ARITHMETIC_CASES
        ENDIF

        IF (BIGGR_OF)
            ARITHMETIC_CASES
        ENDIF

        IF (SMALLR_OF)
            ARITHMETIC_CASES
        ENDIF

        IF (WON_OF)
            LOGIC_CASES
        ENDIF

        IF (EITHER_OF)
            LOGIC_CASES
        ENDIF

        IF (BOTH_OF)
            LOGIC_CASES
        ENDIF

        IF (IF_YOU_SAY)
            MERGE (IF_YOU_SAY, SO)
        ENDIF

        IF (IM_OUTTA_YR)
            case TOKEN_LOOP:
                break;
        ENDIF

        IF (I_IZ)
            case TOKEN_FUNC:
                break;
        ENDIF

        #undef CHECK_END
        #undef DEFAULT_CASE
        #undef DROP_NEXT
        #undef IF
        #undef ENDIF
        #undef DEPRECATED
        #undef MERGE
        #undef MERGE_OF
        #undef ARITHMETIC_CASES
        #undef LOGIC_CASES
        #undef CHECK_VAR
        #undef REG_LOCAL_TABLE
        #define IN

    }


    END (PARSE_CONSTRUCTS)
}


