#!/usr/bin/env python
"""
@file    version.py
@author  Michael.Behrisch@dlr.de
@date    2007
@version $Id: version.py 6858 2009-02-25 13:27:57Z behrisch $

This script rebuilds "/../../src/version.h", the file which
 lets the applications know the version of their build.
If a version file exists, it is kept, otherwise the current
 SVN revision is used as version information.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import sys
from os.path import dirname, exists, getmtime

versionFile = dirname(sys.argv[0]) + "/../../src/version.h"
entriesFile = ".svn/entries"
svnRevision = "UNKNOWN"
if exists(entriesFile):
    if exists(versionFile) and getmtime(versionFile) > getmtime(entriesFile):
        sys.exit()
    entries = open(entriesFile)
    for i, l in enumerate(entries):
        if i == 3 and l.strip().isdigit():
            svnRevision = l.strip()
        revIndex = l.find('revision="')
        if revIndex >= 0:
            revIndex += 10
            svnRevision = l[revIndex:l.index('"', revIndex)]
            break
    entries.close()

version = open(versionFile, 'w')
print >> version, '#define VERSION_STRING "dev-SVN-r%s"' % svnRevision
version.close()
