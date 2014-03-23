"""Extracts the version number of PythonScript"""

import sys, os
import re

pathToVersionFile = os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), '..\PythonScript\src\PythonScriptVersion.h')

versionHeader = open(pathToVersionFile, 'r')
versionRegex = re.compile(r'PYSCR_VERSION_STRING\s+"([0-9.]+)"')

for line in versionHeader:
	match = versionRegex.search(line)
	if match:
		print match.group(1),
		break



