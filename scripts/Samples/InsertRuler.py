# -*- coding: utf-8 -*-
"""
    InsertRuler - Demo

    Inserts a basic ruler using annotation

    Usage:
        Run script.
"""
from Npp import editor

ruler_header = ''.join(['{:>10}'.format(x) for x in range(1,21)])
ruler_footer = ''.join(['{0} '.format('123456789') for x in range(1,21)])

editor.annotationSetText(0, '{}\r\n{}'.format(ruler_header,ruler_footer))
editor.annotationSetStyle(0, 0)
editor.annotationSetVisible(not editor.annotationGetVisible())