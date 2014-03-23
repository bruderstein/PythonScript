
# First we'll start an undo action, then Ctrl-Z will undo the actions of the whole script

editor.beginUndoAction()


# Do a Python regular expression replace, full support of Python regular expressions

# This replaces any three uppercase letters that are repeated, 
# so ABDABD, DEFDEF or DGBDGB etc. the first 3 characters, 
# so ABD, DEF and DGB in these cases.

editor.rereplace(r"([A-Z]{3})\1", r"\1")

# Do a multi-line Python regular expression replace.

# This example replaces any <br/> that is followed by another on the next line (with optional spaces in between), with a single one
editor.rereplace(r"<br/>\s*\r\n\s*<br/>", "<br/>\r\n")


# End the undo action, so Ctrl-Z will undo the above two actions
editor.endUndoAction()