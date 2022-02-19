# -*- coding: utf-8 -*-
import datetime

# Just in case, we'll clear all the existing callbacks for FILEBEFORESAVE
notepad.clearCallbacks([NOTIFICATION.FILEBEFORESAVE])

# Define the function to call just before the file is saved
def addSaveStamp(args):
	if notepad.getBufferFilename(args["bufferID"])[-4:] == '.log':
		result = notepad.messageBox("Saving a .log file.  Would you like to append a 'File saved on...' message to the file?  (This is a Python Script event, click cancel to stop this message appearing)", "Python Script Callback", MESSAGEBOXFLAGS.YESNOCANCEL)
		if result == MESSAGEBOXFLAGS.RESULTCANCEL:
			notepad.clearCallbacks([NOTIFICATION.FILEBEFORESAVE])
		elif result == MESSAGEBOXFLAGS.RESULTYES:
			notepad.activateBufferID(args["bufferID"])
			editor.appendText("File saved on %s\r\n" % datetime.date.today())


# ... and register the callback
notepad.callback(addSaveStamp, [NOTIFICATION.FILEBEFORESAVE])

# As this is a sample, we'll inform the user
notepad.messageBox("FILEBEFORESAVE notification registered.\n*.log files will now automatically be modified before saving.\nCtrl-Click the script to edit.\n", "Python Script Demo", 0)
