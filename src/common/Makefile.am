# /***********************************************************************
#  *
#  * Tiary, a terminal-based diary keeping system for Unix-like systems
#  * Copyright (C) 2009, 2010, 2011, 2012, 2014, 2018, 2019,
#  * chys <admin@CHYS.INFO>
#  *
#  * This software is licensed under the 3-clause BSD license.
#  * See LICENSE in the source package and/or online info for details.
#  *
#  **********************************************************************/



AM_CPPFLAGS = @CONF_CPPFLAGS@

noinst_LIBRARIES = libcommon.a

libcommon_a_SOURCES = \
	action.h \
	aes.h \
	aes.cpp \
	algorithm.h \
	algorithm.cpp \
	bswap.h \
	bzip2.h \
	bzip2.cpp \
	condition.h \
	condition.cpp \
	containers.h \
	datetime.h \
	datetime.cpp \
	datetime_format.cpp \
	debuglog.h \
	debuglog.cpp \
	digest.h \
	dir.h \
	dir.cpp \
	external.h \
	external.cpp \
	format.h \
	format.cpp \
	misc.h \
	misc.cpp \
	re.h \
	re.cpp \
	signal.h \
	signal.cpp \
	split_line.h \
	split_line.cpp \
	string.h \
	string.cpp \
	string_match.h \
	string_match.cpp \
	unicode.h \
	unicode.cpp \
	unicode.gen.cpp \
	xml.h \
	xml.cpp

noinst_PROGRAMS = unicode_range.gen

unicode_range_gen_SOURCES = unicode_range_gen.cpp

CLEANFILES = unicode.gen.cpp

unicode.gen.cpp: unicode_range.gen$(EXEEXT)
	./unicode_range.gen$(EXEEXT) > $@ || (rm -f $@; exit 1)
