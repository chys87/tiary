dnl /***********************************************************************
dnl  *
dnl  * Tiary, a terminal-based diary keeping system for Unix-like systems
dnl  * Copyright (C) 2009-2023,
dnl  * chys <admin@CHYS.INFO>
dnl  *
dnl  * This software is licensed under the 3-clause BSD license.
dnl  * See LICENSE in the source package and/or online info for details.
dnl  *
dnl  **********************************************************************/

dnl You will need a recent version of autoconf, automake and autoconf-archive
dnl to use this file.

AC_DEFUN([MAJOR_VERSION], [0])
AC_DEFUN([MINOR_VERSION], [4])
AC_DEFUN([PATCH_VERSION], [0])

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
AC_DEFINE([TIARY_COPYRIGHT_YEAR],["2009-2023"],[Copyright year])
AC_DEFINE([TIARY_COPYRIGHT_YEAR_WIDE],[L"2009-2023"],[Copyright year])

dnl Define some useful feature macros
AC_USE_SYSTEM_EXTENSIONS
AH_BOTTOM([#include "config-common.h"])

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
PKG_CHECK_MODULES([ncursesw], [ncursesw >= 5.0], [], [AC_MSG_ERROR([*** ncursesw >= 5.0 is required.])])

dnl Check for set_escdelay and mouse support in libncursesw (Optional)
bak_CXXFLAGS="$CXXFLAGS"
bak_LIBS="$LIBS"
CXXFLAGS="$CXXFLAGS $pkg_cv_ncursesw_CFLAGS"
LIBS="$LIBS $pkg_cv_ncursesw_LIBS"
AC_CHECK_FUNCS([set_escdelay])
AC_CHECK_FUNCS([mousemask], [AC_DEFINE([TIARY_USE_MOUSE], [1], [Define to 1 if mouse support is available])])
CXXFLAGS="$bak_CXXFLAGS"
LIBS="$bak_LIBS"

dnl Check for libxml2 (Required)
PKG_CHECK_MODULES([libxml2], [libxml-2.0 >= 2.6.0],[],[AC_MSG_ERROR([*** libxml2 >= 2.7.0 is required.])])

dnl Check for RE2 (Optional)
PKG_CHECK_MODULES([re2], [re2 >= 0.0.0], [
					AC_DEFINE([TIARY_USE_RE2], [1], [Define to 1 if re2 is available])
					], [AC_MSG_WARN([*** re2 is unavailable.])])

dnl Check for libcrypto from OpenSSL (Required)
PKG_CHECK_MODULES([libcrypto], [libcrypto >= 1.0.2], [AC_SUBST(LIBS)], [AC_MSG_ERROR([*** libcrypto >= 1.0.2 is required.])])

CONF_CPPFLAGS="$CONF_CPPFLAGS $pkg_cv_ncursesw_CFLAGS $pkg_cv_libxml2_CFLAGS $pkg_cv_re2_CFLAGS $pkg_cv_libcrypto_CFLAGS"
CONF_LIBS="$CONF_LIBS $pkg_cv_ncursesw_LIBS $pkg_cv_libxml2_LIBS $pkg_cv_re2_LIBS $pkg_cv_libcrypto_LIBS"

dnl Check for required functions from libcrypto
bak_CXXFLAGS="$CXXFLAGS"
bak_LIBS="$LIBS"
CXXFLAGS="$CXXFLAGS $pkg_cv_libcrypto_CFLAGS"
LIBS="$LIBS $pkg_cv_libcrypto_LIBS"
AC_CHECK_FUNCS([MD5_Update SHA512_Update EVP_aes_256_cbc EVP_CIPHER_CTX_new], [],
				[AC_MSG_ERROR([*** MD5, SHA512, EVP-AES-256-CBC are required])])
CXXFLAGS="$bak_CXXFLAGS"
LIBS="$bak_LIBS"

dnl Check for wcwidth function (Optional)
AC_CHECK_FUNCS([wcwidth], [], [AC_MSG_ERROR([*** wcwidth not found])])

dnl Check for optional libc functions ***at
AC_CHECK_FUNCS([openat fstatat])

dnl Check for a bunch of optional libc functions
AC_CHECK_FUNCS([faccessat euidaccess eaccess mempcpy])
AC_CHECK_FUNCS([strchrnul wcschrnul get_current_dir_name])

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
AC_CHECK_SIZEOF([wchar_t],[],[])
if ! test x"$ac_cv_sizeof_wchar_t" = x4; then
	AC_MSG_ERROR([*** sizeof(wchar_t) must be 4!])
fi

dnl We plan to migrate to char32_t for Unicode strings
AC_CHECK_SIZEOF([char32_t], [], [])
if ! test x"$ac_cv_sizeof_char32_t" = x4; then
	AC_MSG_ERROR([*** char32_t type not found!])
fi

dnl Check for required header stdint.h
AC_CHECK_HEADER([stdint.h])
if test -z "$ac_cv_header_stdint_h"; then
	AC_MSG_ERROR([*** <stdint.h> missing on your system.])
fi

dnl Check for builtins
AC_MSG_CHECKING([if the compiler supports __builtin_bswap16])
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(, [[return __builtin_bswap16(0xabcd) == 0xcdab ? 0 : 1;]])],
	[
		AC_MSG_RESULT([yes]);
		AC_DEFINE([HAVE___BUILTIN_BSWAP16], [1], [Define to 1 if the compiler supports __builtin_bswap16])
], [AC_MSG_RESULT([no])])

AC_MSG_CHECKING([if the compiler supports __builtin_bswap32])
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(, [[return __builtin_bswap32(0xabcd1234) == 0x3412cdab ? 0 : 1;]])],
	[
		AC_MSG_RESULT([yes]);
		AC_DEFINE([HAVE___BUILTIN_BSWAP32], [1], [Define to 1 if the compiler supports __builtin_bswap32])
], [AC_MSG_RESULT([no])])

AC_MSG_CHECKING([if the compiler supports __builtin_bswap64])
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(, [[return __builtin_bswap64(0x12345678abcdabcdULL) == 0xcdabcdab78563412ULL ? 0 : 1;]])],
	[
		AC_MSG_RESULT([yes]);
		AC_DEFINE([HAVE___BUILTIN_BSWAP64], [1], [Define to 1 if the compiler supports __builtin_bswap64])
], [AC_MSG_RESULT([no])])

dnl Check for case ranges
AC_MSG_CHECKING([for case ranges (a GCC extension)])
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(, [[
		int a = 5;
		switch (a) {
			case 0 ... 3: return 1;
			case 4 ... 6: return 0;
			default: return 2;
		}
		return 3;
	]])],
	[
		AC_MSG_RESULT([yes]);
		AC_DEFINE([HAVE_CASE_RANGES], [1], [Define to 1 if the compiler supports case ranges (a GCC extension)])
], [AC_MSG_RESULT([no])])

dnl Check for GCC attributes
m4_foreach_w([f],[
	const pure
],[
	AX_GCC_FUNC_ATTRIBUTE(f)
])


dnl Add several useful flags if supported by compiler
AX_APPEND_COMPILE_FLAGS([-Wall -fmerge-all-constants -fno-threadsafe-statics -fno-exceptions -finput-charset=utf-8])
AX_APPEND_COMPILE_FLAGS([-Werror=return-type])
AX_APPEND_LINK_FLAGS([-Wl,-O1 -Wl,--as-needed])

dnl Check whether the compiler has some support for C++20 (prefer C++23/2b)
AX_CHECK_COMPILE_FLAG([-std=gnu++2b],[
	AX_APPEND_FLAG([-std=gnu++2b])
], [
	AX_CHECK_COMPILE_FLAG([-std=gnu++20],[
		AX_APPEND_FLAG([-std=gnu++20])
	],[
		AC_MSG_ERROR(["-std=gnu++20" is not supported. You need a modern compiler.])
	])
])

dnl Let's always include tiary-config.h (renamed to avoid conflict with src/diary/config.h)
CONF_CPPFLAGS="$CONF_CPPFLAGS -I\$(top_builddir)/src -I\$(top_srcdir)/src -include tiary-config.h"
AC_SUBST([CONF_CPPFLAGS])
AC_SUBST([CONF_LIBS])

AC_CONFIG_HEADERS([src/tiary-config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/common/Makefile
	src/diary/Makefile
	src/main/Makefile
	src/ui/Makefile
	ut/Makefile
])
AC_OUTPUT
