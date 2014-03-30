import sys, re

versionRegex = re.compile('^\s*Latest Version: ([0-9]+\.){3}[0-9]+\s*')
header = open(sys.argv[1], "r")
result = ''
for line in header:
	if versionRegex.match(line):
		result += 'Latest Version: %s\r\n' % sys.argv[2]
	else:
		result += line

header.close()
header = open(sys.argv[1], "w")
header.write(result)
header.close()
