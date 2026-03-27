#ifndef LSCRIPT_STDLIB_H
#define LSCRIPT_STDLIB_H

#include "lscript_library.h"

class LSLLibrary
{
private:
    LLScriptLibrary& mLibrary;

public:
    LSLLibrary(LLScriptLibrary& library)
        : mLibrary(library)
    {}
    void init();
};

#endif // LSCRIPT_STDLIB_H
