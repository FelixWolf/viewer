/**
* @file main.cpp
* @brief Command line compiler for LSL
*
* $LicenseInfo:firstyear=2026&license=viewerlgpl$
* Alchemy Viewer Source Code
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
#include "linden_common.h"
#include "lluuid.h"
#include "llerror.h"
#include "llfile.h"
#include "lltimer.h"
#include "llstring.h"
#include "lscript_rt_interface.h"

int main(int argc, char *argv[])
{
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("file", boost::program_options::value<std::string>(), "input file")
        ("output", boost::program_options::value<std::string>(), "output file")
        ("error", boost::program_options::value<std::string>(), "error file")
        ("debug", boost::program_options::value<std::string>(), "debug file")
        ("target", boost::program_options::value<std::string>(), "compile target: assembly | bytecode | cil")
        ("class", boost::program_options::value<std::string>(), "Class ID")
        ("god", "Compile in Godmode");

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
        std::cout << desc << "\n";
        return 1;
    }

    if (!vm.count("file"))
    {
        std::cerr << "Error: Input file not specified\n";
        return 1;
    }

    LLUUID uid = LLUUID::generateNewID();

    LScriptCompileTarget compile_target = LSCRIPT_TARGET_BYTE_CODE;
    if (vm.count("target"))
    {
        std::string target = vm["target"].as<std::string>();
        LLStringUtil::toLower(target);
        if (target == "assembly")
        {
            compile_target = LSCRIPT_TARGET_ASSEMBLY;
        }
        else if (target == "cil" || target == "mono")
        {
            compile_target = LSCRIPT_TARGET_CIL_ASSEMBLY;
        }
        else
        {
            compile_target = LSCRIPT_TARGET_BYTE_CODE;
        }
    }

    std::string dst_filename;
    if (!vm.count("output"))
    {
        std::filesystem::path inputFilePath(vm["file"].as<std::string>());
        if (compile_target == LSCRIPT_TARGET_BYTE_CODE)
        {
            dst_filename = inputFilePath.stem().string() + ".lso";
        }
        else if (compile_target == LSCRIPT_TARGET_CIL_ASSEMBLY)
        {
            dst_filename = inputFilePath.stem().string() + ".cil";
        }
        else
        {
            dst_filename = inputFilePath.stem().string() + ".asm";
        }
    }
    else
    {
        dst_filename = vm["output"].as<std::string>();
    }

    std::string err_filename;
    if (!vm.count("error"))
    {
        std::filesystem::path tempDir = std::filesystem::temp_directory_path();
        err_filename = (tempDir / uid.asString().c_str()).string();
    }
    else
    {
        err_filename = vm["error"].as<std::string>();
    }

    std::string classname;
    if (vm.count("class"))
    {
        classname = vm["class"].as<std::string>();
    }
    else
    {
        classname = uid.asString();
    }

    auto start = std::chrono::high_resolution_clock::now();
    bool success;
    if (vm.count("debug"))
    {
        success = lscript_compile(vm["file"].as<std::string>().c_str(),
                        dst_filename.c_str(),
                        err_filename.c_str(),
                        compile_target,
                        classname.c_str(),
                        vm.count("god") > 0);
    }
    else
    {
        success = lscript_compile(vm["file"].as<std::string>().c_str(),
                        dst_filename.c_str(),
                        err_filename.c_str(),
                        compile_target,
                        classname.c_str(),
                        vm.count("god") > 0);
    }
    auto stop = std::chrono::high_resolution_clock::now();

    if(success)
    {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << "Compile successful!" << std::endl;
    }
    else
    {
        std::cout << "Compile failed." << std::endl;

        LLFILE* fp = LLFile::fopen(err_filename, "r");
        if(fp)
        {
            char buffer[MAX_STRING];        /*Flawfinder: ignore*/
            std::string line;
            while(!feof(fp)) 
            {
                if (fgets(buffer, MAX_STRING, fp) == NULL)
                {
                    buffer[0] = '\0';
                }

                if(feof(fp))
                {
                    break;
                }
                else
                {
                    line.assign(buffer);
                    LLStringUtil::stripNonprintable(line);
                    std::cerr << line << std::endl;
                }
            }
            fclose(fp);
        }
    }

    //If we weren't asked to output the error file, delete it when we are done
    if (!vm.count("error"))
    {
        if (LLFile::isfile(err_filename))
        {
            LLFile::remove(err_filename);
        }
    }
    return success != true;
}
