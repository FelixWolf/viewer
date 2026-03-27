
/**
* @file library.cpp
* @brief Non-modifying LSL functions implementations
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

#include "lscript_experimental.h"
#include "linden_common.h"
#include "lscript_library.h"
#include "lscript_alloc.h"
#include <netinet/in.h>  // For htonl (on POSIX)
#include "llbase64.h"

void llList2TypedString(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;

    std::string serializedData;

    int listSize = args[0].getListLength();
    LLScriptLibData *current = &args[0];
    for(int i = 0; i < listSize; i++)
    {
        current = current->mListp;
        // Type identifiers cannot contain a number or be any of ABCDEF
        // Current type identifiers:
        // i = integer
        // r = real / float
        // k = key
        // K = arbitrary string as key
        // s = string
        // v = vector
        // q = quaternion
        if(current->mType == LST_INTEGER)
        {
            if (current->mInteger == 0)
                serializedData += "i";
            else
                serializedData += "i" + std::to_string(current->mInteger);
        }
        else if(current->mType == LST_FLOATINGPOINT)
        {
            std::string source = std::to_string(current->mFP);
            std::size_t start = source.find_first_not_of("0");

            if (start == std::string::npos)
                serializedData += "r";
            else
            {
                std::size_t end = source.find_last_not_of("0.");
                serializedData += "r" + source.substr(start, end - start + 1);
            }
        }
        else if(current->mType == LST_KEY)
        {
            if (sizeof(current->mKey) == 0)
            {
                serializedData += "k";
            }
            else if (LLUUID::validate(current->mKey))
            {
                LLUUID key(current->mKey);
                if (!key.isNull())
                    serializedData += "k" + key.asString();
            }
            else
            {
                LLWString srcstr = utf8str_to_wstring(current->mKey);
                size_t size = srcstr.size();
                if (size == 0)
                    serializedData += "K";
                else
                    serializedData += "K" + std::to_string(size) + ":" + current->mKey;
            }
        }
        else if(current->mType == LST_STRING)
        {
            LLWString srcstr = utf8str_to_wstring(current->mString);
            size_t size = srcstr.size();
            if (size == 0)
                serializedData += "s";
            else
                serializedData += "s" + std::to_string(size) + ":" + current->mString;
        }
        else if(current->mType == LST_VECTOR)
        {
            serializedData += "v";

            if (!current->mVec.isExactlyZero())
            {
                // In theory, can just do 0, 1, 2, but if VX, VY, or VZ changes,
                // we will run into issues. Compiler should optimize it.
                const U32 axises[3] = {VX, VY, VZ};
                for(int i = 0; i < 3; i++)
                {
                    if (current->mVec.mV[axises[i]] != 0)
                    {
                        std::string source = std::to_string(current->mVec.mV[axises[i]]);
                        std::size_t start = source.find_first_not_of("0");

                        if (start != std::string::npos)
                        {
                            std::size_t end = source.find_last_not_of("0.");
                            serializedData += source.substr(start, end - start + 1);
                        }
                    }
                    if(i != 2)
                        serializedData += ",";
                }
            }
        }
        else if(current->mType == LST_QUATERNION)
        {
            serializedData += "q";

            // Quaternions have no isExactlyZero(). :(
            if (current->mQuat.mQ[VX] != 0 || current->mQuat.mQ[VY] != 0 ||
                current->mQuat.mQ[VZ] != 0 || current->mQuat.mQ[VW] != 0
            )
            {
                // In theory, can just do 0, 1, 2, but if VX, VY, VZ, or VW changes,
                // we will run into issues. Compiler should optimize it.
                const U32 axises[4] = {VX, VY, VZ, VW};
                for (int i = 0; i < 4; i++)
                {
                    if (current->mQuat.mQ[axises[i]] != 0)
                    {
                        std::string source = std::to_string(current->mQuat.mQ[axises[i]]);
                        std::size_t start = source.find_first_not_of("0");

                        if (start != std::string::npos)
                        {
                            std::size_t end = source.find_last_not_of("0.");
                            serializedData += source.substr(start, end - start + 1);
                        }
                    }
                    if(i != 3)
                        serializedData += ",";
                }
            }
        }
        else
        {
            serializedData += "?";
        }
    }

    retval->mString = new char[serializedData.length() + 1];

    std::strcpy(retval->mString, serializedData.c_str());
}

void llTypedString2List(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_LIST;

    LLScriptLibData *current = retval;

    LLWString payload = utf8str_to_wstring(args[0].mString);
    for(int i = 0; i < payload.size();)
    {
        llwchar entryType = payload[i];
        i++;
        switch(entryType)
        {
            case 'i':
            {
                current->mListp = new LLScriptLibData;
                current = current->mListp;
                current->mType = LST_INTEGER;

                int start = i;
                for (; i < payload.size(); i++)
                {
                    if (i == start && payload[i] == '-')
                        continue;
                    else if ((i-1 == start && payload[i-1] == '0' && payload[i] == 'x' ))
                        continue;
                    else if ((i-2 == start && payload[i-2] == '-' && payload[i-1] == '0' && payload[i] == 'x' ))
                        continue;
                    else if (payload[i] >= '0' && payload[i] <= '9')
                        continue;
                    break;
                }

                // std::stoi does conversion from hex to integer automagically for us!
                try
                {
                    current->mInteger = std::stoi(wstring_to_utf8str(payload.substr(start, i - start + 1)));
                }
                catch (const std::invalid_argument& e)
                {
                    current->mInteger = 0;
                }
            }
            break;
            case 'r':
            {
                current->mListp = new LLScriptLibData;
                current = current->mListp;
                current->mType = LST_FLOATINGPOINT;

                int start = i;
                bool foundDot = false;
                for(; i < payload.size(); i++)
                {
                    if (i == start && payload[i] == '-')
                        continue;
                    else if (payload[i] == '.' && foundDot == false)
                    {
                        foundDot = true;
                        continue;
                    }
                    else if (payload[i] >= '0' && payload[i] <= '9')
                        continue;
                    break;
                }

                try
                {
                    current->mFP = std::stof(wstring_to_utf8str(payload.substr(start, i - start + 1)));
                }
                catch (const std::invalid_argument& e)
                {
                    current->mFP = 0;
                }
            }
            break;
            case 'k':
            {
                current->mListp = new LLScriptLibData;
                current = current->mListp;
                current->mType = LST_KEY;

                // Only copy it if it is a valid key, otherwise assume it is null key
                // then continue to the next element

                if(i + UUID_STR_SIZE < payload.size())
                {
                    std::string utf8str = wstring_to_utf8str(payload.substr(i, UUID_STR_SIZE - 1));
                    bool validKey = LLUUID::validate(utf8str);

                    if (validKey)
                    {
                        const char* srckey = utf8str.c_str();
                        size_t length = std::strlen(srckey);
                        current->mKey = new char[length + 1];
                        std::strcpy(current->mKey, srckey);
                        i += utf8str.size();
                    }
                    else
                    {
                        std::string nullKeyStr = LLUUID::null.asString();
                        size_t length = nullKeyStr.length();
                        current->mKey = new char[length + 1];
                        std::strcpy(current->mKey, nullKeyStr.c_str());
                    }
                }
            }
            break;
            case 's':
            case 'K': // Arbitrary keys
            {
                current->mListp = new LLScriptLibData;
                current = current->mListp;
                if (entryType == 's')
                    current->mType = LST_STRING;
                else if (entryType == 'K')
                    current->mType = LST_KEY;

                int start = i;
                for (; i < payload.size(); i++)
                {
                    if (payload[i] >= '0' && payload[i] <= '9')
                        continue;
                    else if (payload[i] == ':')
                    {
                        i++;
                    }
                    break;
                }

                if(start == i)
                    break;

                int size = 0;
                try
                {
                    size = std::stoi(wstring_to_utf8str(payload.substr(start, i - start)));
                }
                catch (const std::invalid_argument& e)
                {
                    size = 0;
                }

                std::string utf8str = wstring_to_utf8str(payload.substr(i, size));
                const char* value = utf8str.c_str();
                size_t length = std::strlen(value);

                if (entryType == 's')
                {
                    current->mString = new char[length + 1];
                    std::strcpy(current->mString, value);
                }
                else if (entryType == 'K')
                {
                    current->mKey = new char[length + 1];
                    std::strcpy(current->mKey, value);
                }

                i += size;
            }
            break;
            case 'v':
            case 'q':
            {
                int size = 3;
                if(payload[i-1] == 'q')
                    size = 4;

                const U32 axises[4] = {VX, VY, VZ, VW};

                current->mListp = new LLScriptLibData;
                current = current->mListp;

                if (size == 3)
                    current->mType = LST_VECTOR;
                else if (size == 4)
                    current->mType = LST_QUATERNION;

                for (int axis = 0; axis < size; axis++)
                {
                    if (i >= payload.size())
                        break;

                    if (!(payload[i] >= '0' || payload[i] <= '9' || payload[i] == '-' || payload[i] == ',' || payload[i] == '.'))
                    {
                        i--;
                        break;
                    }

                    // Skip over seperators
                    if (payload[i] == ',')
                    {
                        i++;
                        axis--;
                        continue;
                    }

                    int start = i;
                    bool foundDot = false;
                    for (; i < payload.size(); i++)
                    {
                        if (i == start && payload[i] == '-')
                            continue;
                        else if (payload[i] == '.' && foundDot == false)
                        {
                            foundDot = true;
                            continue;
                        }
                        else if (payload[i] >= '0' && payload[i] <= '9')
                            continue;
                        else if (i != start)
                            i--;
                        else if (payload[i] != ',')
                            i--;
                        break;
                    }

                    try
                    {
                        if (size == 3)
                            current->mVec.mV[axises[axis]] = std::stof(wstring_to_utf8str(payload.substr(start, i - start + 1)));
                        else if (size == 4)
                            current->mQuat.mQ[axises[axis]] = std::stof(wstring_to_utf8str(payload.substr(start, i - start + 1)));
                    }
                    catch (const std::invalid_argument& e)
                    {
                        if (size == 3)
                            current->mVec.mV[axises[axis]] = 0;
                        else if (size == 4)
                            current->mQuat.mQ[axises[axis]] = 0;
                    }
                    if (i != size - 1)
                        i++;
                }
            }
            break;
            // White spaces are ignored, allows for pretty printing
            case ' ':
            case '\n':
            case '\t':
            case '\r':
                break;
            default:
                printf("Invalid type %c\n", payload[i]);
                // Not sure what the behavior should be here
                // Probably throw a LSL error
                // Various possibilities here:
                // 1. Skip invalids
                // 2. Return what we already parsed
                // 3. Return none
                break;
        }
    }
}

void llFloatToBase64(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;

    F32 floatval = args[0].mFP;
    uint32_t value;
    memcpy(&value, &floatval, sizeof(floatval));

    value = htonl(value);

    std::string binary(4, '\0');
    binary[0] = (value >> 24) & 0xFF;
    binary[1] = (value >> 16) & 0xFF;
    binary[2] = (value >> 8) & 0xFF;
    binary[3] = value & 0xFF;

    std::string encoded = LLBase64::encode(reinterpret_cast<const U8*>(binary.data()), binary.size());

    retval->mString = strdup(encoded.c_str());
}

void llBase64ToFloat(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    std::string decoded = LLBase64::decodeAsString(args[0].mString);

    if (decoded.size() < 4)
    {
        retval->mFP = 0;
        return;
    }

    int value = (static_cast<unsigned char>(decoded[0]) << 24) |
                (static_cast<unsigned char>(decoded[1]) << 16) |
                (static_cast<unsigned char>(decoded[2]) << 8)  |
                (static_cast<unsigned char>(decoded[3]));

    value = htonl(value);

    memcpy(&(retval->mFP), &value, sizeof(retval->mFP));
}


void LSLLibraryExperimental::init()
{
    mLibrary.assignExec("llList2TypedString", llList2TypedString);
    mLibrary.assignExec("llTypedString2List", llTypedString2List);
    mLibrary.assignExec("llFloatToBase64", llFloatToBase64);
    mLibrary.assignExec("llBase64ToFloat", llBase64ToFloat);
}
