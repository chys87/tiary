# /***********************************************************************
#  *
#  * Tiary, a terminal-based diary keeping system for Unix-like systems
#  * Copyright (C) 2009, 2010, 2011, 2012, 2014, 2019,
#  * chys <admin@CHYS.INFO>
#  *
#  * This software is licensed under the 3-clause BSD license.
#  * See LICENSE in the source package and/or online info for details.
#  *
#  **********************************************************************/



AM_CPPFLAGS = @CONF_CPPFLAGS@
LDADD = @CONF_LIBS@

SUBDIRS = common diary main ui

bin_PROGRAMS = tiary

tiary_LDADD = main/libmain.a ui/libui.a diary/libdiary.a common/libcommon.a $(LDADD)

tiary_SOURCES = main.cpp
