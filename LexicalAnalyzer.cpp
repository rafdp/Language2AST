
class LexicalAnalyzer_t : NZA_t, NonCopiable_t
{
    VirtualCodeRepresentation_t* code_;
    void ok ();
public:
    LexicalAnalyzer_t (VirtualCodeRepresentation_t* code);
    ~LexicalAnalyzer_t ();

    void LoadFile (std::string filename, uint32_t lineN);
    void ParseLine (std::string token);
    void Analyze ();
};

void LexicalAnalyzer_t::ok ()
{
    DEFAULT_OK_BLOCK
    if (code_ == nullptr)
        _EXC_N (NULL_CODE, "Null VirtualCodeRepresentation_t pointer")
}

LexicalAnalyzer_t::LexicalAnalyzer_t (VirtualCodeRepresentation_t* code)
try :
    code_ (code)
{
    ok ();
}
END (ERROR_CTOR)

LexicalAnalyzer_t::~LexicalAnalyzer_t (VirtualCodeRepresentation_t* code)
try :
    code_ (code)
{
    ok ();
}
END (ERROR_DTOR)

void LexicalAnalyzer_t::LoadFile (std::string filename)
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

void LexicalAnalyzer_t::ParseLine (std::string line, uint32_t lineN)
{
    std::string token;
    for (auto i = line.begin (); i < line.end (); i++)
    {
        while (i < line.end () && *i == ' ') i++;
        if (i == line.end ()) break;

        token.clear ();
        while (i < line.end () && *i != ' ') {token += *i; i++};
        code_->AddPrimaryToken (token);
    }
}
