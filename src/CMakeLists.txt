# Useful utilities
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckTypeSize)
INCLUDE(TestBigEndian)
INCLUDE(FindBZip2)
INCLUDE(FindLibXml2)
INCLUDE(CheckCXXCompilerFlag)
INCLUDE(CheckCXXSourceCompiles)

# Check for required wchar_t functions
CHECK_FUNCTION_EXISTS(wmemchr HAVE_WMEMCHR)
CHECK_FUNCTION_EXISTS(mbrtowc HAVE_MBRTOWC)
IF(NOT HAVE_WMEMCHR OR NOT HAVE_MBRTOWC)
	MESSAGE(FATAL_ERROR "Unicode support is missing in your system.")
ENDIF(NOT HAVE_WMEMCHR OR NOT HAVE_MBRTOWC)

# Check whether this is a big endian system
TEST_BIG_ENDIAN(BIG_ENDIAN)
IF(BIG_ENDIAN)
	ADD_DEFINITIONS("-DTIARY_BIG_ENDIAN=1")
ENDIF(BIG_ENDIAN)

# Check for required libbz2
IF(NOT BZIP2_FOUND)
	MESSAGE(FATAL_ERROR "libbz2 is required.")
ENDIF(NOT BZIP2_FOUND)
IF(NOT BZIP2_NEED_PREFIX)
	MESSAGE(FATAL_ERROR "Please upgrade your bzip2 version.")
ENDIF(NOT BZIP2_NEED_PREFIX)
ADD_DEFINITIONS("-I${BZIP2_INCLUDE_DIR} ${BZIP2_DEFINITIONS}")

# Check for required libncursesw
CHECK_INCLUDE_FILES("ncursesw/curses.h" HAVE_NCURSESW_HEADERS)
CHECK_LIBRARY_EXISTS(ncursesw waddwstr '' HAVE_LIBNCURSESW)
IF(NOT HAVE_NCURSESW_HEADERS OR NOT HAVE_LIBNCURSESW)
	MESSAGE(FATAL_ERROR "ncurses with Unicode support is required.")
ENDIF(NOT HAVE_NCURSESW_HEADERS OR NOT HAVE_LIBNCURSESW)

# Check for ESCDELAY in libncursesw (undocumented feature)
SET(CMAKE_REQUIRED_LIBRARIES -lncursesw)
CHECK_CXX_SOURCE_COMPILES(
	"#include <ncursesw/curses.h> \n
	int main () { ESCDELAY = 50; }" HAVE_ESCDELAY)
SET(CMAKE_REQUIRED_LIBRARIES)
IF(HAVE_ESCDELAY)
	ADD_DEFINITIONS("-DTIARY_HAVE_ESCDELAY=1")
ENDIF(HAVE_ESCDELAY)


# Check for optional mouse support in libncursesw
SET(CMAKE_REQUIRED_LIBRARIES -lncursesw)
CHECK_FUNCTION_EXISTS(mousemask HAVE_MOUSE_SUPPORT)
SET(CMAKE_REQUIRED_LIBRARIES)
IF(HAVE_MOUSE_SUPPORT)
	ADD_DEFINITIONS("-DTIARY_USE_MOUSE=1")
ELSE(HAVE_MOUSE_SUPPORT)
	MESSAGE(STATUS)
	MESSAGE(STATUS "Mouse support is disabled.")
	MESSAGE(STATUS)
ENDIF(HAVE_MOUSE_SUPPORT)

# Check for required libxml2
IF(NOT LIBXML2_FOUND)
	MESSAGE(FATAL_ERROR "libxml2 is required.")
ENDIF(NOT LIBXML2_FOUND)
ADD_DEFINITIONS("-I${LIBXML2_INCLUDE_DIR} ${LIBXML2_DEFINITIONS}")

# Check for optional libpcre
CHECK_INCLUDE_FILES("pcre.h" HAVE_PCRE_HEADER)
CHECK_LIBRARY_EXISTS(pcre pcre_compile2 '' HAVE_PCRE)
IF(HAVE_PCRE_HEADER AND HAVE_PCRE)
	ADD_DEFINITIONS("-DTIARY_USE_PCRE=1")
	SET(PCRE_LIBRARIES "-lpcre")
ELSE(HAVE_PCRE_HEADER AND HAVE_PCRE)
	MESSAGE(STATUS)
	MESSAGE(STATUS "Regular expression support is disabled.")
	MESSAGE(STATUS)
	SET(PCRE_LIBRARIES)
ENDIF(HAVE_PCRE_HEADER AND HAVE_PCRE)


# Check for optional libc function wcwidth
CHECK_FUNCTION_EXISTS(wcwidth HAVE_WCWIDTH)
IF(HAVE_WCWIDTH)
	ADD_DEFINITIONS("-DTIARY_HAVE_WCWIDTH=1")
ENDIF(HAVE_WCWIDTH)

# Check for optional libc function localtime_r and gmtime_r
CHECK_FUNCTION_EXISTS(localtime_r HAVE_LOCALTIME_R)
CHECK_FUNCTION_EXISTS(gmtime_r HAVE_GMTIME_R)
IF(HAVE_LOCALTIME_R AND HAVE_GMTIME_R)
	ADD_DEFINITIONS("-DTIARY_HAVE_LOCALTIME_R_AND_GMTIME_R=1")
ENDIF(HAVE_LOCALTIME_R AND HAVE_GMTIME_R)

# Check for optional libc functions ***at
CHECK_FUNCTION_EXISTS(openat HAVE_OPENAT)
CHECK_FUNCTION_EXISTS(fstatat HAVE_FSTATAT)
IF(HAVE_OPENAT AND HAVE_FSTATAT)
	ADD_DEFINITIONS("-DTIARY_HAVE_AT_FILE=1")
ENDIF(HAVE_OPENAT AND HAVE_FSTATAT)
CHECK_FUNCTION_EXISTS(faccessat HAVE_FACCESSAT)
IF(HAVE_FACCESSAT)
	ADD_DEFINITIONS("-DTIARY_HAVE_FACCESSAT=1")
ENDIF(HAVE_FACCESSAT)

# Check for optional libc function euidaccess/eaccess
CHECK_FUNCTION_EXISTS(euidaccess HAVE_EUIDACCESS)
CHECK_FUNCTION_EXISTS(eaccess HAVE_EACCESS)
IF(HAVE_EUIDACCESS)
	ADD_DEFINITIONS("-DTIARY_HAVE_EUIDACCESS=1")
ELSEIF(HAVE_EACCESS)
	ADD_DEFINITIONS("-DTIARY_HAVE_EUIDACCESS=1")
	ADD_DEFINITIONS("-Deuidaccess=eaccess")
ENDIF(HAVE_EUIDACCESS)

# Check for optional libc function mempcpy
CHECK_FUNCTION_EXISTS(mempcpy HAVE_MEMPCPY)
IF(HAVE_MEMPCPY)
	ADD_DEFINITIONS("-DTIARY_HAVE_MEMPCPY=1")
ENDIF(HAVE_MEMPCPY)

# Check for optional libc function stpcpy and wcpcpy
CHECK_FUNCTION_EXISTS(stpcpy HAVE_STPCPY)
CHECK_FUNCTION_EXISTS(wcpcpy HAVE_WCPCPY)
IF(HAVE_STPCPY AND HAVE_WCPCPY)
	ADD_DEFINITIONS("-DTIARY_HAVE_STPCPY=1")
ENDIF(HAVE_STPCPY AND HAVE_WCPCPY)

# Check for optional libc function rawmemchr
CHECK_FUNCTION_EXISTS(rawmemchr HAVE_RAWMEMCHR)
IF(HAVE_RAWMEMCHR)
	ADD_DEFINITIONS("-DTIARY_HAVE_RAWMEMCHR=1")
ENDIF(HAVE_RAWMEMCHR)

# Check for optional libc function strchrnul
CHECK_FUNCTION_EXISTS(strchrnul HAVE_STRCHRNUL)
IF(HAVE_STRCHRNUL)
	ADD_DEFINITIONS("-DTIARY_HAVE_STRCHRNUL=1")
ENDIF(HAVE_STRCHRNUL)

# Check for optional unlocked FILE operations
SET(UNLOCKED_STDIO
	getc getchar putc putchar clearerr feof ferror
	fileno fflush fgetc fputc fread fwrite fgets fputs
	fprintf vfprintf
)
FOREACH(FOO ${UNLOCKED_STDIO})
	CHECK_FUNCTION_EXISTS(${FOO}_unlocked HAVE_${FOO}_unlocked)
	IF(NOT HAVE_${FOO}_unlocked)
		ADD_DEFINITIONS("-D${FOO}_unlocked=${FOO}")
	ENDIF(NOT HAVE_${FOO}_unlocked)
ENDFOREACH(FOO)

# Check for optional "get_current_dir_name" function
CHECK_FUNCTION_EXISTS(get_current_dir_name HAVE_GET_CURRENT_DIR_NAME)
IF(HAVE_GET_CURRENT_DIR_NAME)
	ADD_DEFINITIONS("-DTIARY_HAVE_GET_CURRENT_DIR_NAME=1")
ENDIF(HAVE_GET_CURRENT_DIR_NAME)

# Check for optional "canonicalize_file_name" function
CHECK_FUNCTION_EXISTS(canonicalize_file_name HAVE_CANONICALIZE_FILE_NAME)
IF(HAVE_CANONICALIZE_FILE_NAME)
	ADD_DEFINITIONS("-DTIARY_HAVE_CANONICALIZE_FILE_NAME=1")
ENDIF(HAVE_CANONICALIZE_FILE_NAME)

# Check for and sizeof(int) (Must be 4)
CHECK_TYPE_SIZE(int SIZEOF_INT)
IF(NOT SIZEOF_INT STREQUAL 4)
	MESSAGE(FATAL_ERROR "This program cannot compile on a platform where sizeof(int) != 4")
ENDIF(NOT SIZEOF_INT STREQUAL 4)

# Check for sizeof(wchar_t) (Must be 4)
SET(CMAKE_EXTRA_INCLUDE_FILES "wchar.h")
CHECK_TYPE_SIZE(wchar_t SIZEOF_WCHAR_T)
SET(CMAKE_EXTRA_INCLUDE_FILES)
ADD_DEFINITIONS("-DTIARY_SIZEOF_WCHAR_T=${SIZEOF_WCHAR_T}")
IF(NOT SIZEOF_WCHAR_T STREQUAL 4)
	MESSAGE(FATAL_ERROR "This program cannot copmile on a platform where
	sizeof(wchar_t) != 4")
ENDIF(NOT SIZEOF_WCHAR_T STREQUAL 4)

# Check for size of pointer
CHECK_TYPE_SIZE("void *" SIZEOF_POINTER)
ADD_DEFINITIONS("-DTIARY_SIZEOF_POINTER=${SIZEOF_POINTER}")

# Check how to define a 64-bit integer
CHECK_TYPE_SIZE("long" SIZEOF_LONG)
CHECK_TYPE_SIZE("long long" SIZEOF_LONG_LONG)
CHECK_TYPE_SIZE("__int64" SIZEOF___INT64)
IF(SIZEOF_LONG STREQUAL 8)
	ADD_DEFINITIONS("-DTIARY_INT64_TYPE=long")
ELSEIF(SIZEOF_LONG_LONG STREQUAL 8)
	ADD_DEFINITIONS("-DTIARY_INT64_TYPE='long long'")
ELSEIF(SIZEOF___INT64 STREQUAL 8)
	ADD_DEFINITIONS("-DTIARY_INT64_TYPE=__int64")
ELSE(SIZEOF_LONG STREQUAL 8)
	MESSAGE(FATAL_ERROR "I do not know how to define a 64-bit integer.")
ENDIF(SIZEOF_LONG STREQUAL 8)

# Check for optional stdint.h
CHECK_INCLUDE_FILES("stdint.h" HAVE_STDINT_H)
IF(HAVE_STDINT_H)
	ADD_DEFINITIONS("-DTIARY_HAVE_STDINT_H")
ENDIF(HAVE_STDINT_H)

# Add "-Wall" to make useful warnings
CHECK_CXX_COMPILER_FLAG("-Wall" HAVE_CXX_WALL)
IF(HAVE_CXX_WALL)
	ADD_DEFINITIONS("-Wall")
ENDIF(HAVE_CXX_WALL)

# Add "-ffast-math" if supported
CHECK_CXX_COMPILER_FLAG("-ffast-math" HAVE_FAST_MATH)
IF(HAVE_FAST_MATH)
	ADD_DEFINITIONS("-ffast-math")
ENDIF(HAVE_FAST_MATH)

# Add "-fno-math-errno" if supported
CHECK_CXX_COMPILER_FLAG("-fno-math-errno" HAVE_NO_MATH_ERRNO)
IF(HAVE_NO_MATH_ERRNO)
	ADD_DEFINITIONS("-fno-math-errno")
ENDIF(HAVE_NO_MATH_ERRNO)

# Add options additional flags that will reduce the 
# code size and/or improve efficiency
CHECK_CXX_COMPILER_FLAG("-fno-threadsafe-statics" HAVE_NO_THREADSAFE_STATICS)
IF(HAVE_NO_THREADSAFE_STATICS)
	ADD_DEFINITIONS("-fno-threadsafe-statics")
ENDIF(HAVE_NO_THREADSAFE_STATICS)
CHECK_CXX_COMPILER_FLAG("-fno-exceptions" HAVE_NO_EXCEPTIONS)
IF(HAVE_NO_EXCEPTIONS)
	ADD_DEFINITIONS("-fno-exceptions")
ENDIF(HAVE_NO_EXCEPTIONS)


# Check whether the compiler has some support for C++0x
# Currently supports: GCC (>= gcc 4.3); ICC (>= icc 11)
CHECK_CXX_COMPILER_FLAG("-std=gnu++0x" HAVE_SOME_CXX0X)
IF(HAVE_SOME_CXX0X)
	ADD_DEFINITIONS("-std=gnu++0x")
	SET(CMAKE_REQUIRED_FLAGS "-std=gnu++0x")
ENDIF(HAVE_SOME_CXX0X)

# Rvalue references (>= gcc 4.3)
CHECK_CXX_SOURCE_COMPILES("
	#include <algorithm>\n
	int my_move (int && y) { return std::move(y); }\n
	int main () { return my_move (0); }"
	HAVE_RVALUE_REFERENCES)
IF(HAVE_RVALUE_REFERENCES)
	ADD_DEFINITIONS("-DTIARY_HAVE_RVALUE_REFERENCES=1")
ENDIF (HAVE_RVALUE_REFERENCES)

# static_assert (>= gcc 4.3)
CHECK_CXX_SOURCE_COMPILES("static_assert (1 == 1, \"1 != 1\");\n int main () { return 0; }" HAVE_STATIC_ASSERT)
IF(HAVE_STATIC_ASSERT)
	ADD_DEFINITIONS("-DTIARY_HAVE_STATIC_ASSERT=1")
ENDIF(HAVE_STATIC_ASSERT)

# default/delete functions (>= gcc 4.4)
CHECK_CXX_SOURCE_COMPILES("
	struct A { A () = default; A (const A&) = delete; };
	int main () { return 0; }" HAVE_DEFAULT_DELETE_FUNCTIONS)
IF(HAVE_DEFAULT_DELETE_FUNCTIONS)
	ADD_DEFINITIONS("-DTIARY_HAVE_DEFAULT_DELETE_FUNCTIONS=1")
ENDIF(HAVE_DEFAULT_DELETE_FUNCTIONS)

# unordered_set/unordered_map (>= gcc 4.3)
CHECK_CXX_SOURCE_COMPILES("
	#include <unordered_set> \n
	#include <unordered_map> \n
	#include <string> \n
	#include <cstdlib> \n
	using namespace std;
	unordered_map <wstring, unordered_set <string> > map;
	int main ()
	{
		unordered_set <string> & set = map[L\"hello\"];
		set.insert (\"1\");
		set.insert (\"2\");
		const pair <const wstring, unordered_set <string> > &pr = *map.begin ();
		return 0;
	}
" HAVE_UNORDERED_SET_MAP)

# tr1/unordered_{set,map} (>= gcc 4)
CHECK_CXX_SOURCE_COMPILES("
	#include <tr1/unordered_set> \n
	#include <tr1/unordered_map> \n
	#include <string> \n
	#include <cstdlib> \n
	using namespace std;
	tr1::unordered_map <wstring, tr1::unordered_set <string> > map;
	int main ()
	{
		tr1::unordered_set <string> & set = map[L\"hello\"];
		set.insert (\"1\");
		set.insert (\"2\");
		const pair <const wstring, tr1::unordered_set <string> > &pr = *map.begin ();
		return 0;
	}
" HAVE_TR1_UNORDERED_SET_MAP)

# std::hash<const std::string &> ?
CHECK_CXX_SOURCE_COMPILES("
	#include <unordered_set> \n
	#include <string> \n
	using namespace std;
	int main ()
	{
		unordered_set<std::string, std::hash<const std::string &> > s1;
		unordered_set<std::wstring, std::hash<const std::wstring &> > s2;
		s1.insert (\"1\");
		s2.insert (L\"1\");
		return 0;
	}" HAVE_HASH_STRING_REF)

# hash<const std::tr1::string &> ?
# (Nonstandard but improves efficiency)
CHECK_CXX_SOURCE_COMPILES("
	#include <tr1/unordered_set> \n
	#include <string> \n
	using namespace std;
	int main ()
	{
		tr1::unordered_set<std::string, std::tr1::hash<const std::string &> > s1;
		tr1::unordered_set<std::wstring, std::tr1::hash<const std::wstring &> > s2;
		s1.insert (\"1\");
		s2.insert (L\"1\");
		return 0;
	}" HAVE_TR1_HASH_STRING_REF)

IF(HAVE_UNORDERED_SET_MAP)
	ADD_DEFINITIONS("-DTIARY_HAVE_STD_UNORDERED_SET_MAP=1")
	IF(HAVE_HASH_STRING_REF)
		ADD_DEFINITIONS("-DTIARY_HAVE_STD_HASH_STRING_REF=1")
	ENDIF(HAVE_HASH_STRING_REF)
ELSEIF(HAVE_TR1_UNORDERED_SET_MAP)
	ADD_DEFINITIONS("-DTIARY_HAVE_TR1_UNORDERED_SET_MAP=1")
	IF(HAVE_TR1_HASH_STRING_REF)
		ADD_DEFINITIONS("-DTIARY_HAVE_STD_TR1_HASH_STRING_REF=1")
	ENDIF(HAVE_TR1_HASH_STRING_REF)
ENDIF(HAVE_UNORDERED_SET_MAP)

# initializer list (>= gcc 4.4)
CHECK_CXX_SOURCE_COMPILES("
	#include <list> \n
	using namespace std;
	int main()
	{
		return (list<int>{2,3,4,5}.size() == 4) ? 0 : 1;
	}" HAVE_INITIALIZER_LIST)
IF(HAVE_INITIALIZER_LIST)
	ADD_DEFINITIONS("-DTIARY_HAVE_INITIALIZER_LIST=1")
ENDIF(HAVE_INITIALIZER_LIST)

# Variadic templates and tuples (>= gcc 4.3)
CHECK_CXX_SOURCE_COMPILES("#include <tuple> \n
	using namespace std;
	template <typename... Args>
	size_t mysizeof (Args... args)
	{
		return sizeof...(Args);
	}
	template <typename... Args>
	typename tuple_element<0, tuple<Args...> >::type first (tuple<Args...>&tp)
	{
		return tp.get<0>();
	}
	int main () {}" HAVE_TUPLES)
IF(HAVE_TUPLES)
	ADD_DEFINITIONS("-DTIARY_HAVE_TUPLES=1")
ENDIF(HAVE_TUPLES)

# decltype (>= gcc 4.3)
CHECK_CXX_SOURCE_COMPILES("#include <iostream> \n
#include <cstdlib> \n

template <typename F>
void test (F foo)
{
	decltype (foo ()) ret = foo ();
	std::cout << ret;
}

int main()
{
	test (std::rand);
}" HAVE_DECLTYPE)
IF(HAVE_DECLTYPE)
	ADD_DEFINITIONS("-DTIARY_HAVE_DECLTYPE=1")
ENDIF(HAVE_DECLTYPE)

SET(CMAKE_REQUIRED_FLAGS)
# END OF C++0X FEATURES




ADD_EXECUTABLE(tiary
	main.cpp
	)

# The order is important here
SET(TIARY_SUBDIRS
	main
	diary
	ui
	common
	)

SET(TIARY_SYSLIBS ncursesw ${BZIP2_LIBRARIES} ${LIBXML2_LIBRARIES}
	${PCRE_LIBRARIES})


FOREACH(DIR ${TIARY_SUBDIRS})
	ADD_SUBDIRECTORY(${DIR})
	ADD_DEPENDENCIES(tiary tiary_${DIR})
	TARGET_LINK_LIBRARIES(tiary tiary_${DIR})
ENDFOREACH(DIR)

TARGET_LINK_LIBRARIES(tiary ${TIARY_SYSLIBS})




# Test
#ADD_EXECUTABLE(test test.cpp)
#FOREACH(DIR ${TIARY_SUBDIRS})
#	ADD_DEPENDENCIES(test tiary_${DIR})
#	TARGET_LINK_LIBRARIES(test tiary_${DIR})
#ENDFOREACH(DIR)
#TARGET_LINK_LIBRARIES(test ${TIARY_SYSLIBS})
