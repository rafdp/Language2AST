#include "Build.h"

int main()
{
    EXPN = new exception_data (20, "ExceptionErrors.txt");
    try
    {
        Compiler_t compiler ("code.txt", "ast.txt", "CodeErrors.txt");
        if (!compiler.Compile ()) return 0;

        return 0;
    }
    catch (ExceptionHandler& ex)
    {
        printf ("Exception occurred\nCheck \"ExceptionErrors.txt\"");
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
