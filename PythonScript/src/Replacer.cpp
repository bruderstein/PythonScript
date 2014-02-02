#include "stdafx.h"

#ifndef REPLACER_H_DGB_20131120
#define REPLACER_H_DGB_20131120


#include "Replacer.h"
#include "ReplaceEntry.h"
#include "UTF8Iterator.h"
#include "ConstString.h"
#include "UtfConversion.h"
#include "Match.h"

namespace NppPythonScript
{

using UtfConversion::toStringType;

typedef boost::basic_regex<U32, u32_regex_traits> u32_regex;
typedef boost::regex_iterator<UTF8Iterator, U32, u32_regex_traits> u32_regex_iterator;



}
#endif 
