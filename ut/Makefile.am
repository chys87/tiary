# /***********************************************************************
#  *
#  * Tiary, a terminal-based diary keeping system for Unix-like systems
#  * Copyright (C) 2019, chys <admin@CHYS.INFO>
#  *
#  * This software is licensed under the 3-clause BSD license.
#  * See LICENSE in the source package and/or online info for details.
#  *
#  **********************************************************************/


AM_CPPFLAGS = @CONF_CPPFLAGS@
LDADD = ../src/common/libcommon.a -lgtest_main -lgtest
check_PROGRAMS = datetime.out format.out
TESTS = $(check_PROGRAMS)
datetime_out_SOURCES = datetime.cpp
format_out_SOURCES = format.cpp
