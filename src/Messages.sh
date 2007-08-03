#! /usr/bin/env bash
$EXTRACTRC `find . -name \*.ui -o -name \*.rc -o -name \*.kcfg` > rc.cpp
$XGETTEXT *.cpp \
	    catalog/*.cpp \
	    cataloglistview/*.cpp \
	    common/*.cpp \
	    glossary/*.cpp \
	    mergemode/*.cpp \
	    prefs/*.cpp \
	    project/*.cpp \
	    tm/*.cpp \
	    webquery/*.cpp \
	    -o $podir/kaider.pot
