# -*- coding: utf-8 -*-
# You can for example bound this script to CTRL+SHIFT+C in Settings > Shortcut Mapper > Plugins Commands > Run Previous Script
# It will show a typical Notepad++/Scintilla autocompletion list, but based on the content of the `tags` file in the current file directory

# The CTags format is described there: http://ctags.sourceforge.net/FORMAT
# But this script will also work if the `tags` file simply contains one word per line

import os
ctags_filepath = os.path.join(os.path.dirname(notepad.getCurrentFilename()), 'tags')
if os.path.exists(ctags_filepath):
    with open(ctags_filepath) as ctags_file:
        ctags = set(line.split('\t')[0] for line in ctags_file.readlines() if not line.startswith('!'))
    editor.autoCShow(0, ' '.join(sorted(ctags)))
