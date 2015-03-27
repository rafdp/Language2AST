#include "Build.h"

int main (int argc, char** argv)
{
    std::string input, output;
    if (argc != 3)
    {
        std::string cmd (argv[0]);
        size_t n = cmd.find_last_of ('\\');
        if (n != cmd.npos) cmd.erase (cmd.begin(), cmd.begin() + n + 1);
        printf ("%s: Invalid number of command line arguments\nExpected input.file output.file\n", cmd.c_str ());
        return 0;
    }
    else
    {
        input = argv[1];
        output = argv[2];
    }

    EXPN = new exception_data (20, "RafExceptionErrors.txt");
    try
    {
        Compiler_t compiler (input, output, "RafCodeErrors.txt");
        if (!compiler.Compile ()) return 0;
        compiler.Save ();

        return 0;
    }
    catch (ExceptionHandler& ex)
    {
        printf ("Exception occurred\nCheck \"RafExceptionErrors.txt\"");
        ex.WriteLog (EXPN);
    }
    catch (std::exception err)
    {
        printf ("Exception occurred: %s\n", err.what ());
    }
    catch (...)
    {
        printf ("Exception occurred\n");
    }
}
