
class Compiler_t : NZA_t, NonCopiable_t
{
    std::string source_, ast_, errors_;

    CumulativeErrors_t errorList_;

    VirtualCodeRepresentation_t code_;
    LolcodeParser_t parser_;
    LolcodeLexicalAnalyzer_t analyzer_;

    int processingLevel_;
    //! add tree, syntax analysis ...

public:
    void ok ();

    Compiler_t (std::string source, std::string ast, std::string errors);
    ~Compiler_t ();

    bool Compile ();

    bool ManageErrors ();
};

void Compiler_t::ok ()
{
    DEFAULT_OK_BLOCK
}

Compiler_t::Compiler_t (std::string source, std::string ast, std::string errors)
try :
    source_          (source),
    ast_             (ast),
    errors_          (errors),
    errorList_       (),
    code_            (),
    parser_          (&code_, &errorList_),
    analyzer_        (&code_, &errorList_),
    processingLevel_ (0)
{

/*
}
*/
END (CTOR)

Compiler_t::~Compiler_t ()
{}

bool Compiler_t::Compile ()
{
    BEGIN
    parser_.LoadFile (source_);

    if (! ManageErrors ()) return false;

    parser_.ParseTokens ();

    if (! ManageErrors ()) return false;

    parser_.ParseConstructs ();

    if (! ManageErrors ()) return false;

    analyzer_.Process ();

    if (! ManageErrors ()) return false;

    analyzer_.Dump ();

    END (PROCESS)
    return true;
}

bool Compiler_t::ManageErrors ()
{
    processingLevel_++;
    if (errorList_.Empty ()) return true;
    if (errorList_.Ignore () == false)
    {
        errorList_.DumpErrors (errors_, processingLevel_ - 1, source_);
        printf ("%d errors and warnings...\nCannot proceed, check \"%s\"\n",
                errorList_.Size (), errors_.c_str ());
        return false;
    }
    if (errorList_.Ignore () == true)
    {
        errorList_.DumpErrors (errors_, processingLevel_ - 1, source_);
        printf ("%d warnings...\nCheck \"%s\"\nProceed?[y/any key]\n",
                errorList_.Size (), errors_.c_str ());
        if (getche () == 'y')
        {
            errorList_.Clear ();
            return true;
        }
        else
            return false;
    }
    return true;
}
