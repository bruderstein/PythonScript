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

import sys
import os
import Face



types = {
	'string'	: 'boost::python::str',
	'position'     	: 'int',
	'cells'        	: 'ScintillaCells',
	'colour'	: 'boost::python::tuple',
	'keymod'	: 'int'  # Temporary hack - need this to be a real type 
        }

castsL = {
	'boost::python::str'	: lambda name: "reinterpret_cast<LPARAM>(static_cast<const char*>(boost::python::extract<const char *>(" + name + ")))",
	# Hack - assume a tuple is a colour
        'boost::python::tuple': lambda name: "MAKECOLOUR(" + name + ")".format(name) 
	}
	
castsW = {
	'boost::python::str'	: lambda name: "reinterpret_cast<WPARAM>(static_cast<const char*>(boost::python::extract<const char *>(" + name + ")))",
	# Hack - assume a tuple is a colour
	'boost::python::tuple': lambda name: "MAKECOLOUR(" + name + ")".format(name)
	}	
	
castsRet = {
	'bool' : lambda val: 'return 0 != (' + val + ')',
	'boost::python::tuple': lambda val: 'int retVal = callScintilla(' + val + ');\n\treturn boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal))'
	
	}
	
# Must be kept in sync with pythonTypeExplosions	
typeExplosions = {
	#'colour'    : lambda name: 'int {0}Red, int {0}Green, int {0}Blue'.format(name),
	'findtext'  : lambda name: 'int start, int end, boost::python::object {0}'.format(name),
	'textrange' : lambda name: 'int start, int end'
}

# Must be kept in sync with typeExplosions
pythonTypeExplosions = {
	#'colour'    : lambda name: 'int {0}Red, int {0}Green, int {0}Blue'.format(name),
	'findtext'  : lambda name: 'start, end, {0}'.format(name),
	'textrange' : lambda name: 'start, end'
}

exclusions = [ 'FormatRange' ]

def Contains(s,sub):
	return s.find(sub) != -1

def symbolName(v):
	return "SCI_" + v["Name"].upper()

def castLparam(ty, name):
	return castsL.get(ty, lambda n: n)(name)

def castWparam(ty, name):
	return castsW.get(ty, lambda n: n)(name)

def castReturn(ty, val):
	return castsRet.get(ty, lambda v: 'return ' + v)(val)

def cellsBody(v, out):
	out.write("\treturn callScintilla(" + symbolName(v) + ", " + v["Param2Name"] + ".length(), reinterpret_cast<LPARAM>(" + v["Param2Name"] + ".cells()));\n")
	

def constString(v, out):
	out.write("\tconst char *raw = boost::python::extract<const char *>(" + v["Param2Name"] + ".attr(\"__str__\")());\n")
	out.write("\treturn callScintilla(" + symbolName(v) + ", len(" + v["Param2Name"] + "), reinterpret_cast<LPARAM>(raw));\n");
	
def retString(v, out):
	out.write("\tint resultLength = callScintilla(" + symbolName(v) + ");\n")
	out.write("\tchar *result = new char[resultLength + 1];\n")
	out.write("\tcallScintilla(" + symbolName(v) + ", resultLength + 1, reinterpret_cast<LPARAM>(result));\n")
	out.write("\tresult[resultLength] = '\\0';\n")
	out.write("\tboost::python::str o = boost::python::str(result);\n")
	out.write("\tdelete [] result;\n")
	out.write("\treturn o;\n")

def getLineBody(v, out):
	out.write("\tint lineCount = callScintilla(SCI_GETLINECOUNT);\n")
	out.write("\tif (line >= lineCount)\n")	
	out.write("\t{\n")
	out.write("\t\tthrow out_of_bounds_exception();\n")
	out.write("\t}\n")
	out.write("\telse\n")
	out.write("\t{\n")
	out.write("\t\tint resultLength = callScintilla(" + symbolName(v) + ", line);\n")
	out.write("\t\tchar *result = new char[resultLength + 1];\n")
	out.write("\t\tcallScintilla(" + symbolName(v) + ", line, reinterpret_cast<LPARAM>(result));\n")
	out.write("\t\tresult[resultLength] = '\\0';\n")
	out.write("\t\tboost::python::str o = boost::python::str(result);\n")
	out.write("\t\tdelete [] result;\n")
	out.write("\t\treturn o;\n")
	out.write("\t}\n")
	

def retStringNoLength(v, out):
	out.write("\tint resultLength = callScintilla(" + symbolName(v))
	if v["Param1Type"] != '' or v["Param2Type"] != '':
		out.write(", ")
		if v["Param1Type"] != '':
			out.write(v["Param1Name"]);
			
		if v["Param2Type"] != '':
			out.write(v["Param2Name"]);
	
	out.write(");\n")
	
	out.write("\tchar *result = new char[resultLength + 1];\n")
	out.write("\tcallScintilla(" + symbolName(v) + ", ")
	
	if v["Param1Type"] or v["Param2Type"]:
		if v["Param1Type"]:
			out.write(v["Param1Name"]);
		if v["Param2Type"]:
			out.write(v["Param2Name"]);
	else:
		out.write("0");
				
		
	out.write(", reinterpret_cast<LPARAM>(result));\n")
	out.write("\tresult[resultLength] = '\\0';\n")
	out.write("\tboost::python::str o = boost::python::str(result);\n")
	out.write("\tdelete [] result;\n")
	out.write("\treturn o;\n")

def findTextBody(v, out):
	out.write('\tSci_TextToFind src;\n')
	out.write('\tsrc.chrg.cpMin = start;\n')
	out.write('\tsrc.chrg.cpMax = end;\n')
	out.write('\tsrc.lpstrText = const_cast<char*>((const char *)boost::python::extract<const char *>({0}.attr("__str__")()));\n'.format(v['Param2Name']))
	out.write('\tint result = callScintilla({0}, {1}, reinterpret_cast<LPARAM>(&src));\n'.format(symbolName(v), v["Param1Name"]))
	out.write('\tif (-1 == result)\n')
	out.write('\t{\n\t\treturn boost::python::object();\n\t}\n')
	out.write('\telse\n\t{\n\t\treturn boost::python::make_tuple(src.chrgText.cpMin, src.chrgText.cpMax);\n\t}\n')
	
	
def getTextRangeBody(v, out):
	out.write('\tSci_TextRange src;\n')
	out.write('\tif (end < start)\n')
	out.write('\t{\n')
	out.write('\t\tint temp = start;\n')
	out.write('\t\tstart = end;\n')
	out.write('\t\tend = temp;\n')
	out.write('\t}\n')
	out.write('\tsrc.chrg.cpMin = start;\n')
	out.write('\tsrc.chrg.cpMax = end;\n')
	out.write('\tsrc.lpstrText = new char[(end-start) + 1];\n')
	out.write('\tcallScintilla({0}, 0, reinterpret_cast<LPARAM>(&src));\n'.format(symbolName(v)))
	out.write('\tboost::python::str ret(src.lpstrText);\n')
	out.write('\tdelete [] src.lpstrText;\n')
	out.write('\treturn ret;\n')
	

def getStyledTextBody(v, out):
	out.write('\tSci_TextRange src;\n')
	out.write('\tif (end < start)\n')
	out.write('\t{\n')
	out.write('\t\tint temp = start;\n')
	out.write('\t\tstart = end;\n')
	out.write('\t\tend = temp;\n')
	out.write('\t}\n')
	out.write('\tsrc.chrg.cpMin = start;\n')
	out.write('\tsrc.chrg.cpMax = end;\n')
	out.write('\tsrc.lpstrText = new char[((end-start) * 2) + 2];\n')
	out.write('\tcallScintilla({0}, 0, reinterpret_cast<LPARAM>(&src));\n'.format(symbolName(v)))
	out.write('\tboost::python::list styles;\n')
	out.write('\tchar *result = new char[(end-start) + 1];\n')
	out.write('\tfor(int pos = 0; pos < (end - start); pos++)\n')
	out.write('\t{\n')
	out.write('\t\tresult[pos] = src.lpstrText[pos * 2];\n')
	out.write('\t\tstyles.append((int)(src.lpstrText[(pos * 2) + 1]));\n')
	out.write('\t}\n')
	out.write("\tresult[end-start] = '\\0';\n")	
	out.write('\tboost::python::str resultStr(result);\n')
	out.write('\tdelete [] src.lpstrText;\n')
	out.write('\tdelete [] result;\n')
	out.write('\treturn boost::python::make_tuple(resultStr, styles);\n')

	
	
def standardBody(v, out):
	call = 'callScintilla(' + symbolName(v)

	if v["Param2Type"] != '' and v["Param1Type"] == '':
		call += ', 0'
		
	if v["Param1Type"]:
		call += ', ' + castWparam(v["Param1Type"], v["Param1Name"])
		
	if v["Param2Type"]:
		call += ', ' + castLparam(v["Param2Type"], v["Param2Name"])
	call += ")"
	
	
	if (v["ReturnType"] != 'void'):
		out.write('\t')
		out.write(castReturn(v["ReturnType"], call))
	else:
		out.write('\t' + call)

	out.write(";\n")

        
				
def mapType(t):
	return types.get(t, t)
	
def mapCompare(t, s):
	if (t == s or t == ''):
		return True
	else:
		return False
	
def mapSignature(s):
	
	for t in argumentMap:
		if mapCompare(t[0], s[0]) and mapCompare(t[1], s[1]) and mapCompare(t[2], s[2]) and mapCompare(t[3], s[3]):
			return argumentMap[t];
	return None

# Explodes a type to more parameters - e.g. colour
def explodeType(ty, name):
	return typeExplosions.get(ty, lambda name: ty + " " + name)(name)  

def explodePythonType(ty, name):
	return pythonTypeExplosions.get(ty, lambda name: name)(name)  


def writeParams(param1Type, param1Name, param2Type, param2Name):
	retVal = ""
	if param1Type:
		retVal += explodeType(param1Type, param1Name)
		
		if param2Type:
			retVal += ', '
	if param2Type:
		retVal += explodeType(param2Type, param2Name)
		
	return retVal
		

def writePythonParams(param1Type, param1Name, param2Type, param2Name):
	retVal = ""
	if param1Type:
		retVal += explodePythonType(param1Type, param1Name)
		
		if param2Type:
			retVal += ', '
	if param2Type:
		retVal += explodePythonType(param2Type, param2Name)
		
	return retVal


argumentMap = {
#  (firstParamType,     firstParamName, secondParamType, secondParamName)  :  ( returnType, FirstParamType, SecondParamType, bodyFunction)
   ('int', 		'length', 	'string', 	'') 	: ('int', '', 'boost::python::object', constString),
   ('int', 		'length', 	'stringresult', '') 	: ('boost::python::str', '' ,   '', retString),
   ('int',		'line',		'stringresult',	'')	: ('boost::python::str', 'int', '',  retStringNoLength),
   ('', 		'', 		'stringresult', '') 	: ('boost::python::str', '' ,   '', retStringNoLength),
   ('int',		'length', 	'cells',	'')	: ('int', '', 'ScintillaCells', cellsBody),
   ('int',		'',		'findtext', 	'ft')	: ('boost::python::object', 'int', 'findtext', findTextBody),
   ('',			'',		'textrange', 	'tr')	: ('boost::python::str', '', 'textrange', getTextRangeBody)
   	
}

specialCases = {
	'GetStyledText' : ('boost::python::tuple', 'int', 'start', 'int', 'end', getStyledTextBody),
	'GetLine': ('boost::python::str', 'int', 'line', '', '', getLineBody)
}

def getSignature(v):
	sig = v["ReturnType"] + " ScintillaWrapper::" + v["Name"] + "("
	sig += writeParams(v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"])
	sig += ")"
	return sig

def formatPythonName(name):
	return name[0:1].lower() + name[1:]


def getPythonSignature(v):
	sig = "{0}(".format(formatPythonName(v["Name"]))
	
	sig += writePythonParams(v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"])
	sig += ")"
	if v["ReturnType"] and v["ReturnType"] != "void":
		sig += " -> " + v["ReturnType"].replace("boost::python::", "")
		
	
	return sig


def writeCppFile(f,out):
	out.write('#include "stdafx.h"\n')
	out.write('#include "ScintillaWrapper.h"\n')
	out.write('#include "Scintilla.h"\n')
	out.write('\n\n')
	
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
			if v["FeatureType"] in ["fun", "get", "set"]:
				
				if v["Name"] in exclusions:
					continue
					
				if v["Name"] in specialCases:
					(v["ReturnType"], v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"], body) = specialCases[v["Name"]]   
				else:
					sig = mapSignature((v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"]))
					returnType = "int"
					if sig is not None:
						v["ReturnType"] = sig[0]
						v["Param1Type"] = sig[1]
						v["Param2Type"] = sig[2]
						body = sig[3]
					else:
						v["ReturnType"] = mapType(v["ReturnType"])
						v["Param1Type"] = mapType(v["Param1Type"])
						v["Param2Type"] = mapType(v["Param2Type"])
						body = standardBody
				
				out.write("/** " + "\n  * ".join(v["Comment"]) + "\n  */\n")
				
				out.write(getSignature(v))
				out.write("\n{\n")
				body(v, out)
				out.write("}\n\n")
				
			
				
				
def writeHFile(f,out):
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
			if v["FeatureType"] in ["fun", "get", "set"]:
				
				if v["Name"] in exclusions:
					continue
				
				sig = mapSignature((v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"]))
				returnType = "int"
				if sig is not None:
					v["ReturnType"] = sig[0]
					v["Param1Type"] = sig[1]
					v["Param2Type"] = sig[2]
					body = sig[3]
				else:
					v["ReturnType"] = mapType(v["ReturnType"])
					v["Param1Type"] = mapType(v["Param1Type"])
					v["Param2Type"] = mapType(v["Param2Type"])
					body = standardBody
				
				out.write("\t/** " + "\n\t  * ".join(v["Comment"]) + "\n  */\n")
				
				out.write("\t");
				out.write(getSignature(v).replace(' ScintillaWrapper::', ' '))
				out.write(";\n\n")

	
def writeBoostWrapFile(f,out):
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
			if v["FeatureType"] in ["fun", "get", "set"]:
				
				if v["Name"] in exclusions:
					continue
				
				out.write('\t\t.def("{0}", &ScintillaWrapper::{1}, \"'.format(formatPythonName(v["Name"]), v["Name"]))
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
				out.write('\n\t\t.value("{0}", PYSCR_{1})'.format(val[0][len(v['Value']):].upper(), val[0]))
			out.write(';\n\n')

	out.write('\tboost::python::enum_<ScintillaNotification>("SCINTILLANOTIFICATION")'.format(name, name.upper()))
	
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
				if v["FeatureType"] == "evt":
					out.write('\n\t\t.value("{0}", PYSCR_SCN_{1})'.format(name.upper(), name.upper())) 
	out.write(';\n\n')
	
	out.write('\tboost::python::enum_<ScintillaMessage>("SCINTILLAMESSAGE")'.format(name, name.upper()))
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
				if v["FeatureType"] in ["fun", "get", "set"]:
					out.write('\n\t\t.value("SCI_{0}", PYSCR_SCI_{1})'.format(name.upper(), name.upper())) 
					
	out.write(';\n\n')
	
def writeScintillaDoc(f, out):
	oldCat = ""
	
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
			if v["FeatureType"] in ["fun", "get", "set"]:
				

				
				if v["Name"] in exclusions:
					continue
					
				if v["Name"] in specialCases:
					(v["ReturnType"], v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"], body) = specialCases[v["Name"]]   
				else:
					sig = mapSignature((v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"]))
					returnType = "int"
					if sig is not None:
						v["ReturnType"] = sig[0]
						v["Param1Type"] = sig[1]
						v["Param2Type"] = sig[2]
					else:
						v["ReturnType"] = mapType(v["ReturnType"])
						v["Param1Type"] = mapType(v["Param1Type"])
						v["Param2Type"] = mapType(v["Param2Type"])
				
				
				# out.write("/** " + "\n  * ".join(v["Comment"]) + "\n  */\n")
				out.write(".. method:: Editor.") 
				out.write(getPythonSignature(v))
				out.write("\n\n   ")
				out.write("\n   ".join(v["Comment"]))
				out.write("\n\n   See Scintilla documentation for `{0} <http://www.scintilla.org/ScintillaDoc.html#{0}>`_\n\n".format(symbolName(v)))
				

def writeScintillaEnums(f, out):
	out.write("\n\n")
	sorted_enums = sorted(f.enums)
	for name in sorted_enums:
		v = f.enums[name]
		if v.get('Values'):
			out.write('{0}\n{1}\n\n.. _{0}:\n.. class:: {0}\n\n'.format(name.upper(), '-' * len(name)))
			
			for val in v['Values']:
				out.write('.. attribute:: {0}.{1}\n\n'.format(name.upper(), val[0][len(v['Value']):].upper()))
	
					
				
	
def findEnum(f, name):
	for e in f.enums:
		l = len(f.enums[e]["Value"])
		if f.enums[e]["Value"] == name[:l]:
			return e
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
		if Contains(line, "/* ++Autogenerated"):
			copying = 0
			genfn(definition, output)
		if Contains(line, "/* --Autogenerated"):
			copying = 1
			output.write(line)

def contents(filename):
	f = open(filename)
	t = f.read()
	f.close()
	return t

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
	cpp = open("ScintillaWrapperGenerated.cpp", 'w')
	writeCppFile (f, cpp)
	cpp.close()
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
	
