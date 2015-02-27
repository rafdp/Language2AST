#ifndef BUILD_H_INCLUDED
#define BUILD_H_INCLUDED

#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#include "Exception.h"
#include <assert.h>
#include <iostream>
#include <locale>
#include <vector>
#include <algorithm>

template <typename T>
T __zero__ ()
{
    T res;
    return res;
}
#define ZERO(T) (__zero__<T> ())



#include "Errors.h"
#include "Node.cpp"
#include "Tex.cpp"
#include "Differentiate.cpp"
#include "RecursiveDescentParser.cpp"


#endif // BUILD_H_INCLUDED
