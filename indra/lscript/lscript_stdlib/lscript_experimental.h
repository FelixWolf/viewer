#ifndef LSCRIPT_EXPERIMENTAL_H
#define LSCRIPT_EXPERIMENTAL_H

#include "lscript_library.h"

class LSLLibraryExperimental
{
private:
    LLScriptLibrary& mLibrary;

public:
    LSLLibraryExperimental(LLScriptLibrary& library)
        : mLibrary(library)
    {}
    void init();
};

#endif // LSCRIPT_EXPERIMENTAL_H
