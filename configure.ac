dnl /***********************************************************************
dnl  *
dnl  * Tiary, a terminal-based diary keeping system for Unix-like systems
dnl  * Copyright (C) 2009, 2010, 2011, 2012, 2014, 2016, 2018 chys <admin@CHYS.INFO>
dnl  *
dnl  * This software is licensed under the 3-clause BSD license.
dnl  * See LICENSE in the source package and/or online info for details.
dnl  *
dnl  **********************************************************************/

dnl You will need a recent version of autoconf, automake and autoconf-archive
dnl to use this file.

AC_DEFUN([MAJOR_VERSION], [0])
AC_DEFUN([MINOR_VERSION], [3])
AC_DEFUN([PATCH_VERSION], [1])

AC_INIT([tiary],MAJOR_VERSION.MINOR_VERSION.PATCH_VERSION,[admin@CHYS.INFO])
AM_INIT_AUTOMAKE([foreign -Wall dist-bzip2 dist-xz nostdinc])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

dnl Use CXX compiler during configure
AC_LANG(C++)

dnl Define versions
AC_DEFINE([TIARY_MAJOR_VERSION],MAJOR_VERSION,[Version])
AC_DEFINE([TIARY_MINOR_VERSION],MINOR_VERSION,[Version])
AC_DEFINE([TIARY_PATCH_VERSION],PATCH_VERSION,[Version])
dnl Remember to change the years on every new year!
AC_DEFINE([TIARY_COPYRIGHT_YEAR],["2009, 2010, 2011, 2012, 2013, 2014, 2018"],[Copyright year])
AC_DEFINE([TIARY_COPYRIGHT_YEAR_WIDE],[L"2009, 2010, 2011, 2012, 2013, 2014, 2018"],[Copyright year])

dnl Define some useful feature macros
AC_USE_SYSTEM_EXTENSIONS
AH_TOP([#include "config-common.h"])

dnl Check for required wchar_t support
AC_FUNC_MBRTOWC
if test x"$ac_cv_func_mbrtowc" != xyes; then
	AC_MSG_ERROR([Unicode support unavailable.])
fi

dnl Check for byte order
AC_C_BIGENDIAN([ dnl Big endian
	AC_DEFINE([TIARY_BIG_ENDIAN],[1],[Define to 1 if big endian])
],[ dnl Little endian
],[ dnl Unknown
	AC_MSG_ERROR([Funny byte order.])
],[ dnl "Universal"
	AC_MSG_ERROR([Funny byte order.])
])

dnl Check for libbz2 (Required)
AC_CHECK_LIB([bz2], [BZ2_bzBuffToBuffCompress], [LIBS="$LIBS -lbz2"],
			 [AC_MSG_ERROR([*** libbz2 is required])])

dnl Check for libncursesw (Required)
AC_CHECK_HEADER([ncursesw/curses.h],[],
				[AC_MSG_ERROR([*** ncursesw/curses.h not found])])
AC_CHECK_LIB([ncursesw], [waddwstr], [LIBS="$LIBS -lncursesw"],
			 [AC_MSG_ERROR([*** libncursesw not found])])

dnl Check for set_escdelay in libncursesw (Optional)
AC_CHECK_FUNCS([set_escdelay])

dnl Check for mouse support in libncursesw (Optional)
AC_CHECK_FUNCS([mousemask], [AC_DEFINE([TIARY_USE_MOUSE], [1], [Define to 1 if mouse support is available])])

dnl Check for libxml2 (Required)
PKG_CHECK_MODULES([libxml2], [libxml-2.0 >= 2.6.0],[
					CXXFLAGS="$CXXFLAGS $pkg_cv_libxml2_CFLAGS"
					LIBS="$LIBS $pkg_cv_libxml2_LIBS"
					],[AC_MSG_ERROR([*** libxml2 >= 2.7.0 is required.])])

dnl Check for pcre (Optional)
AC_CHECK_HEADER([pcre.h],
				[AC_CHECK_LIB([pcre], [pcre_compile2],
							  [LIBS="$LIBS -lpcre"
							   AC_DEFINE([TIARY_USE_PCRE],[1],[Define to 1 if pcre is available])])])

dnl Check for wcwidth function (Optional)
AC_CHECK_FUNCS([wcwidth])

dnl Check for optional libc functions ***at
AC_CHECK_FUNCS([openat fstatat])
if test x"$ac_cv_func_openat" != xyes || test x"$ac_cv_func_fstatat" != xyes; then
	AC_DEFINE([TIARY_HAVE_AT_FILE],[1],[Define if it appears **at functions are supported.])
fi

dnl Check for a bunch of optional libc functions
AC_CHECK_FUNCS([faccessat euidaccess eaccess mempcpy])
AC_CHECK_FUNCS([stpcpy wcpcpy strchrnul get_current_dir_name])

dnl Check for optional unlocked FILE operations
m4_foreach_w([f],[
	getc getchar putc putchar clearerr feof ferror
	fileno fflush fgetc fputc fread fwrite fgets fputs
	fprintf vfprintf
],[
	AC_CHECK_FUNCS(f[_unlocked],[],[
		AC_DEFINE(f[_unlocked],f,[Define to ]f[ if ]f[_unlocked is unavailable])
	])
])

dnl Check for sizeof(wchar_t) (Must be 4)
AC_CHECK_SIZEOF([wchar_t],[],[#include <wchar.h>])
if ! test x"$ac_cv_sizeof_wchar_t" = x4; then
	AC_MSG_ERROR([*** sizeof(wchar_t) must be 4!])
fi

dnl Check for required header stdint.h
AC_CHECK_HEADER([stdint.h])
if test -z "$ac_cv_header_stdint_h"; then
	AC_MSG_ERROR([*** <stdint.h> missing on your system.])
fi

dnl Add several useful flags if supported by compiler
AX_APPEND_COMPILE_FLAGS([-Wall -fmerge-all-constants -fno-threadsafe-statics -fno-exceptions])
AX_APPEND_LINK_FLAGS([-Wl,-O1 -Wl,--as-needed])

dnl Check whether the compiler has some support for C++14
AX_CHECK_COMPILE_FLAG([-std=gnu++14],[
	have_some_cxx0x=yes
	AX_APPEND_FLAG([-std=gnu++14])
],[
	AC_MSG_ERROR(["-std=gnu++14" is not supported. You need a modern compiler.])
])

dnl Now we assume these C++14 features are always present
AC_DEFINE([TIARY_HAVE_RVALUE_REFERENCES], [1], [Always define to 1 since we now assume C++14 support])
AC_DEFINE([TIARY_HAVE_DEFAULT_DELETE_FUNCTIONS], [1], [Always define to 1 since we now assume C++14 support])
AC_DEFINE([TIARY_HAVE_INITIALIZER_LIST], [1], [Always define to 1 since we now assume C++14 support])
AC_DEFINE([TIARY_HAVE_TUPLES], [1], [Always define to 1 since we now assume C++14 support])
AC_DEFINE([TIARY_HAVE_DECLTYPE], [1], [Always define to 1 since we now assume C++14 support])

dnl Let's always include config.h
CFLAGS="$CFLAGS -include config.h"
CXXFLAGS="$CXXFLAGS -include config.h"

AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/common/Makefile
	src/diary/Makefile
	src/main/Makefile
	src/ui/Makefile
])
AC_OUTPUT
