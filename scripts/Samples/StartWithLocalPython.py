# -*- coding: utf-8 -*-
"""
    StartWithLocalPython

    Executes active document with local installed python interpreter
    instead of using the python script plugin delivered engine.

    Usage:
    Local python_interpreter path needs to be modified.
    Open python file and run script.
    Output will be printed to console window.
"""
from Npp import notepad, console, MESSAGEBOXFLAGS
import subprocess
import shlex
import os

python_interpreter = {
    MESSAGEBOXFLAGS.RESULTYES : u'D:\\ProgramData\\Python\\Python37_64\\python.exe',
    MESSAGEBOXFLAGS.RESULTNO : u'D:\\ProgramData\\Python\\Python27_64\\python.exe'}

def run_command(command, __cwd):
    
    _startupinfo = subprocess.STARTUPINFO() 
    _startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    process = subprocess.Popen(shlex.split(command),
                               startupinfo=_startupinfo,
                               stdout=subprocess.PIPE, 
                               stderr=subprocess.PIPE, 
                               cwd=__cwd)
    _stdout, _stderr = process.communicate()
    
    console.show()
    if _stdout: console.write(_stdout)
    if _stderr: console.write(_stderr)


file = notepad.getCurrentFilename()
_cwd, _ = os.path.split(file)
answer = notepad.messageBox(('Yes = Python 37_64\n\n'
                             'No = Python 27_64\n\n'
                             'Cancel = No execution'),'Select python interpreter',3)
if answer != MESSAGEBOXFLAGS.RESULTCANCEL:
    run_command('"{}" -u "{}"'.format(python_interpreter[answer], file), _cwd)