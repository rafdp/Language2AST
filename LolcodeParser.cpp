
class LolcodeParser_t : NZA_t
{
    DISABLE_CLASS_COPY (LolcodeParser_t)

    VirtualCodeRepresentation_t* code_;
    CumulativeErrors_t*          cerrs_;
    void ok ();
    void ParseLine (std::string token, uint32_t lineN);
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

void LolcodeParser_t::ParseLine (std::string line, uint32_t lineN)
{
    std::string token;
    for (auto i = line.begin (); i < line.end (); i++)
    {
        while (i < line.end () && *i == ' ') i++;
        if (i == line.end ()) break;

        token.clear ();
        while (i < line.end () && *i != ' ') {token += *i; i++;}
        code_->AddPrimaryToken (token);
    }
}
