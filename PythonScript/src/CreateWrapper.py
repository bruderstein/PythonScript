# CreateWrapper.py - regenerate the ScintillaWrapper.h and ScintillaWrapper.cpp
# files from the Scintilla.iface interface definition file.

# Todo:

# GetViewWS
# SetSearchFlags  - need flags type in list or something
# GetCharAt

# AutoCSetSeparator
# UserListShow
# AutoCSetTypeSeparator - ignore - cannot change this

# FindText
# GetSelText - stringresult

# SetWrapMode - modes
# SetLengthForEncode, TargetAsUTF8 - these want wrapping up, and excluding
# CreateDocument etc, move to advanced?
# Return PyNone in FindText

#  DONE - excluded Type: formatrange
#  DONE Type: textrange
# DONE GetStyledText needs exception -currently DANGEROUS - needs twice as many bytes as currently
# Keymod type

import os
import Face

types = {
	'string'	: 'boost::python::object',
	'position'	: 'Sci_Position',
	'line'		: 'intptr_t',
	'cells'		: 'ScintillaCells',
	'pointer'	: 'intptr_t',
	'colour'	: 'boost::python::tuple',
	'colouralpha'	: 'boost::python::tuple',
	'keymod'	: 'int',  # Temporary hack - need this to be a real type
	#replace all enums by ints like before 4.x scintilla iface, todo check how to distinguish between real enums and flag like usage
	'WhiteSpace'	: 'int',
	'TabDrawMode'	: 'int',
	'EndOfLine'	: 'int',
	'IMEInteraction'	: 'int',
	'MarkerSymbol'	: 'int',
	'MarkerOutline'	: 'int',
	'MarginType'	: 'int',
	'StylesCommon'	: 'int',
	'CharacterSet'	: 'int',
	'CaseVisible'	: 'int',
	'FontWeight'	: 'int',
	'IndicatorStyle'	: 'int',
	'IndicatorNumbers'	: 'int',
	'IndicValue'	: 'int',
	'IndicFlag'	: 'int',
	'IndentView'	: 'int',
	'PrintOption'	: 'int',
	'FindOption'	: 'int',
	'FoldLevel'	: 'int',
	'FoldDisplayTextStyle'	: 'int',
	'FoldAction'	: 'int',
	'AutomaticFold'	: 'int',
	'FoldFlag'	: 'int',
	'IdleStyling'	: 'int',
	'Wrap'	: 'int',
	'WrapVisualFlag'	: 'int',
	'WrapVisualLocation'	: 'int',
	'WrapIndentMode'	: 'int',
	'LineCache'	: 'int',
	'PhasesDraw'	: 'int',
	'FontQuality'	: 'int',
	'MultiPaste'	: 'int',
	'Accessibility'	: 'int',
	'EdgeVisualStyle'	: 'int',
	'PopUp'	: 'int',
	'DocumentOption'	: 'int',
	'Status'	: 'int',
	'CursorShape'	: 'int',
	'VisiblePolicy'	: 'int',
	'CaretPolicy'	: 'int',
	'SelectionMode'	: 'int',
	'CaseInsensitiveBehaviour'	: 'int',
	'MultiAutoComplete'	: 'int',
	'Ordering'	: 'int',
	'CaretSticky'	: 'int',
	'Alpha'	: 'int',
	'CaretStyle'	: 'int',
	'MarginOption'	: 'int',
	'AnnotationVisible'	: 'int',
	'UndoFlags'	: 'int',
	'VirtualSpace'	: 'int',
	'Technology'	: 'int',
	'LineEndType'	: 'int',
	'TypeProperty'	: 'int',
	'ModificationFlags'	: 'int',
	'Update'	: 'int',
	'Keys'	: 'int',
	'KeyMod'	: 'int',
	'CompletionMethods'	: 'int',
	'CharacterSource'	: 'int',
	'Lexer'	: 'int',
	'Bidirectional'	: 'int',
	'LineCharacterIndexType'	: 'int',
	'EOLAnnotationVisible'	: 'int',
	'Layer'	: 'int',
	'AutoCompleteOption'	: 'int',
	'RepresentationAppearance'	: 'int',
	'Supports'	: 'int',
	'Element'	: 'int',
	'ChangeHistoryOption'	: 'int',
}

castsL = {
	'boost::python::object'	: "reinterpret_cast<LPARAM>(string{}.c_str())",
	# Hack - assume a tuple is a colour
	'boost::python::tuple': "static_cast<LPARAM>(rgb{})"
}

castsW = {
	'boost::python::object'	: "reinterpret_cast<WPARAM>(string{}.c_str())",
	# Hack - assume a tuple is a colour
	'boost::python::tuple': "static_cast<WPARAM>(rgb{})"
}

castsRet = {
	'bool' : 'return 0 != ({})',
	'boost::python::tuple': 'int retVal = (int){};\n\treturn boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal))'
}

# Must be kept in sync with pythonTypeExplosions
typeExplosions = {
	#'colour'    : lambda name: 'int {0}Red, int {0}Green, int {0}Blue'.format(name),
	'findtext' : 'Sci_PositionCR start, Sci_PositionCR end, boost::python::object {}',
	'findtextfull' : 'Sci_Position start, Sci_Position end, boost::python::object {}',
	'textrange' : 'Sci_PositionCR start, Sci_PositionCR end',
	'textrangefull' : 'Sci_Position start, Sci_Position end'
}

# Must be kept in sync with typeExplosions
pythonTypeExplosions = {
	#'colour'    : lambda name: 'int {0}Red, int {0}Green, int {0}Blue'.format(name),
	'findtext' : 'start, end, {}',
	'findtextfull' : 'start, end, {}',
	'textrange' : 'start, end',
	'textrangefull' : 'start, end'
}

withGilConversions = {
	'boost::python::object'	: '\tstd::string string{0} = getStringFromObject({0});\n',
	'boost::python::tuple' : '\tCOLORREF rgb{0} = MAKECOLOUR({0});\n',
}

disallowedInCallback = {
	'SearchInTarget' : 'Use an asynchronous callback or one of the editor.search(), editor.research(), editor.replace(), editor.rereplace() methods.',
	'FindText' : 'Use an asynchronous callback or one of the editor.search(), editor.research(), editor.replace(), editor.rereplace() methods.',
	'SetDocPointer' : 'Use an asynchronous callback, or avoid using SetDocPointer in the callback.',

}

exclusions = [ 'FormatRange', 'FormatRangeFull']


def symbolName(v):
	return "SCI_" + v["Name"].upper()


def castLparam(ty, name):
	return castsL.get(ty, name).format(name)


def castWparam(ty, name):
	return castsW.get(ty, name).format(name)


def castReturn(ty, val):
	return '{}'.format(castsRet.get(ty, 'return {}'.format(val)).format(val))


def withGilParam(out, type, name):
	out.write(withGilConversions.get(type, '').format(name))


def checkDisallowedInCallback(v, out):
	disallowedMessage = disallowedInCallback.get(v['Name'], None)
	if disallowedMessage:
		out.write('''	notAllowedInCallback("{0} is not allowed in a synchronous callback. {1}");\n'''.format(formatPythonName(v['Name']), disallowedMessage))


def traceCall(v, out):
	out.write('''	DEBUG_TRACE(L"ScintillaWrapper::{}\\n");\n'''.format(v['Name']))


def cellsBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write('''	return callScintilla({0}, {1}.length(), reinterpret_cast<LPARAM>({1}.cells()));\n'''.format(symbolName(v),v["Param2Name"]))


def constString(v, out):
	#out.write("\tconst char *raw = boost::python::extract<const char *>(" + v["Param2Name"] + ".attr(\"__str__\")());\n")
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	std::string s = getStringFromObject({0});
	return callScintilla({1}, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
'''.format(v["Param2Name"], symbolName(v)))


def retString(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	PythonCompatibleStrBuffer result(callScintilla({0}) + 1);
	// result.size() does not depend on the order of evaluation here
	//lint -e{{864}}
	callScintilla({0}, result.size(), reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
'''.format(symbolName(v)))


def getLineBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	intptr_t lineCount = callScintilla(SCI_GETLINECOUNT);
	if (line >= lineCount)
	{{
		throw out_of_bounds_exception();
	}}
	else
	{{
		PythonCompatibleStrBuffer result(callScintilla(SCI_LINELENGTH, line));
		callScintilla({0}, line, reinterpret_cast<LPARAM>(*result));
		return boost::python::str(result.c_str());
	}}
'''.format(symbolName(v)))


def retStringNoLength(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	parameters = ''
	if v["Param1Type"] != '' or v["Param2Type"] != '':
		parameters = ", "
		if v["Param1Type"] != '':
			parameters += v["Param1Name"]

		if v["Param2Type"] != '':
			parameters += v["Param2Name"]

	out.write(
'''	PythonCompatibleStrBuffer result(callScintilla({0}{1}));
	callScintilla({0}{2}, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
'''.format(symbolName(v), parameters, ', 0' if parameters == '' else parameters))


def retStringFromKey(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	std::string keyString = getStringFromObject({0});
	PythonCompatibleStrBuffer result(callScintilla({1}, reinterpret_cast<WPARAM>(keyString.c_str()), 0));
	callScintilla({1}, reinterpret_cast<WPARAM>(keyString.c_str()), reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
'''.format(v["Param1Name"], symbolName(v)))


def retStringFromEnc(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	std::string encString = getStringFromObject({0});
	PythonCompatibleStrBuffer result(callScintilla({1}, reinterpret_cast<WPARAM>(encString.c_str()), 0));
	callScintilla({1}, reinterpret_cast<WPARAM>(encString.c_str()), reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
'''.format(v["Param1Name"], symbolName(v)))


def retStringFromName(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	std::string nameString = getStringFromObject({0});
	PythonCompatibleStrBuffer result(callScintilla({1}, reinterpret_cast<WPARAM>(nameString.c_str()), 0));
	callScintilla({1}, reinterpret_cast<WPARAM>(nameString.c_str()), reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
'''.format(v["Param1Name"], symbolName(v)))


def retStringFromUTF8(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	std::string utf8String = getStringFromObject({0});
	PythonCompatibleStrBuffer result(callScintilla({1}, reinterpret_cast<WPARAM>(utf8String.c_str()), 0));
	callScintilla({1}, reinterpret_cast<WPARAM>(utf8String.c_str()), reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
'''.format(v["Param1Name"], symbolName(v)))


def findTextBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	std::string search = getStringFromObject({0});
	Sci_TextToFind src{{}};
	src.chrg.cpMin = start;
	src.chrg.cpMax = end;
	// We assume  findText won\'t write to this buffer - it should be const
	src.lpstrText = const_cast<char*>(search.c_str());
	intptr_t result = callScintilla({1}, {2}, reinterpret_cast<LPARAM>(&src));
	if (-1 == result)
	{{
		return boost::python::object();
	}}
	else
	{{
		return boost::python::make_tuple(src.chrgText.cpMin, src.chrgText.cpMax);
	}}
'''.format(v['Param2Name'], symbolName(v), v["Param1Name"]))

def findTextFullBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	std::string search = getStringFromObject({0});
	Sci_TextToFindFull src{{}};
	src.chrg.cpMin = start;
	src.chrg.cpMax = end;
	// We assume  findTextFull won\'t write to this buffer - it should be const
	src.lpstrText = const_cast<char*>(search.c_str());
	intptr_t result = callScintilla({1}, {2}, reinterpret_cast<LPARAM>(&src));
	if (-1 == result)
	{{
		return boost::python::object();
	}}
	else
	{{
		return boost::python::make_tuple(src.chrgText.cpMin, src.chrgText.cpMax);
	}}
'''.format(v['Param2Name'], symbolName(v), v["Param1Name"]))

def getTextRangeBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	Sci_TextRange src{{}};
	if (end == -1)
	{{
		end = GetLength();
	}}

	if (end < start)
	{{
		Sci_PositionCR temp = start;
		start = end;
		end = temp;
	}}
	PythonCompatibleStrBuffer result((end-start) + 1);
	src.chrg.cpMin = start;
	src.chrg.cpMax = end;
	src.lpstrText = *result;
	callScintilla({0}, 0, reinterpret_cast<LPARAM>(&src));
	return boost::python::str(result.c_str());
'''.format(symbolName(v)))

def getTextRangeFullBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	Sci_TextRangeFull src{{}};
	if (end == -1)
	{{
		end = GetLength();
	}}

	if (end < start)
	{{
		Sci_Position temp = start;
		start = end;
		end = temp;
	}}
	PythonCompatibleStrBuffer result((end-start) + 1);
	src.chrg.cpMin = start;
	src.chrg.cpMax = end;
	src.lpstrText = *result;
	callScintilla({0}, 0, reinterpret_cast<LPARAM>(&src));
	return boost::python::str(result.c_str());
'''.format(symbolName(v)))

def getStyledTextBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	Sci_TextRange src{{}};
	if (end < start)
	{{
		Sci_PositionCR temp = start;
		start = end;
		end = temp;
	}}
	src.chrg.cpMin = start;
	src.chrg.cpMax = end;
	src.lpstrText = new char[size_t(((end-start) * 2) + 2)];
	callScintilla({0}, 0, reinterpret_cast<LPARAM>(&src));
	boost::python::list styles;
	PythonCompatibleStrBuffer result(end-start);
	for(idx_t pos = 0; pos < result.size() - 1; pos++)
	{{
		(*result)[pos] = src.lpstrText[pos * 2];
		styles.append((int)(src.lpstrText[(pos * 2) + 1]));
	}}
	boost::python::str resultStr(result.c_str());
	delete [] src.lpstrText;
	return boost::python::make_tuple(resultStr, styles);
'''.format(symbolName(v)))


def getStyledTextFullBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	Sci_TextRangeFull src{{}};
	if (end < start)
	{{
		Sci_Position temp = start;
		start = end;
		end = temp;
	}}
	src.chrg.cpMin = start;
	src.chrg.cpMax = end;
	src.lpstrText = new char[size_t(((end-start) * 2) + 2)];
	callScintilla({0}, 0, reinterpret_cast<LPARAM>(&src));
	boost::python::list styles;
	PythonCompatibleStrBuffer result(end-start);
	for(idx_t pos = 0; pos < result.size() - 1; pos++)
	{{
		(*result)[pos] = src.lpstrText[pos * 2];
		styles.append((int)(src.lpstrText[(pos * 2) + 1]));
	}}
	boost::python::str resultStr(result.c_str());
	delete [] src.lpstrText;
	return boost::python::make_tuple(resultStr, styles);
'''.format(symbolName(v)))


def annotationSetTextBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	const char *newText;
	std::string s;
	if ({0}.is_none())
	{{
		newText = NULL;
	}}
	else
	{{
		s = getStringFromObject({1});
		newText = s.c_str();
	}}
	callScintilla({2}, static_cast<WPARAM>({3}), reinterpret_cast<LPARAM>(newText));
'''.format(v["Param2Name"], v["Param2Name"], symbolName(v), v["Param1Name"]))


def getSetDocPointerBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	callScintilla({0}, 0, {1});
'''.format(symbolName(v), v["Param2Name"]))


def getAddRefDocumentBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	callScintilla({0}, 0, {1});
'''.format(symbolName(v), v["Param2Name"]))


def getReleaseDocumentBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	callScintilla({0}, 0, {1});
'''.format(symbolName(v), v["Param2Name"]))


def getPrivateLexerCallBody(v, out):
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	out.write(
'''	return callScintilla({0}, {1}, {2});
'''.format(symbolName(v), v["Param1Name"], v["Param2Name"]))


def getGetRangePointerBody(v, out):
	out.write(
'''	GILRelease release;
	const char *charPtr = reinterpret_cast<const char*>(callScintilla({0}, {2}, {3}));
	release.reacquire();
	return {1}(charPtr, charPtr + {3});
'''.format(symbolName(v), v["ReturnType"], v["Param1Name"], v["Param2Name"]))


def getGetCharacterPointerBody(v, out):
	out.write(
'''	GILRelease release;
	const char *charPtr = reinterpret_cast<const char*>(callScintilla({0}));
	release.reacquire();
	return {1}(charPtr);
'''.format(symbolName(v), v["ReturnType"]))


def standardBody(v, out):
	# We always release the GIL.  For standard getters, this shouldn't really be necessary.
	# However, it doesn't appear to affect performance to dramatically (yet!), so we'll leave it in until
	# we need to optimise.
	traceCall(v, out)
	checkDisallowedInCallback(v, out)
	withGilParam(out, v['Param1Type'], v['Param1Name'])
	withGilParam(out, v['Param2Type'], v['Param2Name'])

	call = 'callScintilla({0}{1}{2}{3})'.format(symbolName(v),
												', 0' if v["Param2Type"] != '' and v["Param1Type"] == '' else '',
												', ' + castWparam(v["Param1Type"], v["Param1Name"]) if v["Param1Type"] else '',
												', ' + castLparam(v["Param2Type"], v["Param2Name"]) if v["Param2Type"] else '')

	out.write('\t{0};\n'.format(castReturn(v["ReturnType"], call) if (v["ReturnType"] != 'void') else call))


def mapType(t):
	return types.get(t, t)


def mapCompare(t, s):
	return True if (t == s or t == '' or (t is None and s == '')) else False


def mapSignature(s):
	for t in argumentMap:
		if mapCompare(t[0], s[0]) and mapCompare(t[1], s[1]) and mapCompare(t[2], s[2]) and mapCompare(t[3], s[3]):
			return t[4]
	return None


# Explodes a type to more parameters - e.g. colour
def explodeType(ty, name):
	return typeExplosions.get(ty, "{} {}".format(ty, name)).format(name)


def explodePythonType(ty, name):
	return pythonTypeExplosions.get(ty, name).format(name)


def returnParamString(param1Type, param1Name, param2Type, param2Name, func2call):
	retVal = ""
	if param1Type:
		retVal += func2call(param1Type, param1Name)
		if param2Type:
			retVal += ', '
	if param2Type:
		retVal += func2call(param2Type, param2Name)

	return retVal


def writeParams(param1Type, param1Name, param2Type, param2Name):
	return returnParamString(param1Type, param1Name, param2Type, param2Name, explodeType)


def writePythonParams(param1Type, param1Name, param2Type, param2Name):
	return returnParamString(param1Type, param1Name, param2Type, param2Name, explodePythonType)


def getPythonParamNamesQuoted(param1Type, param1Name, param2Type, param2Name):
	mappedSig = mapSignature((param1Type, param1Name, param2Type, param2Name))
	if mappedSig:
		param1Type = mappedSig[1]
		param2Type = mappedSig[2]

	pythonParams = writePythonParams(param1Type, param1Name, param2Type, param2Name)
	quotedParams = ", ".join(['"' + p.strip() + '"' for p in pythonParams.split(',')])
	return quotedParams


argumentMap = [
#	(firstParamType,	firstParamName,		secondParamType,	secondParamName):(returnType,				FirstParamType, 			SecondParamType, 		 	bodyFunction)
	('position',	'length',			'string',		'',			   ('intptr_t', 			'', 					'boost::python::object',constString)),
	('position',	'length',			'stringresult', '',			   ('boost::python::str', 	'', 					'', 					retString)),
	('string',		'key',				'stringresult', '',			   ('boost::python::str', 	'boost::python::object','', 					retStringFromKey)),
	('string',		'encodedCharacter',	'stringresult', '', 		   ('boost::python::str', 	'boost::python::object','', 					retStringFromEnc)),
	('string',		'name',				'stringresult', '', 		   ('boost::python::str', 	'boost::python::object','', 					retStringFromName)),
	('string',		'utf8',				'stringresult', '', 		   ('boost::python::str', 	'boost::python::object','', 					retStringFromUTF8)),
	('int',			'',					'stringresult',	'',			   ('boost::python::str', 	'int', 					'', 					retStringNoLength)),
	('position',	'',					'stringresult',	'',			   ('boost::python::str', 	'intptr_t', 			'', 					retStringNoLength)),
	('line',		'',					'stringresult',	'',			   ('boost::python::str', 	'intptr_t', 			'', 					retStringNoLength)),
	('',			'',					'stringresult',	'',			   ('boost::python::str', 	'', 					'', 					retStringNoLength)),
	('position',	'length',			'cells',		'',			   ('intptr_t', 			'', 					'ScintillaCells', 		cellsBody)),
	('FindOption',	'',					'findtext',		'ft',		   ('boost::python::object','int', 					'findtext', 			findTextBody)),
	('FindOption',	'',					'findtextfull',	'ft',		   ('boost::python::object','int', 					'findtextfull',			findTextFullBody)),
	('',			'',					'textrange', 	'tr',		   ('boost::python::str', 	'', 					'textrange',			getTextRangeBody)),
	('',			'',					'textrangefull','tr',		   ('boost::python::str', 	'', 					'textrangefull',		getTextRangeFullBody)),
]


specialCases = {
	'GetStyledText' : ('boost::python::tuple', 'Sci_PositionCR', 'start', 'Sci_PositionCR', 'end', getStyledTextBody),
	'GetStyledTextFull' : ('boost::python::tuple', 'Sci_Position', 'start', 'Sci_Position', 'end', getStyledTextFullBody),
	'GetLine': ('boost::python::str', 'int', 'line', '', '', getLineBody),
	'AnnotationSetText' : ('void', 'int', 'line', 'boost::python::object', 'text', annotationSetTextBody),
	'SetDocPointer' :('void', '','','intptr_t', 'pointer', getSetDocPointerBody),
	'AddRefDocument' :('void', '','', 'intptr_t', 'doc', getAddRefDocumentBody),
	'ReleaseDocument' :('void', '','', 'intptr_t', 'doc', getReleaseDocumentBody),
	'PrivateLexerCall' :('intptr_t', 'intptr_t','operation','intptr_t', 'pointer', getPrivateLexerCallBody),
	'GetCharacterPointer' :('boost::python::str', '','','', '', getGetCharacterPointerBody),
	'GetRangePointer' :('boost::python::str', 'int','position','int', 'rangeLength', getGetRangePointerBody)
}


def getSignature(v):
	return '{0} ScintillaWrapper::{1}({2})'.format(v["ReturnType"],
												   v["Name"],
												   writeParams(v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"]))


def formatPythonName(name):
	return name[0:1].lower() + name[1:]


def getPythonSignature(v):
	return "{0}({1}){2}".format(formatPythonName(v["Name"]),
								writePythonParams(v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"]),
								" -> " + v["ReturnType"].replace("boost::python::", "") if v["ReturnType"] and v["ReturnType"] != "void" else '')


def emptyIsVoid(var):
	return 'void' if var == '' else var


def writeCppFile(f,out):
	out.write(
'''#include "stdafx.h"
#include "ScintillaWrapper.h"
#include "Scintilla.h"
#include "GILManager.h"

namespace NppPythonScript
{
// Helper class
class PythonCompatibleStrBuffer
{
public:
	inline explicit PythonCompatibleStrBuffer(size_t length) :
		m_bufferLen(length + 1),
		m_bufferPtr(new char[m_bufferLen])
	{
		if (m_bufferPtr && m_bufferLen > 0) m_bufferPtr[m_bufferLen-1] = \'\\0\';
	}
	inline explicit PythonCompatibleStrBuffer(int length) :
		m_bufferLen(length >= 0 ? (size_t)(length+1) : 0),
		m_bufferPtr(new char[m_bufferLen])
	{
		if (m_bufferPtr && m_bufferLen > 0) m_bufferPtr[m_bufferLen-1] = \'\\0\';
	}
	inline explicit PythonCompatibleStrBuffer(LRESULT length) :
		m_bufferLen(length >= 0 ? (size_t)(length+1) : 0),
		m_bufferPtr(new char[m_bufferLen])
	{
		if (m_bufferPtr && m_bufferLen > 0) m_bufferPtr[m_bufferLen-1] = \'\\0\';
	}
	inline ~PythonCompatibleStrBuffer() { delete [] m_bufferPtr; }
	inline char* operator*() { return m_bufferPtr; }
	inline const char* c_str() const { return m_bufferPtr; }
	inline size_t size() const { return m_bufferLen; }
private:
	PythonCompatibleStrBuffer() = delete;  // default constructor disabled
	PythonCompatibleStrBuffer(const PythonCompatibleStrBuffer&) = delete; // copy constructor disabled
	PythonCompatibleStrBuffer& operator = (const PythonCompatibleStrBuffer&) = delete; // Disable assignment operator disabled
	size_t m_bufferLen;
	char* m_bufferPtr;
};

''')
	uniqueCombinations = {}

	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
			if v["FeatureType"] in ["fun", "get", "set"]:

				if v["Name"] in exclusions:
					continue
				if not uniqueCombinations.get((v["ReturnType"], v["Param1Type"], v["Param2Type"])):
					uniqueCombinations[(v["ReturnType"], v["Param1Type"], v["Param2Type"])] = []
				uniqueCombinations[(v["ReturnType"], v["Param1Type"], v["Param2Type"])].append(v["Name"])

				if v["Name"] in specialCases:
					(v["ReturnType"], v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"], body) = specialCases[v["Name"]]
				else:
					sig = mapSignature((v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"]))

					if sig is not None:
						v["ReturnType"] = 'intptr_t' if sig[0] in ['int', 'int pointer'] else sig[0]
						v["Param1Type"] = sig[1]
						v["Param2Type"] = sig[2]

						body = sig[3]
					else:
						#if !checkStandardTypeIsKnown(v["ReturnType", v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"]):
						#	print("Warning: unrecognised parameter combination for {0}({1} {2}, {3} {4})".format(v["Name"], v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"]))
						v["ReturnType"] = 'intptr_t' if v["ReturnType"] in ['int', 'position'] else mapType(v["ReturnType"])
						v["Param1Type"] = mapType(v["Param1Type"])
						v["Param2Type"] = mapType(v["Param2Type"])
						body = standardBody

				out.write("/** " + "\n  * ".join(v["Comment"]) + "\n  */\n")

				out.write(getSignature(v))
				out.write("\n{\n")
				body(v, out)
				out.write("}\n\n")

	out.write('}\n')

	# print "Unique combinations:"
	# for k in uniqueCombinations:
		# print str(k) + ' (%s)' % ", ".join(uniqueCombinations[k][:4])
"""
	for k in uniqueCombinations:
		comb = [c[0].lower() + c[1:] for c in uniqueCombinations[k][:4]]
		print '    def test_scintillawrapper_{0}_{1}_{2}(self):'.format(k[0], emptyIsVoid(k[1]), emptyIsVoid(k[2]))
		print '        pass'
		print '        # call one of the following: {}\n'.format(", ".join(comb[:4]))

		print '    def callback_scintillawrapper_{0}_{1}_{2}(self, args):'.format(k[0], emptyIsVoid(k[1]), emptyIsVoid(k[2]))
		print '        # call one of the following: {}'.format(", ".join(comb[:4]))
		print '        self.callbackCalled = True\n'

		print '    def test_scintillawrapper_{0}_{1}_{2}_in_callback(self):'.format(k[0], emptyIsVoid(k[1]), emptyIsVoid(k[2]))
		print '        editor.callback(lambda args: self.callback_scintillawrapper_{0}_{1}_{2}(args), [SCINTILLANOTIFICATION.MODIFIED])'.format(k[0], emptyIsVoid(k[1]), emptyIsVoid(k[2]))
		print '        editor.write("test");'
		print '        time.sleep(0.1)'
		print '        self.assertEqual(self.callbackCalled, True)\n'
	"""


def writeHFile(f,out):
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
			if v["FeatureType"] in ["fun", "get", "set"]:

				if v["Name"] in exclusions:
					continue

				sig = mapSignature((v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"]))

				if sig is not None:
					if sig[0] == '=':
						v["ReturnType"] = mapType(v["ReturnType"])
					else:
						v["ReturnType"] = sig[0]
					v["Param1Type"] = sig[1]
					v["Param2Type"] = sig[2]

				else:
					v["ReturnType"] = mapType(v["ReturnType"])
					v["Param1Type"] = mapType(v["Param1Type"])
					v["Param2Type"] = mapType(v["Param2Type"])

				out.write("\t/** " + "\n\t  * ".join(v["Comment"]) + "\n  */\n")

				out.write("\t")
				out.write(getSignature(v).replace(' ScintillaWrapper::', ' '))
				out.write(";\n\n")


def writeBoostWrapFile(f,out):
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
			if v["FeatureType"] in ["fun", "get", "set"]:

				if v["Name"] in exclusions:
					continue

				out.write('\t\t.def("{0}", &ScintillaWrapper::{1}, '.format(formatPythonName(v["Name"]), v["Name"]))
				# TODO: This is getting really nasty. We need to refactor this whole file.
				# We need to
				quotedParams = getPythonParamNamesQuoted(v['Param1Type'], v['Param1Name'], v['Param2Type'], v['Param2Name'])
				if quotedParams and quotedParams != '""':
					out.write("boost::python::args({0}), ".format(quotedParams))

				out.write('\"')
				out.write("\\n".join(v["Comment"]).replace('\\','\\\\').replace('"','\\"').replace('\\\\n', '\\n'))
				out.write('\")\n')


def writeEnumsHFile(f, out):
	for name in f.enums:
		v = f.enums[name]
		if v.get('Values'):
			out.write('enum {0}\n'.format(name))
			out.write('{\n\t')
			join = ""

			for val in v.get('Values'):
				out.write('{0}PYSCR_{1} = {1}'.format(join, val[0]))
				join = ',\n\t'
			out.write('\n};\n\n')
	out.write("""\n/* The following is the enum of events/notifications.
		   * Note that the PYSCN_XXX constants are NOT automatically generated (in ScintillaNotifications.h).
		   * This is very deliberate.
		   * An error here indicates that a new notification has been added,
		   * and hence handler code should be added to the ScintillaWrapper::notify() function
		   */\n""")

	join = ""
	out.write("enum ScintillaMessage\n")
	out.write("{")
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
			if v["FeatureType"] in ["fun", "get", "set"]:
				out.write("%s\n\tPYSCR_SCI_%s = SCI_%s" % (join, name.upper(), name.upper()))
				join = ","
	out.write("\n};\n")

	out.write("enum ScintillaNotification\n")
	out.write("{")

	join = ""
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":

			if v["FeatureType"] == "evt":
				out.write("%s\n\tPYSCR_SCN_%s = PYSCN_%s" % (join, name.upper(), name.upper()))
				join = ","
	out.write("\n};\n")


def writeEnumsWrapperFile(f, out):
	for name in f.enums:
		v = f.enums[name]
		if v.get('Values'):
			out.write('\tboost::python::enum_<{0}>("{1}")'.format(name, name.upper()))
			for val in v['Values']:
				takeEnumValueFromPosition = None
				for prefix in v['Value'].split(' '):
					if val[0][:len(prefix)] == prefix:
						takeEnumValueFromPosition = len(prefix)
						break

				# Hack for ModificationFlags
				# There's more than one prefix specified for ModificationFlags, separated with spaces.
				# Unfortunately, some of the prefixes are the complete symbol, which means we patch that here to just remove the SC_
				if val[0] in ['SC_STARTACTION', 'SC_MULTISTEPUNDOREDO', 'SC_LASTSTEPINUNDOREDO', 'SC_MULTILINEUNDOREDO', 'SC_MODEVENTMASKALL']:
					takeEnumValueFromPosition = len('SC_')

				out.write('\n\t\t.value("{0}", PYSCR_{1})'.format(val[0][takeEnumValueFromPosition:].upper(), val[0]))
			out.write(';\n\n')

	out.write('\tboost::python::enum_<ScintillaNotification>("SCINTILLANOTIFICATION")') #.format(name, name.upper()))

	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
				if v["FeatureType"] == "evt":
					out.write('\n\t\t.value("{0}", PYSCR_SCN_{1})'.format(name.upper(), name.upper()))
	out.write(';\n\n')

	out.write('\tboost::python::enum_<ScintillaMessage>("SCINTILLAMESSAGE")') #.format(name, name.upper()))
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
				if v["FeatureType"] in ["fun", "get", "set"]:
					out.write('\n\t\t.value("SCI_{0}", PYSCR_SCI_{1})'.format(name.upper(), name.upper()))

	out.write(';\n\n')


def writeScintillaDoc(f, out):
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
			if v["FeatureType"] in ["fun", "get", "set"]:

				if v["Name"] in exclusions:
					continue

				if v["Name"] in specialCases:
					(v["ReturnType"], v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"], _) = specialCases[v["Name"]]
				else:
					sig = mapSignature((v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"]))

					if sig is not None:
						v["ReturnType"] = sig[0]
						v["Param1Type"] = sig[1]
						v["Param2Type"] = sig[2]
					else:
						v["ReturnType"] = mapType(v["ReturnType"])
						v["Param1Type"] = mapType(v["Param1Type"])
						v["Param2Type"] = mapType(v["Param2Type"])

				# out.write("/** " + "\n  * ".join(v["Comment"]) + "\n  */\n")
				out.write(".. method:: editor.")
				out.write(getPythonSignature(v).replace('intptr_t','int')) # documentation should contain int instead of intptr_t
				out.write("\n\n   ")
				out.write("\n   ".join(v["Comment"]).replace('\\', '\\\\'))
				out.write("\n\n   See Scintilla documentation for `{0} <https://www.scintilla.org/ScintillaDoc.html#{0}>`_\n\n".format(symbolName(v)))


def writeScintillaEnums(f, out):
	out.write("\n\n")
	sorted_enums = sorted(f.enums)
	for name in sorted_enums:
		v = f.enums[name]
		if v.get('Values'):
			out.write('{0}\n{1}\n\n.. _{0}:\n.. class:: {0}\n\n'.format(name.upper(), '-' * len(name)))

			for val in v['Values']:
				takeEnumValueFromPosition = None
				for prefix in v['Value'].split(' '):
					if val[0][:len(prefix)] == prefix:
						takeEnumValueFromPosition = len(prefix)
						break

				# Hack for ModificationFlags
				# There's more than one prefix specified for ModificationFlags, separated with spaces.
				# Unfortunately, some of the prefixes are the complete symbol, which means we patch that here to just remove the SC_
				if val[0] in ['SC_STARTACTION', 'SC_MULTISTEPUNDOREDO', 'SC_LASTSTEPINUNDOREDO', 'SC_MULTILINEUNDOREDO', 'SC_MODEVENTMASKALL']:
					takeEnumValueFromPosition = len('SC_')

				out.write('.. attribute:: {0}.{1}\n\n'.format(name.upper(), val[0][takeEnumValueFromPosition:].upper()))


def findEnum(f, name):
	for enumName in f.enums:
		for e in f.enums[enumName]["Value"].split(' '):
			l = len(e)
			if e == name[:l]:
				return enumName
	return None


def findEnumValues(f):
	f.enums = {}
	for name in f.order:
		v = f.features[name]
		if v["FeatureType"] == 'enu':
			f.enums[name] = { 'Name' : name, 'Value': v["Value"] }

	for name in f.order:
		v = f.features[name]
		if v["FeatureType"] == 'val':
			enum = findEnum(f, name)

			if enum is not None:
				if f.enums[enum].get("Values", None) == None:
					f.enums[enum]["Values"] = []
				f.enums[enum]["Values"] += [(name, v["Value"])]


def CopyWithInsertion(input, output, genfn, definition):
	copying = 1
	for line in input.readlines():
		if copying:
			output.write(line)
		if "/* ++Autogenerated" in line:
			copying = 0
			genfn(definition, output)
		if "/* --Autogenerated" in line:
			copying = 1
			output.write(line)


def contents(filename):
	with open(filename) as f:
		return f.read()


def Regenerate(filename, genfn, definition):
	inText = contents(filename)
	tempname = "HFacer.tmp"
	out = open(tempname,"w")
	hfile = open(filename)
	CopyWithInsertion(hfile, out, genfn, definition)
	out.close()
	hfile.close()
	outText = contents(tempname)
	if inText == outText:
		os.unlink(tempname)
	else:
		os.unlink(filename)
		os.rename(tempname, filename)


f = Face.Face()
try:
	f.ReadFromFile("Scintilla.iface")
	findEnumValues(f)
	f.ReadFromFile("LexicalStyles.iface")
	findEnumValues(f)
	with open("ScintillaWrapperGenerated.cpp", 'w') as cpp:
		writeCppFile (f, cpp)

	Regenerate("ScintillaWrapper.h", writeHFile, f)
	Regenerate("ScintillaPython.cpp", writeBoostWrapFile, f)
	Regenerate("Enums.h", writeEnumsHFile, f)
	Regenerate("EnumsWrapper.cpp", writeEnumsWrapperFile, f)
	Regenerate(r"..\..\docs\source\scintilla.rst", writeScintillaDoc, f)
	Regenerate(r"..\..\docs\source\enums.rst", writeScintillaEnums, f)
	#Regenerate("SciLexer.h", printLexHFile, f)
	#print("Maximum ID is %s" % max([x for x in f.values if int(x) < 3000]))
except:
	raise

