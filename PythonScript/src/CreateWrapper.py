# CreateWrapper.py - regenerate the ScintillaWrapper.h and ScintillaWrapper.cpp
# files from the Scintilla.iface interface definition file.

# Todo:

# GetViewWS
# GetCharAt
#  Type: colour
# AutoCSetSeparator
# UserListShow
# AutoCSetTypeSeparator - ignore - cannot change this
# GetReadOnly, GetLineVisible, GetTabIndents,GetBackSpaceUnIndents - need a list of methods that return a bool
# FindText
#  Type: findtext
#  Type: formatrange
# GetSelText - stringresult
#  Type: textrange
# SetSearchFlags  - need flags type in list or something
# SetWrapMode - modes
# SetLengthForEncode, TargetAsUTF8 - these want wrapping up, and excluding
# CreateDocument etc, move to advanced?

import sys
import os
import Face



types = {
	'string'	: 'str',
	'position'     	: 'int',
	'cells'        	: 'ScintillaCells',
	'colour'	: 'ScintillaColour'
        }

castsL = {
	'str'	: lambda name: "reinterpret_cast<LPARAM>(static_cast<const char*>(extract<const char *>(" + name + ")))"
	}
castsW = {
	'str'	: lambda name: "reinterpret_cast<WPARAM>(static_cast<const char*>(extract<const char *>(" + name + ")))"
	}	
	
castsRet = {
	'bool' : lambda val: 'static_cast<bool>(' + val + ')'
	}
	

def Contains(s,sub):
	return s.find(sub) != -1

def symbolName(v):
	return "SCI_" + v["Name"].upper()

def castLparam(ty, name):
	return castsL.get(ty, lambda n: n)(name)

def castWparam(ty, name):
	return castsW.get(ty, lambda n: n)(name)

def castReturn(ty, val):
	return castsRet.get(ty, lambda v: v)(val)

def cellsBody(v, out):
	out.write("\treturn call(" + symbolName(v) + ", " + v["Param2Name"] + ".length(), " + v["Param2Name"] + ".cellsPtr());\n")
	
def constString(v, out):
	out.write("\tconst char *raw = extract<const char *>(" + v["Param2Name"] + ");\n")
	out.write("\treturn call(" + symbolName(v) + ", len(" + v["Param2Name"] + "), reinterpret_cast<LPARAM>(raw));\n");
	
def retString(v, out):
	out.write("\tint resultLength = call(" + symbolName(v) + ");\n")
	out.write("\tchar *result = (char *)malloc(resultLength + 1);\n")
	out.write("\tcall(" + symbolName(v) + ", resultLength + 1, reinterpret_cast<LPARAM>(result));\n")
	out.write("\tresult[resultLength] = '\0';\n")
	out.write("\tstr o = str((const char *)result);\n")
	out.write("\tfree(result);\n")
	out.write("\treturn o;\n")

def retStringNoLength(v, out):
	out.write("\tint resultLength = call(" + symbolName(v) + ");\n")
	out.write("\tchar *result = (char *)malloc(resultLength + 1);\n")
	out.write("\tcall(" + symbolName(v) + ", 0, reinterpret_cast<LPARAM>(result));\n")
	out.write("\tresult[resultLength] = '\0';\n")
	out.write("\tstr o = str((const char *)result);\n")
	out.write("\tfree(result);\n")
	out.write("\treturn o;\n")

		

def standardBody(v, out):
	call = 'call(' + symbolName(v)

	if v["Param2Type"] != '' and v["Param1Type"] == '':
		call += ', 0'
		
	if v["Param1Type"]:
		call += ', ' + castWparam(v["Param1Type"], v["Param1Name"])
		
	if v["Param2Type"]:
		call += ', ' + castLparam(v["Param2Type"], v["Param2Name"])
	call += ")"
	
	
	if (v["ReturnType"] != 'void'):
		out.write('\treturn ')
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


def writeParams(param1Type, param1Name, param2Type, param2Name, out):
	if param1Type: 
		out.write(param1Type + " " + param1Name)
		if param2Type:
			out.write(', ')
	if param2Type:
		out.write(param2Type + " " + param2Name)
		
argumentMap = { 
   ('int', 		'length', 	'string', 	'') 	: ('int', '', 'str', constString),
   ('int', 		'length', 	'stringresult', '') 	: ('str', '' ,   '', retString),
   ('', 		'', 		'stringresult', '') 	: ('str', '' ,   '', retStringNoLength),
   ('int',		'length', 	'cells',	'')	: ('int', '', 'ScintillaCells', cellsBody)
   	}		

def printHFile(f,out):
	for name in f.order:
		v = f.features[name]
		if v["Category"] != "Deprecated":
			if v["FeatureType"] in ["fun", "get", "set"]:
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
				out.write(v["ReturnType"] + " ScintillaWrapper::" + name + "(")
				
				writeParams(v["Param1Type"], v["Param1Name"], v["Param2Type"], v["Param2Name"], out)
				out.write(")\n{\n")
				body(v, out)
				out.write("}\n\n")
			
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
	printHFile (f, sys.stdout)
	#Regenerate("SciLexer.h", printLexHFile, f)
	#print("Maximum ID is %s" % max([x for x in f.values if int(x) < 3000]))
except:
	raise
