# -*- coding: utf-8 -*-
# From https://sourceforge.net/p/npppythonscript/discussion/1199074/thread/988630ae/
# Multi-Edit needs to be activated in the N++ edit preferences.

def getWord_Sel(selNum):    # tuple of:  word, word_start, word_end
    sPos = editor.getSelectionNStart(selNum)
    ePos = editor.getSelectionNEnd(selNum)
    if sPos != ePos:
        sWrd = editor.getTextRange(sPos, ePos)
    else:
        sPos = editor.wordStartPosition(sPos, True)
        ePos = editor.wordEndPosition(sPos, True)
        sWrd = editor.getTextRange(sPos, ePos)

    return (sWrd, sPos, ePos)

def replaceWords(var1, var2):
    editor.setTarget(var2[1], var2[2])
    editor.replaceTarget(var1[0])
    editor.setTarget(var1[1], var1[2])
    editor.replaceTarget(var2[0])
    return len(var2[0]) - len(var1[0])

def setSels(var1, var2, diff):
    editor.setSelection(var1[1], var1[1]+len(var2[0]))
    editor.addSelection(var2[1]+diff, var2[1]+diff+len(var1[0]))

selsCt = editor.getSelections()
if selsCt == 2:
    editor.beginUndoAction()

    tSel1 = getWord_Sel(0)
    tSel2 = getWord_Sel(1)
    if tSel1[1] < tSel2[1]:
        sel1, sel2 = tSel1, tSel2
    else:
        sel1, sel2 = tSel2, tSel1

    diff = replaceWords(sel1, sel2)
    setSels(sel1, sel2, diff)

    editor.endUndoAction()