/**
* @file main.cpp
* @brief Command line compiler for LSL
*
* $LicenseInfo:firstyear=2026&license=viewerlgpl$
* Alchemy Viewer Source Code
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

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <boost/program_options.hpp>
#include <filesystem>
#include <chrono>
#include <thread>
#include <vector>
#include <map>
#include "linden_common.h"
#include "lluuid.h"
#include "llerror.h"
#include "llfile.h"
#include "lltimer.h"
#include "llstring.h"
#include "llsingleton.h"
#include "lscript_execute.h"
#include "lscript_rt_interface.h"
#include "lscript_stdlib.h"

class LLScriptPool : public LLSingleton<LLScriptPool>
{
    LLSINGLETON_EMPTY_CTOR(LLScriptPool);

private:
    std::unordered_map<LLUUID, LLScriptExecuteLSL2*> mScripts;

    void initSingleton()
    {
        LSLLibrary library(gScriptLibrary);
        library.init();
        gScriptLibrary.assignExec("llSleep", llSleepWrapper);
    }

public:
    static void llSleepWrapper(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
    {
        instance().llSleep(retval, args, id);
    }

    void addScript(const LLUUID &id, LLScriptExecuteLSL2* execute)
    {
        mScripts[id] = execute;
    }

    int runQuanta(F32 maxTime = 1.0f, bool debug = false)
    {
        if (mScripts.empty())
        {
            return 0;
        }

        F32 time_slice = maxTime / static_cast<F32>(mScripts.size());

        int count = 0;
        for (auto &script_pair : mScripts)
        {
            LLScriptExecuteLSL2 *execute = script_pair.second;

            LLTimer timer;
            const char *error = nullptr;
            U32 events_processed = 0;

            execute->runQuanta(debug, script_pair.first, &error, time_slice, events_processed, timer);
            ++count;
        }

        return count;
    }

    void llSleep(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
    {
        auto it = mScripts.find(id);
        if (it != mScripts.end())
        {
            it->second->setSleep(args[0].mFP);
        }
    }
};

int main(int argc, char *argv[])
{
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Produce help message")
        ("file", boost::program_options::value<std::string>(), "Input file")
        ("debug", "Enable debug mode");

    boost::program_options::positional_options_description pos_desc;
    pos_desc.add("file", 1);

    boost::program_options::variables_map vm;
    boost::program_options::store(
        boost::program_options::command_line_parser(argc, argv)
        .options(desc)
        .positional(pos_desc)
        .run(),
        vm
    );
    boost::program_options::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 1;
    }

    if (!vm.count("file"))
    {
        std::cerr << "Error: Input file not specified" << std::endl;
        return 1;
    }

    std::string filename = vm["file"].as<std::string>();

    LLScriptPool& pool = LLScriptPool::instance();

    LLScriptExecuteLSL2 *execute = NULL;

    if (filename.empty())
    {
        std::cerr << "Error: Input file not found" << std::endl;
        return 1;
    }

    LLFILE* file = LLFile::fopen(filename, "rb");  /* Flawfinder: ignore */
    if(!file)
    {
        std::cerr << "Error: failed to read file" << std::endl;
        return 1;
    }
    else
    {
        execute = new LLScriptExecuteLSL2(file);
        pool.addScript(LLUUID::generateNewID(), execute);
    }

    LLTimer timer;
    float fps = 1.0 / 450.0;

    while(true)
    {
        timer.reset();
        pool.runQuanta(fps, false);

        F32 elapsedTime = timer.getElapsedTimeF32();
        F32 sleepTime = fps - elapsedTime;

        if (sleepTime > 0)
        {
            std::this_thread::sleep_for(std::chrono::duration<F32>(sleepTime));
        }
    }

    return 0;
}
