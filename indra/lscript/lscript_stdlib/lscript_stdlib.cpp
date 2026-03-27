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

#include "lscript_stdlib.h"
#include "linden_common.h"
#include "lscript_library.h"
#include "lscript_alloc.h"
#include "llrand.h"
#include "lltimer.h"
#include "lldate.h"
#include "llmd5.h"
#include "llbase64.h"
#include <cstring>  // For memcpy
#include <netinet/in.h>  // For htonl (on POSIX)
#include <openssl/sha.h> // OpenSSL library for SHA-1 hashing
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <algorithm>
#include <ctime>

void llSin(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = sin(args[0].mFP);
}

void llCos(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = cos(args[0].mFP);
}

void llTan(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = tan(args[0].mFP);
}

void llAtan2(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = atan2(args[0].mFP, args[1].mFP);
}

void llSqrt(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = sqrt(args[0].mFP);
}

void llPow(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = pow(args[0].mFP, args[1].mFP);
}

void llAbs(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    retval->mInteger = abs(args[0].mInteger);
}

void llFabs(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = fabs(args[0].mFP);
}

void llFrand(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = ll_frand(args[0].mFP);
}

void llFloor(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    retval->mInteger = llfloor(args[0].mFP);
}

void llCeil(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    retval->mInteger = llceil(args[0].mFP);
}

void llRound(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    retval->mInteger = ll_round(args[0].mFP);
}

void llVecMag(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = args[0].mVec.magVec();
}

void llVecNorm(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_VECTOR;
    args[0].mVec.normalize();
    retval->mVec = args[0].mVec;
}

void llVecDist(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = dist_vec(args[0].mVec, args[1].mVec);
}

void llRot2Euler(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_VECTOR;
    args[0].mQuat.getEulerAngles(&retval->mVec.mV[VX], &retval->mVec.mV[VY], &retval->mVec.mV[VZ]);
}

void llEuler2Rot(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_QUATERNION;
    retval->mQuat.setEulerAngles(args[0].mVec.mV[VX], args[0].mVec.mV[VY], args[0].mVec.mV[VZ]);
}

void llAxes2Rot(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_QUATERNION;
    retval->mQuat = LLQuaternion(args[0].mVec, args[1].mVec, args[2].mVec);
}

void llRot2Fwd(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_VECTOR;
    retval->mVec = LLVector3(1, 0, 0) * args[0].mQuat;
    retval->mVec.normalize();
}

void llRot2Left(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_VECTOR;
    retval->mVec = LLVector3(0, 1, 0) * args[0].mQuat;
    retval->mVec.normalize();
}

void llRot2Up(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_VECTOR;
    retval->mVec = LLVector3(0, 0, 1) * args[0].mQuat;
    retval->mVec.normalize();
}

void llRotBetween(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_QUATERNION;
    retval->mQuat.shortestArc(args[0].mVec, args[1].mVec);
}

void llGetWallclock(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    struct tm* time = utc_to_pacific_time(time_corrected(), is_daylight_savings());
    retval->mFP = time->tm_hour * 3600 + time->tm_min * 60 + time->tm_sec;
}

void llGetSubString(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;

    LLWString sourceStr = utf8str_to_wstring(args[0].mString);
    int strLen = sourceStr.size();
    int start = args[1].mInteger;
    int end = args[2].mInteger;

    // Handle negative indices
    if (start < 0) start += strLen;
    if (end < 0) end += strLen;

    // Ensure start and end are within bounds
    if (start < 0) start = 0;
    if (end >= strLen) end = strLen - 1;

    // Determine the behavior based on the comparison of start and end
    if (start <= end)
    {
        // Normal substring extraction
        std::string utf8str = wstring_to_utf8str(sourceStr.substr(start, end - start + 1));
        const char* destStr = utf8str.c_str();
        size_t length = std::strlen(destStr);
        retval->mString = new char[length + 1];
        std::strcpy(retval->mString, destStr);
    }
    else
    {
        // Exclusion case: return [0..end] + [start..end_of_string]
        std::string utf8str = wstring_to_utf8str(sourceStr.substr(0, end + 1) + sourceStr.substr(start));
        const char* destStr = utf8str.c_str();
        size_t length = std::strlen(destStr);
        retval->mString = new char[length + 1];
        std::strcpy(retval->mString, destStr);
    }
}

//TODO: Test me
void llDeleteSubString(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;
    LLWString sourceStr = utf8str_to_wstring(args[0].mString);
    int strLen = sourceStr.size();
    int start = args[1].mInteger;
    int end = args[2].mInteger;

    // Handle negative indices
    if (start < 0) start += strLen;
    if (end < 0) end += strLen;

    // Ensure start and end are within bounds
    if (start < 0) start = 0;
    if (end >= strLen) end = strLen - 1;

    if (start <= end)
    {
        std::string utf8str = wstring_to_utf8str(sourceStr.substr(0, start) + sourceStr.substr(end + 1));
        const char* destStr = utf8str.c_str();
        size_t length = std::strlen(destStr);
        retval->mString = new char[length + 1];
        std::strcpy(retval->mString, destStr);
    }
    else
    {
        // Exclusion case: delete [0..end] and [start..strLen-1], keep [end+1..start-1]
        std::string utf8str = wstring_to_utf8str(sourceStr.substr(end + 1, start - end - 1));
        const char* destStr = utf8str.c_str();
        size_t length = std::strlen(destStr);
        retval->mString = new char[length + 1];
        std::strcpy(retval->mString, destStr);
    }
}

//TODO: Test me
void llInsertString(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;
    LLWString sourceStr = utf8str_to_wstring(args[0].mString);
    LLWString insertStr = utf8str_to_wstring(args[2].mString);
    int insertPos = args[1].mInteger;

    // Bounds check
    if (insertPos < 0)
        insertPos = 0;
    else if (insertPos > sourceStr.size())
        insertPos = sourceStr.size();

    std::string utf8str = wstring_to_utf8str(sourceStr.substr(0, insertPos) + insertStr + sourceStr.substr(insertPos));
    const char* destStr = utf8str.c_str();
    size_t length = std::strlen(destStr);
    retval->mString = new char[length + 1];
    std::strcpy(retval->mString, destStr);
}

void llToUpper(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;
    LLWString orig = utf8str_to_wstring(args[0].mString);
    LLWStringUtil::toUpper(orig);
    retval->mString = strdup(wstring_to_utf8str(orig).c_str());
}

void llToLower(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;
    LLWString orig = utf8str_to_wstring(args[0].mString);
    LLWStringUtil::toLower(orig);
    retval->mString = strdup(wstring_to_utf8str(orig).c_str());
}

void llStringLength(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    retval->mInteger = utf8str_to_wstring(args[0].mString, strlen(args[0].mString)).length();
}

//TODO: Test me
void llAxisAngle2Rot(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_QUATERNION;
    retval->mQuat = LLQuaternion(args[1].mFP, args[0].mVec);
}

//TODO: Test me
void llRot2Axis(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_VECTOR;
    F32 angle_radians, x, y, z;
    args[0].mQuat.getAngleAxis(&angle_radians, &x, &y, &z);
    retval->mVec = LLVector3(x, y, z);
    retval->mVec.normalize();
}

//TODO: Test me
void llRot2Angle(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    F32 angle_radians, x, y, z;
    args[0].mQuat.getAngleAxis(&angle_radians, &x, &y, &z);
    retval->mFP = angle_radians;
}

//TODO: Test me
void llAcos(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = acos(args[0].mFP);
}

//TODO: Test me
void llAsin(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = asin(args[0].mFP);
}

void llAngleBetween(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;

    LLQuaternion quatA(args[0].mQuat);
    LLQuaternion quatB(args[1].mQuat);

    quatA.normalize();
    quatB.normalize();

    float dotProduct = dot(quatA, quatB);

    retval->mFP = std::acos(2 * dotProduct * dotProduct - 1);
}

void llSubStringIndex(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    retval->mInteger = -1;

    LLWString source = utf8str_to_wstring(args[0].mString);
    LLWString pattern = utf8str_to_wstring(args[1].mString);

    size_t found = source.find(pattern);
    if (found != LLWString::npos)
        retval->mInteger = static_cast<int>(found);
}

//void llListSort(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)

void llGetListLength(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    retval->mInteger = args[0].getListLength();
}

void llList2Integer(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    int listSize = args[0].getListLength();
    int pos = args[1].mInteger;
    if (pos < 0)
        pos = listSize + pos;

    if (pos >= 0 && pos < listSize)
    {
        LLScriptLibData *current = &args[0];
        while (pos-- >= 0)
            current = current->mListp;

        retval->mInteger = current->mInteger;
    }
}

void llList2Float(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    int listSize = args[0].getListLength();
    int pos = args[1].mInteger;
    if (pos < 0)
        pos = listSize + pos;

    if (pos >= 0 && pos < listSize)
    {
        LLScriptLibData *current = &args[0];
        while (pos-- >= 0)
            current = current->mListp;

        retval->mFP = current->mFP;
    }
}

void llList2String(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;
    int listSize = args[0].getListLength();
    int pos = args[1].mInteger;
    if (pos < 0)
        pos = listSize + pos;

    if (pos >= 0 && pos < listSize)
    {
        LLScriptLibData *current = &args[0];
        while (pos-- >= 0)
            current = current->mListp;

        retval->mString = strdup(current->mString);
    }
}

void llList2Key(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_KEY;
    int listSize = args[0].getListLength();
    int pos = args[1].mInteger;
    if (pos < 0)
        pos = listSize + pos;

    if (pos >= 0 && pos < listSize)
    {
        LLScriptLibData *current = &args[0];
        while (pos-- >= 0)
            current = current->mListp;

        retval->mKey = strdup(current->mKey);
    }
}

void llList2Vector(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_VECTOR;
    int listSize = args[0].getListLength();
    int pos = args[1].mInteger;
    if (pos < 0)
        pos = listSize + pos;

    if (pos >= 0 && pos < listSize)
    {
        LLScriptLibData *current = &args[0];
        while (pos-- >= 0)
            current = current->mListp;

        retval->mVec = current->mVec;
    }
}

void llList2Rot(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_QUATERNION;
    int listSize = args[0].getListLength();
    int pos = args[1].mInteger;
    if (pos < 0)
        pos = listSize + pos;

    if (pos >= 0 && pos < listSize)
    {
        LLScriptLibData *current = &args[0];
        while (pos-- >= 0)
            current = current->mListp;

        retval->mQuat = current->mQuat;
    }
}

void llList2List(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_LIST;
    int listSize = args[0].getListLength();
    int start = args[1].mInteger;
    int end = args[2].mInteger;

    // Handle negative indices
    if (start < 0) start += listSize;
    if (end < 0) end += listSize;

    // Ensure start and end are within bounds
    if (start < 0) start = 0;
    if (end >= listSize) end = listSize - 1;

    LLScriptLibData *dest = retval;
    LLScriptLibData *source = args[0].mListp;

    // Determine the behavior based on the comparison of start and end
    if (start <= end)
    {
        // Adjust source to the start position
        for (int offset = 0; offset < start; ++offset)
        {
            source = source->mListp;
        }

        // Copy elements from start to end
        for (int offset = start; offset <= end; ++offset)
        {
            dest->mListp = new LLScriptLibData(*source);
            dest = dest->mListp;
            source = source->mListp;
        }
    }
    else
    {
        // Exclusion case: create a list excluding the range from end to start
        LLScriptLibData *tempSource = args[0].mListp;

        for (int offset = 0; offset <= end; ++offset)
        {
            dest->mListp = new LLScriptLibData(*tempSource);
            dest = dest->mListp;
            tempSource = tempSource->mListp;
        }

        tempSource = args[0].mListp;
        for (int offset = 0; offset < start; ++offset)
        {
            tempSource = tempSource->mListp;
        }

        for (int offset = start; offset < listSize; ++offset)
        {
            dest->mListp = new LLScriptLibData(*tempSource);
            dest = dest->mListp;
            tempSource = tempSource->mListp;
        }
    }

    dest->mListp = nullptr;
}
//void llDeleteSubList(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)

void llGetListEntryType(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    int listSize = args[0].getListLength();
    int pos = args[1].mInteger;
    if (pos < 0)
        pos = listSize + pos;

    if (pos >= 0 && pos < listSize)
    {
        LLScriptLibData *current = &args[0];
        while (pos-- >= 0)
            current = current->mListp;

        retval->mInteger = current->mType;
    }
}

//void llList2CSV(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
//void llCSV2List(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)

void llListRandomize(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_LIST;
    retval->mListp = lsa_randomize(&args[0], args[1].mInteger);
}

//void llList2ListStrided(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
//void llListInsertList(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
//void llListFindList(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)

void llGetDate(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;
    time_t current_time = time_corrected();
    struct tm* time_info = gmtime(&current_time);

    retval->mString = new char[11];
    strftime(retval->mString, 11, "%Y-%m-%d", time_info);
}

//void llParseString2List(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
//void llDumpList2String(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)

void llMD5String(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;

    char nonce_str[16]; // Could get away with 11, but 16 should be enough for now
    std::sprintf(nonce_str, "%d", args[1].mInteger);

    // Calculate length of the resulting string
    size_t src_len = std::strlen(args[0].mString);
    size_t nonce_len = std::strlen(nonce_str);
    size_t total_len = src_len + nonce_len + 1;

    char* combined_str = new char[total_len + 1];
    std::strcpy(combined_str, args[0].mString);
    std::strcat(combined_str, ":");
    std::strcat(combined_str, nonce_str);

    LLMD5 hash(reinterpret_cast<const unsigned char*>(combined_str));
    delete[] combined_str;

    retval->mString = new char[33];
    hash.hex_digest(retval->mString);
}

void llStringToBase64(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;
    retval->mString = strdup(LLBase64::encode(reinterpret_cast<const U8*>(args[0].mString), std::strlen(args[0].mString)).c_str());
}

void llBase64ToString(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;
    std::string decoded = LLBase64::decodeAsString(args[0].mString);
    decoded.erase(std::remove(decoded.begin(), decoded.end(), '\0'), decoded.end());
    retval->mString = strdup(decoded.c_str());
}

void llXorBase64Strings(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;
    std::string source_a = args[0].mString;
    std::string source_b = args[1].mString;

    size_t len_a = source_a.size();
    size_t len_b = source_b.size();

    if (len_b == 0)
    {
        retval->mString = strdup(source_a.c_str());
        return;
    }

    const std::string Base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    // Determine the "base" value based on the first character of source_b
    int fallback = Base64Table.find(source_b[0]);
    if (fallback == std::string::npos)
    {
        if (source_b[0] == '=')
        {
            source_b[0] = '+';
            fallback = 62;
        }
        else
        {
            source_b[0] = '/';
            fallback = 63;
        }
    }

    std::string result;
    result.reserve(len_a); // Preallocate space

    size_t xor_index = 0;

    for (size_t i = 0; i < len_a; ++i)
    {
        int index_a = Base64Table.find(source_a[i]);

        xor_index = (xor_index + 1) % len_b;  // Cycle XOR key

        if (index_a == std::string::npos)
        {
            result.push_back('=');  // Invalid characters become '='
        }
        else
        {
            int index_b = Base64Table.find(source_b[xor_index]);
            if (index_b == std::string::npos)
            {
                index_b = fallback;
                xor_index = 1; // Bug: force reset of xor_index
            }
            result.push_back(Base64Table[index_a ^ index_b]);
        }
    }

    retval->mString = strdup(result.c_str());
}

void llLog10(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = log10(args[0].mFP);
}

void llLog(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    retval->mFP = log(args[0].mFP);
}

void llGetTimestamp(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::gmtime(&now_time_t);
    long int micros = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count() % 1000000;
    char timestamp_str[29];
    std::strftime(timestamp_str, sizeof(timestamp_str), "%Y-%m-%dT%H:%M:%S", now_tm);
    std::sprintf(timestamp_str + 19, ".%06ldZ", micros);
    retval->mString = strdup(timestamp_str);
}

void llIntegerToBase64(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;

    int value = args[0].mInteger;
    std::string binary(4, '\0');
    binary[0] = (value >> 24) & 0xFF;
    binary[1] = (value >> 16) & 0xFF;
    binary[2] = (value >> 8) & 0xFF;
    binary[3] = value & 0xFF;

    std::string encoded = LLBase64::encode(reinterpret_cast<const U8*>(binary.data()), binary.size());
    retval->mString = strdup(encoded.c_str());
}

void llBase64ToInteger(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    std::string decoded = LLBase64::decodeAsString(args[0].mString);

    if (decoded.size() < 4)
    {
        retval->mInteger = 0;
        return;
    }

    int value = (static_cast<unsigned char>(decoded[0]) << 24) |
                (static_cast<unsigned char>(decoded[1]) << 16) |
                (static_cast<unsigned char>(decoded[2]) << 8)  |
                (static_cast<unsigned char>(decoded[3]));

    retval->mInteger = value;
}

void llGetGMTclock(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_FLOATINGPOINT;
    const time_t current_time = time_corrected();
    struct tm* time_info = gmtime(&current_time);

    retval->mFP = time_info->tm_hour * 3600 + time_info->tm_min * 60 + time_info->tm_sec;
}

//void llParseStringKeepNulls(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
//void llListReplaceList(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)

void llModPow(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;

    std::uint64_t base = static_cast<std::uint64_t>(args[0].mInteger);
    int exponent = args[1].mInteger;
    std::uint64_t modulus = static_cast<std::uint64_t>(args[2].mInteger);

    if (modulus == 0)
    {
        retval->mInteger = 0;
        return;
    }

    std::uint64_t result = 1; // Initialize result as 1, using uint64_t to handle large numbers

    // Compute base^exponent % modulus using iterative exponentiation by squaring
    while (exponent > 0)
    {
        // If exponent is odd, multiply base with result
        if (exponent % 2 == 1)
            result = (result * base) % modulus;

        // Now exponent must be even
        exponent = exponent >> 1; // Divide exponent by 2
        base = (base * base) % modulus; // Change base to base^2
    }

    retval->mInteger = static_cast<int>(result);
}

void llEscapeURL(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id) {
    retval->mType = LST_STRING;
    std::string escaped = LLURI::escape(args->mString);
    retval->mString = strdup(escaped.c_str());
}

void llUnescapeURL(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id) {
    retval->mType = LST_STRING;
    std::string unescaped = LLURI::unescape(args->mString);
    retval->mString = strdup(unescaped.c_str());
}

//void llListStatistics(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)

void llGetUnixTime(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    const time_t current_time = time_corrected();
    retval->mInteger = static_cast<int>(current_time);
}

void llXorBase64StringsCorrect(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;
    std::string source_a = LLBase64::decodeAsString(args[0].mString);
    std::string source_b = LLBase64::decodeAsString(args[1].mString);

    size_t len_a = source_a.size();
    size_t len_b = source_b.size();

    // This is to mimic the bugged behavior of llXorBase64StringsCorrect
    size_t null_index = source_b.find('\0');
    if (null_index != std::string::npos)
        len_b = null_index;

    std::string result;
    result.reserve(len_a); // Preallocate space
    if (len_a > 0 && len_b == 0)
    {
        result = source_a;
    }
    else
    {
        // XOR each byte, repeating the shorter string if necessary
        for (size_t i = 0; i < len_a; ++i)
        {
            result.push_back(source_a[i] ^ source_b[i % len_b]);
        }
    }

    std::string encoded = LLBase64::encode(reinterpret_cast<const U8*>(result.data()), result.size());
    retval->mString = strdup(encoded.c_str());
}

//void llStringTrim(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)

void llSHA1String(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;

    size_t src_len = std::strlen(args[0].mString);
    retval->mString = new char[SHA_DIGEST_LENGTH * 2 + 1];
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(args[0].mString), src_len, hash);

    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
        std::sprintf(retval->mString + i * 2, "%02x", hash[i]);
}

// void llGetSPMaxMemory(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llGetUsedMemory(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llScriptProfiler(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llSetMemoryLimit(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llGetMemoryLimit(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
void llGenerateKey(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_KEY;
    LLUUID newKey;
    newKey.generateNewID();
    retval->mKey = strdup(newKey.asString().c_str());
}

// void llJsonSetValue(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llJsonGetValue(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llJsonValueType(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llJson2List(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llList2Json(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)

void llXorBase64(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;
    std::string source_a = LLBase64::decodeAsString(args[0].mString);
    std::string source_b = LLBase64::decodeAsString(args[1].mString);

    size_t len_a = source_a.size();
    size_t len_b = source_b.size();

    std::string result;
    result.reserve(len_a); // Preallocate space

    if (len_a > 0 && len_b == 0)
    {
        result = source_a;
    }
    else
    {
        for (size_t i = 0; i < len_a; ++i)
        {
            result.push_back(source_a[i] ^ source_b[i % len_b]);
        }
    }

    std::string encoded = LLBase64::encode(reinterpret_cast<const U8*>(result.data()), result.size());
    retval->mString = strdup(encoded.c_str());
}

void llChar(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;
    int codepoint = args[0].mInteger;
    if (codepoint <= 0 || codepoint > 0x10FFFF)
    {
        retval->mString = strdup("");
        return;
    }
    LLWString wstr(1, static_cast<llwchar>(codepoint));
    retval->mString = strdup(wstring_to_utf8str(wstr).c_str());
}

void llOrd(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    retval->mInteger = 0;
    LLWString wstr = utf8str_to_wstring(args[0].mString);
    int strLen = static_cast<int>(wstr.size());
    int index = args[1].mInteger;
    if (index < 0) index += strLen;
    if (index >= 0 && index < strLen)
        retval->mInteger = static_cast<int>(wstr[index]);
}
void llHash(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_INTEGER;
    LLWString wstr = utf8str_to_wstring(args[0].mString);
    U32 hash = 0;
    for (const llwchar &c : wstr)
        hash = c + (hash << 6) + (hash << 16) - hash;
    retval->mInteger = static_cast<S32>(hash);
}

void llSHA256String(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;

    size_t src_len = std::strlen(args[0].mString);
    retval->mString = new char[SHA256_DIGEST_LENGTH * 2 + 1];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(args[0].mString), src_len, hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        std::sprintf(retval->mString + i * 2, "%02x", hash[i]);
}

void llHMAC(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;

    const char* key     = args[0].mString;
    const char* message = args[1].mString;
    std::string algo    = args[2].mString;
    std::transform(algo.begin(), algo.end(), algo.begin(), ::tolower);

    const EVP_MD* evp_md = nullptr;
    if      (algo == "md5")    evp_md = EVP_md5();
    else if (algo == "sha1")   evp_md = EVP_sha1();
    else if (algo == "sha224") evp_md = EVP_sha224();
    else if (algo == "sha256") evp_md = EVP_sha256();
    else if (algo == "sha384") evp_md = EVP_sha384();
    else if (algo == "sha512") evp_md = EVP_sha512();

    if (!evp_md)
    {
        retval->mString = strdup("");
        return;
    }

    unsigned char hmac[EVP_MAX_MD_SIZE];
    unsigned int hmac_len = 0;
    HMAC(evp_md,
         reinterpret_cast<const unsigned char*>(key), std::strlen(key),
         reinterpret_cast<const unsigned char*>(message), std::strlen(message),
         hmac, &hmac_len);

    retval->mString = new char[hmac_len * 2 + 1];
    for (unsigned int i = 0; i < hmac_len; ++i)
        std::sprintf(retval->mString + i * 2, "%02x", hmac[i]);
    retval->mString[hmac_len * 2] = '\0';
}

// void llSignRSA(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llVerifyRSA(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llReplaceSubString(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)

void llComputeHash(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_STRING;

    const char* message = args[0].mString;
    size_t msg_len = std::strlen(message);
    std::string algo = args[1].mString;
    std::transform(algo.begin(), algo.end(), algo.begin(), ::tolower);

    // md5_sha1 is a special concatenation of both digests
    if (algo == "md5_sha1")
    {
        unsigned char md5_hash[MD5_DIGEST_LENGTH];
        unsigned char sha1_hash[SHA_DIGEST_LENGTH];
        MD5(reinterpret_cast<const unsigned char*>(message), msg_len, md5_hash);
        SHA1(reinterpret_cast<const unsigned char*>(message), msg_len, sha1_hash);

        constexpr int total = MD5_DIGEST_LENGTH + SHA_DIGEST_LENGTH;
        retval->mString = new char[total * 2 + 1];
        for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
            std::sprintf(retval->mString + i * 2, "%02x", md5_hash[i]);
        for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
            std::sprintf(retval->mString + (MD5_DIGEST_LENGTH + i) * 2, "%02x", sha1_hash[i]);
        retval->mString[total * 2] = '\0';
        return;
    }

    const EVP_MD* evp_md = nullptr;
    if      (algo == "md5")    evp_md = EVP_md5();
    else if (algo == "sha1")   evp_md = EVP_sha1();
    else if (algo == "sha224") evp_md = EVP_sha224();
    else if (algo == "sha256") evp_md = EVP_sha256();
    else if (algo == "sha384") evp_md = EVP_sha384();
    else if (algo == "sha512") evp_md = EVP_sha512();

    if (!evp_md)
    {
        retval->mString = strdup("");
        return;
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx)
    {
        retval->mString = strdup("");
        return;
    }
    EVP_DigestInit_ex(ctx, evp_md, nullptr);
    EVP_DigestUpdate(ctx, message, msg_len);
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);

    retval->mString = new char[hash_len * 2 + 1];
    for (unsigned int i = 0; i < hash_len; ++i)
        std::sprintf(retval->mString + i * 2, "%02x", hash[i]);
    retval->mString[hash_len * 2] = '\0';
}

// void llGetStartString(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)

void llLinear2sRGB(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_VECTOR;
    for (int i = 0; i < 3; ++i)
    {
        F32 c = llclampf(args[0].mVec.mV[i]);
        if (c <= 0.0031308f)
            retval->mVec.mV[i] = 12.92f * c;
        else
            retval->mVec.mV[i] = 1.055f * powf(c, 1.0f / 2.4f) - 0.055f;
    }
}

void llsRGB2Linear(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
{
    retval->mType = LST_VECTOR;
    for (int i = 0; i < 3; ++i)
    {
        F32 c = llclampf(args[0].mVec.mV[i]);
        if (c <= 0.04045f)
            retval->mVec.mV[i] = c / 12.92f;
        else
            retval->mVec.mV[i] = powf((c + 0.055f) / 1.055f, 2.4f);
    }
}

// void llListFindStrided(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llList2ListSlice(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llListSortStrided(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)
// void llListFindListNext(LLScriptLibData *retval, LLScriptLibData *args, const LLUUID &id)


void LSLLibrary::init()
{
    mLibrary.assignExec("llSin", llSin);
    mLibrary.assignExec("llCos", llCos);
    mLibrary.assignExec("llTan", llTan);
    mLibrary.assignExec("llAtan2", llAtan2);
    mLibrary.assignExec("llSqrt", llSqrt);
    mLibrary.assignExec("llPow", llPow);
    mLibrary.assignExec("llAbs", llAbs);
    mLibrary.assignExec("llFabs", llFabs);
    mLibrary.assignExec("llFrand", llFrand);
    mLibrary.assignExec("llFloor", llFloor);
    mLibrary.assignExec("llCeil", llCeil);
    mLibrary.assignExec("llRound", llRound);
    mLibrary.assignExec("llVecMag", llVecMag);
    mLibrary.assignExec("llVecNorm", llVecNorm);
    mLibrary.assignExec("llVecDist", llVecDist);
    mLibrary.assignExec("llRot2Euler", llRot2Euler);
    mLibrary.assignExec("llEuler2Rot", llEuler2Rot);
    mLibrary.assignExec("llAxes2Rot", llAxes2Rot);
    mLibrary.assignExec("llRot2Fwd", llRot2Fwd);
    mLibrary.assignExec("llRot2Left", llRot2Left);
    mLibrary.assignExec("llRot2Up", llRot2Up);
    mLibrary.assignExec("llRotBetween", llRotBetween);

    mLibrary.assignExec("llGetWallclock", llGetWallclock);

    mLibrary.assignExec("llGetSubString", llGetSubString);
    mLibrary.assignExec("llDeleteSubString", llDeleteSubString);
    mLibrary.assignExec("llInsertString", llInsertString);

    mLibrary.assignExec("llToUpper", llToUpper);
    mLibrary.assignExec("llToLower", llToLower);
    mLibrary.assignExec("llStringLength", llStringLength);

    mLibrary.assignExec("llAxisAngle2Rot", llAxisAngle2Rot);
    mLibrary.assignExec("llRot2Axis", llRot2Axis);
    mLibrary.assignExec("llRot2Angle", llRot2Angle);
    mLibrary.assignExec("llAcos", llAcos);
    mLibrary.assignExec("llAsin", llAsin);

    mLibrary.assignExec("llAngleBetween", llAngleBetween);
    mLibrary.assignExec("llSubStringIndex", llSubStringIndex);
    //mLibrary.assignExec("llListSort", "l", "lii");

    mLibrary.assignExec("llGetListLength", llGetListLength);
    mLibrary.assignExec("llList2Integer", llList2Integer);
    mLibrary.assignExec("llList2Float", llList2Float);
    mLibrary.assignExec("llList2String", llList2String);
    mLibrary.assignExec("llList2Key", llList2Key);
    mLibrary.assignExec("llList2Vector", llList2Vector);
    mLibrary.assignExec("llList2Rot", llList2Rot);

    mLibrary.assignExec("llList2List", llList2List);

    //mLibrary.assignExec("llDeleteSubList", "l", "lii");
    mLibrary.assignExec("llGetListEntryType", llGetListEntryType);
    //mLibrary.assignExec("llList2CSV", "s", "l");
    //mLibrary.assignExec("llCSV2List", "l", "s");
    mLibrary.assignExec("llListRandomize", llListRandomize);
    //mLibrary.assignExec("llList2ListStrided", "l", "liii");

    //mLibrary.assignExec("llListInsertList", "l", "lli");
    //mLibrary.assignExec("llListFindList", "i", "ll");

    mLibrary.assignExec("llGetDate", llGetDate);

    //mLibrary.assignExec("llParseString2List", "l", "sll");

    //mLibrary.assignExec("llDumpList2String", "s", "ls");

    mLibrary.assignExec("llMD5String", llMD5String);

    mLibrary.assignExec("llStringToBase64", llStringToBase64);
    mLibrary.assignExec("llBase64ToString", llBase64ToString);
    mLibrary.assignExec("llXorBase64Strings", llXorBase64Strings);

    mLibrary.assignExec("llLog10", llLog10);
    mLibrary.assignExec("llLog", llLog);

    mLibrary.assignExec("llGetTimestamp", llGetTimestamp);

    mLibrary.assignExec("llIntegerToBase64", llIntegerToBase64);
    mLibrary.assignExec("llBase64ToInteger", llBase64ToInteger);
    mLibrary.assignExec("llGetGMTclock", llGetGMTclock);

    //mLibrary.assignExec("llParseStringKeepNulls", "l", "sll");

    //mLibrary.assignExec("llListReplaceList", "l", "llii");

    mLibrary.assignExec("llModPow", llModPow);

    mLibrary.assignExec("llEscapeURL", llEscapeURL);
    mLibrary.assignExec("llUnescapeURL", llUnescapeURL);

    //mLibrary.assignExec("llListStatistics", "f", "il");
    mLibrary.assignExec("llGetUnixTime", llGetUnixTime);

    mLibrary.assignExec("llXorBase64StringsCorrect", llXorBase64StringsCorrect);

    //mLibrary.assignExec("llStringTrim", "s", "si");

    mLibrary.assignExec("llSHA1String", llSHA1String);

    // mLibrary.assignExec("llGetSPMaxMemory", llGetSPMaxMemory);
    // mLibrary.assignExec("llGetUsedMemory", llGetUsedMemory);
    // mLibrary.assignExec("llScriptProfiler", llScriptProfiler);
    // mLibrary.assignExec("llSetMemoryLimit", llSetMemoryLimit);
    // mLibrary.assignExec("llGetMemoryLimit", llGetMemoryLimit);
    mLibrary.assignExec("llGenerateKey", llGenerateKey);
    // mLibrary.assignExec("llJsonSetValue", llJsonSetValue);
    // mLibrary.assignExec("llJsonGetValue", llJsonGetValue);
    // mLibrary.assignExec("llJsonValueType", llJsonValueType);
    // mLibrary.assignExec("llJson2List", llJson2List);
    // mLibrary.assignExec("llList2Json", llList2Json);
    mLibrary.assignExec("llXorBase64", llXorBase64);
    mLibrary.assignExec("llChar", llChar);
    mLibrary.assignExec("llOrd", llOrd);
    mLibrary.assignExec("llHash", llHash);
    mLibrary.assignExec("llSHA256String", llSHA256String);
    mLibrary.assignExec("llHMAC", llHMAC);
    // mLibrary.assignExec("llSignRSA", llSignRSA);
    // mLibrary.assignExec("llVerifyRSA", llVerifyRSA);
    // mLibrary.assignExec("llReplaceSubString", llReplaceSubString);
    mLibrary.assignExec("llComputeHash", llComputeHash);
    // mLibrary.assignExec("llGetStartString", llGetStartString);
    mLibrary.assignExec("llLinear2sRGB", llLinear2sRGB);
    mLibrary.assignExec("llsRGB2Linear", llsRGB2Linear);
    // mLibrary.assignExec("llListFindStrided", llListFindStrided);
    // mLibrary.assignExec("llList2ListSlice", llList2ListSlice);
    // mLibrary.assignExec("llListSortStrided", llListSortStrided);
    // mLibrary.assignExec("llListFindListNext", llListFindListNext);
}
