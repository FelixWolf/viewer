/**
* @file library.cpp
* @brief Non-standard LSL functions implementations
*
* $LicenseInfo:firstyear=2026&license=viewerlgpl$
* Kyler "Félix" Eastridge
* Copyright (C) 2010, Linden Research, Inc.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation;
* version 2.1 of the License only.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*
* $/LicenseInfo$
*/

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
