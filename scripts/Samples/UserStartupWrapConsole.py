# -*- coding: utf-8 -*-
# From https://github.com/bruderstein/PythonScript/issues/330
# Wrap text in console pythonscript window by putting the following (or an adapted version) in user startup.py:


console.editor.setWrapMode(WRAP.WORD)
console.editor.setWrapIndentMode(WRAPINDENTMODE.SAME)
console.editor.setWrapStartIndent(0)
console.editor.setWrapVisualFlags(WRAPVISUALFLAG.END)

console.editor.setWrapVisualFlagsLocation(WRAPVISUALLOCATION.END_BY_TEXT)
