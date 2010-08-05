
def testContents(contents, lineNumber, totalLines):
        if contents.strip() == "rubbish":
                editor.deleteLine(lineNumber)
                # As we've deleted the line, the "next" line to process
                # is actually the current line, so we return 0 to advance zero lines
                # and hence stay on the same line
                return 0

        elif contents.strip() == "something old":
                editor.replaceLine(lineNumber, "something new")

        elif contents.strip() == "little something":
                editor.replaceLine(lineNumber, "BIG\nSOMETHING"
                # Here we return 2, as we've inserted a newline,
                # and we don't want to test the "SOMETHING" line again
                return 2

        # if you wanted, you could optionally return 1 here, to move the next line
        # but that's the default, so you don't need to bother.


editor.forEachLine(testContents)

# Inform the user that we've done it
notepad.messageBox("Replacements and removals completed", "Python Script Demo", 0)