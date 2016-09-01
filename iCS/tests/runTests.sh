#!/bin/bash
#Bash script for the test run.
#Accepts two parameter:
#1. Directory for binaries (defaults to ./src)
#2. Path to texttest.py executable (default is simply texttest.py)

OLDDIR=$PWD
cd `dirname $0`
export TEXTTEST_HOME="$PWD"
if test x"$1" = x; then
  cd ..
  ICS_BIN="$PWD/src"
else
  ICS_BIN="$1"
fi
cd $OLDDIR
export ICS_BINARY="$ICS_BIN/ics"

if test x"$2" = x; then
  TEXTTEST=texttest.py
else
  TEXTTEST="$2"
fi

$TEXTTEST

