#coding=utf-8

import re

# --------------------------------------------------------------------------------------------------------------------------------------

#                                           Script "Multiples_SR.py"

# A LITTLE adaptation by guy038 from an ORIGINAL and VALUABLE script of Alan KILBORN ( January 2019 ) !

# See https://notepad-plus-plus.org/community/topic/16942/pythonscript-any-ready-pyscript-to-replace-one-huge-set-of-regex-phrases-with-others/21
# and https://notepad-plus-plus.org/community/topic/16942/pythonscript-any-ready-pyscript-to-replace-one-huge-set-of-regex-phrases-with-others/23

# This script :

#   - Reads an existing "SR_List.txt" file, of the CURRENT directory, containing a list of SEARCH/REPLACEMENT strings, ONE PER line
#   - Selects, one at a time, a COUPLE of SEARCH and REPLACEMENT regexes  / expressions / strings / characters
#   - Executes this present S/R on CURRENT edited file, in NOTEPAD++
#   - Loop till the END of file

# Any PURE BLANK line or COMMENT line, beginning with '#', of the "SR_list.txt" file, are simply IGNORED

# --------------------------------------------------------------------------------------------------------------------------------------

# For EACH line, in the "SR_List.txt" file, the format is <DELIMITER><SEARCH regex><DELIMITER><REPLACE regex><DELIMITER>

## EXAMPLES :
## ¯¯¯¯¯¯¯¯

##  Deletes any [ending] "; comment"  /  Delimiter = '!'
#!(?-s)(^.*?);.+!\1!

##  Changes any LOWER-case string "notepad++" in its UPPER-case equivalent  /  Delimiter = '@'
#@(?-i)notepad\+\+@NOTEPAD++@

##  Changes any "Smith" and 'James' strings, with that EXACT case, to, respectively, "Name" and "First name"  /  Delimiter = '&'
##  Deletes any "TEST" string, with that EXACT case
#&(Smith)|TEST|(James)&(?1Name)(?2First name)&

##  Replaces any BACKSLASH character with the "123" number, both  preceded and followed with 3 SPACE characters  /  Delimiter = '%'
#%\\%   123   %
##    or, also, the syntax   %\x5c%   123   %

##  Deletes any string "Fix", followed with a SPACE char, whatever its CASE  /  Delimiter = '+'
#+(?i)Fix ++

##  Change 3 CONSECUTIVE "#" characters with 3 BACKSLASH characters  /  Delimiter = '*'
#*###*\\\\\\*

# --------------------------------------------------------------------------------------------------------------------------------------

# In the CODE line, right below, you may :

#   - Modify the NAME of the file, containing the SEARCH and REPLACEMENT regexes  
#   - Indicate an ABSOLUTE or RELATIVE path, before the filename

with open(r'SR_list.txt') as f: sr_list = f.readlines()

# You may, as well, insert the SEARCH and REPLACE regexes, directly, in THIS script :

#sr_list = [
#    '!(?-s)(^.*?);.+!\\1!',
#    '@(?-i)notepad\\+\\+@NOTEPAD++@',
#    '&(Smith)|TEST|(James)&(?1Name)(?2First name)&',
#    '%\\\\%   123   %',
#          # or the syntax  '%\x5c\x5c%   123   %',
#    '+(?i)Fix ++',
#    '*###*\\\\\\\\\\\\*',
#    ]

# The use of RAW strings  r'.......'  is also possible, in order to SIMPLIFY some regexes

# Note that these RAW regexes are strictly IDENTICAL to those, which could be contained in a "SR_List.txt" file, WITHOUT the 'r' PREFIX 

#sr_list = [
#    r'!(?-s)(^.*?);.+!\1!',
#    r'@(?-i)notepad\+\+@NOTEPAD++@',
#    r'&(Smith)|TEST|(James)&(?1Name)(?2First name)&',
#    r'%\\%   123   %',
#          # or the syntax  r'%\x5c%   123   %',
#    r'+(?i)Fix ++',
#    r'*###*\\\\\\*',
#    ]

editor.beginUndoAction()

console.write ('\nMODIFICATIONS on FILE "{}: "\n\n'.format(notepad.getCurrentFilename()))

# Note : Variable e is always EMPTY string ( Part AFTER the THIRD delimiter and BEFORE the END of line ! )

for line in sr_list:

    if line[0] == '#' or line == '\n' : continue
    (s,r,e) = line[1:].rstrip('\n').split(line[0])

    console.write('    SEARCH  : >{}<\n'.format(s))
    console.write('    REPLACE : >{}<\n\n'.format(r))

    editor.rereplace(s,r)   # or editor.rereplace(s,r,re.IGNORECASE) / editor.rereplace(s,r,re.I)

editor.endUndoAction()

# END of Multiple_SR.py script
