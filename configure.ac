dnl /***********************************************************************
dnl  *
dnl  * Tiary, a terminal-based diary keeping system for Unix-like systems
dnl  * Copyright (C) 2009, 2010, 2011, 2012, chys <admin@CHYS.INFO>
dnl  *
dnl  * This software is licensed under the 3-clause BSD license.
dnl  * See LICENSE in the source package and/or online info for details.
dnl  *
dnl  **********************************************************************/

dnl You will need a recent version of autoconf, automake and autoconf-archive
dnl to use this file.

AC_DEFUN([MAJOR_VERSION], [0])
AC_DEFUN([MINOR_VERSION], [3])
AC_DEFUN([PATCH_VERSION], [0])

AC_INIT([tiary],MAJOR_VERSION.MINOR_VERSION.PATCH_VERSION,[admin@CHYS.INFO])
AM_INIT_AUTOMAKE([foreign -Wall dist-bzip2 dist-lzma dist-xz])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

AC_LANG(C++)

dnl Define versions
AC_DEFINE([TIARY_MAJOR_VERSION],MAJOR_VERSION,[Version])
AC_DEFINE([TIARY_MINOR_VERSION],MINOR_VERSION,[Version])
AC_DEFINE([TIARY_PATCH_VERSION],PATCH_VERSION,[Version])
dnl Remember to change the years on ever new year!
AC_DEFINE([TIARY_COPYRIGHT_YEAR],["2009, 2010, 2011, 2012"],[Copyright year])
AC_DEFINE([TIARY_COPYRIGHT_YEAR_WIDE],[L"2009, 2010, 2011, 2012"],[Copyright year])

dnl Define some useful feature macros
AC_DEFINE([_GNU_SOURCE],[1],[Always defined])
AC_DEFINE([_ALL_SOURCE],[1],[Always defined])
AC_DEFINE([_FORTIFY_SOURCE],[2],[Always defined])
AC_DEFINE([_FILE_OFFSET_BITS],[64],[Always defined to 64])

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

dnl Check whether the compiler has some support for C++11
dnl Currently supports: GCC (>= gcc 4.3); ICC (>= icc 11)
AX_CHECK_COMPILE_FLAG([-std=gnu++11],[
	have_some_cxx0x=yes
	AX_APPEND_COMPILE_FLAGS([-std=gnu++11])
],[
	AX_CHECK_COMPILE_FLAG([-std=gnu++0x],[
		have_some_cxx0x=yes
		AX_APPEND_COMPILE_FLAGS([-std=gnu++0x])
	])
])

if test x"$have_some_cxx0x" = xyes; then

	dnl A macro to test for C++11/0x feature
	dnl $1 = description
	dnl $2 = macro name
	dnl $3 = program
	AC_DEFUN([TIARY_CHECK_CXX11_FEATURE],[
		AS_VAR_PUSHDEF([CACHEVAR],[tiary_cv_cxx0x_$1])
		AC_CACHE_CHECK([whether C++ compiler supports $1 in C++11/0x mode], CACHEVAR,[
			AC_LINK_IFELSE([$3],[
				AS_VAR_SET(CACHEVAR,[yes])
			],[
				AS_VAR_SET(CACHEVAR,[no])
			])
		])
		if test x"AS_VAR_GET(CACHEVAR)" = xyes; then
			AC_DEFINE([$2],[1],[Define to 1 if C++ compiler supports $1 in C++11/0x mode])
		fi
		AS_VAR_POPDEF([CACHEVAR])
	])

	dnl Rvalue references (>= gcc 4.3)
	TIARY_CHECK_CXX11_FEATURE([rvalue references],[TIARY_RVALUE_REFERENCES],[AC_LANG_PROGRAM([
			#include <utility>
			int my_move (int && y) { return std::move(y); }
			],[[
			return my_move (0);
		]])])

	dnl static_assert (>= gcc 4.3)
	TIARY_CHECK_CXX11_FEATURE([static assertions],[TIARY_HAVE_STATIC_ASSERT],[AC_LANG_PROGRAM([
			static_assert(1 == 1, "The impossible happens.");
			],[])])

	dnl default/delete functions (>= gcc 4.4)
	TIARY_CHECK_CXX11_FEATURE([default/delete functions],[TIARY_HAVE_DEFAULT_DELETE_FUNCTIONS],[AC_LANG_PROGRAM([
			struct A { A () = default; A (const A&) = delete; };
			],[])])

	dnl initializer list (>= gcc 4.4)
	TIARY_CHECK_CXX11_FEATURE([initializer list],[TIARY_HAVE_INITIALIZER_LIST],[AC_LANG_PROGRAM([
			#include <list>
			],[
			return (std::list<int>{2,3,4,5}.size() == 4) ? 0 : 1;
			])])

	dnl Variadic templates and tuples (>= gcc 4.3)
	TIARY_CHECK_CXX11_FEATURE([variadic templates and tuples],[TIARY_HAVE_TUPLES],[AC_LANG_PROGRAM([
		#include <tuple>
		using namespace std;
		template <typename... Args>
		size_t mysizeof (Args... args) { return sizeof...(Args); }
		template <typename... Args>
		typename tuple_element<0, tuple<Args...> >::type first (const tuple<Args...>&tp)
		{ return get<0>(tp); }
		],[
		return mysizeof() + first(make_tuple(0, "Hello", 3.2));
		])])

	dnl decltype (>= gcc 4.3)
	TIARY_CHECK_CXX11_FEATURE([decltype],[TIARY_HAVE_DECLTYPE],[AC_LANG_PROGRAM([
		#include <cstdlib>
		template <typename F> void test (F foo)
		{
			decltype (foo ()) ret = foo ();
			int y = ret;
		}],[
			test (std::rand);
		])])

fi

dnl Let's always include config.h
CFLAGS="$CFLAGS -include config.h"
CXXFLAGS="$CXXFLAGS -include config.h"

AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_OUTPUT
