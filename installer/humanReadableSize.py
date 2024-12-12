# -*- coding: utf-8 -*-

from __future__ import print_function
import sys, os

# Stolen from http://stackoverflow.com/questions/1094841/reusable-library-to-get-human-readable-version-of-file-size
def sizeof_format(size):
    for units in ['bytes','KB','MB','GB']:
        if size < 1024.0:
            return "%3.1f%s" % (size, units)
        size /= 1024.0
    return "%3.1f%s" % (size, 'TB')

print(sys.argv[1])

print(sizeof_format(os.stat(sys.argv[1]).st_size))
