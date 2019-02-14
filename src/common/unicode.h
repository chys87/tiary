// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2010, 2018, 2019, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_COMMON_UNICODE_H
#define TIARY_COMMON_UNICODE_H

/**
 * @file	common/unicode.h
 * @author	chys <admin@chys.info>
 * @brief	Headers for functions related to Unicode
 */

#include <stddef.h> // ::size_t
#include <string>
#include <string_view>
#include <wchar.h>

#define TIARY_WIDIFY(s) TIARY_WIDIFY_IMPL(s)
#define TIARY_WIDIFY_IMPL(s) L##s

namespace tiary {

/**
 * @brief	Decides how many bytes are needed to store the character in UTF-8.
 * @param	c	Input wide character
 * @result	Number of bytes need to represent the character in UTF-8. \n
 *			Possible results are from 1 to 4, inclusive. \n
 *			A return value of 0 indicates an error.
 */
unsigned utf8_len_by_wchar(char32_t c);
/**
 * @brief	Decides the length of the UTF-8 sequence, given the first byte.
 * @param	b	First byte of an UTF-8 sequence that represent one Unicode character
 * @result	Length of the UTF-8 sequence. \n
 *			Possible results are from 1 to 4, inclusive. \n
 *			A return value of 0 indicates an error.
 */
unsigned utf8_len_by_first_byte(unsigned char b);

/**
 * @brief	Converts a UTF-8 encoded string to a wide (Unicode) string
 * @param	str	The UTF-8 string to be converted
 * @param	substitute	A character to substitute invalid sequences in str. \n
 *			<code>L'\\0'</code> indicates that invalid sequences should simply be ignored.
 * @result	The converted wide (Unicode) string
 */
std::wstring utf8_to_wstring(std::string_view str, wchar_t substitute = L'?');

/**
 * @brief	Count the number of Unicode characters in a UTF-8 string
 * @param	str	Input UTF-8 string
 * @result	The number of Unicode characters.  The result is unspecified if @c str is invalid UTF-8
 */
size_t utf8_count_chars(std::string_view str);

/**
 * @brief	Converts one single wide (Unicode) character to UTF-8
 * @param	c	The character to be converted.
 * @param	dst	Points to a block of memory, which must be sufficient
 *			to store the converted UTF-8 sequence.
 * @result	Points to the byte following the last one in the converted sequence.
 */
char *wchar_to_utf8(char *dst, char32_t c);
/**
 * @brief	Converts a wide (Unicode) string to UTF-8
 * @param	src	The wide (Unicode) string to be converted.
 * @result	The converted UTF-8 string.
 */
std::string wstring_to_utf8(std::wstring_view src);
std::string wstring_to_utf8(std::u32string_view src);


/**
 * @brief	Converts a multi-byte string to wide (Unicode) string
 * @param	src	The multi-byte string encoded in current LC_CTYPE locale
 * @result	The converted wide (Unicode) string
 *
 * An empty string is returned if any error occurs.
 * (Many encodings are not as robust as UTF-8. Usually one error jungles all
 * remaining data. So we do not attempt recovering at all.)
 */
std::wstring mbs_to_wstring(std::string_view);

/**
 * @brief	Converts a wide (Unicode) string to multi-byte
 * @param	src	The wide (Unicode) string to be converted
 * @param	substitute character to replace nonrepresentable characters ('0' = ignore)
 * @result	The converted multi-byte string encoded in the current LC_CTYPE locale
 */
std::string wstring_to_mbs(std::wstring_view src, char substitute = '?');


/**
 * @brief	Returns the on-screen width of a character
 * @param	c	Input UTF-32 character
 * @result	2 or 1. For abnormal or nonprintable characters, returns 1.
 */
unsigned ucs_width(char32_t c);
/**
 * @brief	Returns the on-screen width of a wide (Unicode) string
 * @param	str	Input wide (Unicode) string
 * @result	The total on-screen width of str. \n
 *			Abnormal and nonprintable characters are counted as 1.
 */
unsigned ucs_width(std::wstring_view str);
/**
 * @brief	Returns the on-screen width of a wide (Unicode) string
 * @param	str	Input UTF-32 string
 * @result	The total on-screen width of str. \n
 *			Abnormal and nonprintable characters are counted as 1.
 */
unsigned ucs_width(std::u32string_view str);
/**
 * @brief	Returns the maximum number of characters to fit in the specified screen width
 * @param	str	The given wide (Unicode) string
 * @param	scrwid	The given screen width
 */
size_t max_chars_in_width(std::wstring_view str, unsigned scrwid);
size_t max_chars_in_width(std::u32string_view str, unsigned scrwid);

/**
 * @brief	Reverse the case of a Unicode character
 * @param	c	Input character to be converted.
 * @result	The converted  character
 *
 * An uppercase character is converted to the corresponding lowercase character;
 * A lowercase character is converted to the corresponding uppcase character;
 * Any other character is returned as is.
 */
char32_t ucs_reverse_case(char32_t c);

/**
 * @brief	Determines whether a character is an alphabetic character
 *
 * This function is different from <code>iswalpha</code> in the return type,
 * and in that CJK characters are excluded, and that
 * it is not influenced by the current locale.
 */
bool ucs_isalpha(char32_t) ATTRIBUTE_CONST;
/**
 * @brief	Determine whether a character is a CJK character
 */
bool ucs_iscjk(char32_t) ATTRIBUTE_CONST;
/**
 * @brief	Determines whether a character is an alphabetic or numeric character
 *
 * This function is different from <code>iswalnum</code> in the return type,
 * and in that CJK characters are excluded, and that
 * it is not influenced by the current locale.
 */
bool ucs_isalnum(char32_t) ATTRIBUTE_CONST;
/**
 * @brief	Determines whether it is appropriate to display a given character
 *			in the beginning of a line.
 *
 * Some characters, like a right bracket or a comma, should not be displayed in the
 * beginning of a line; some others, like a left bracket, should not be displayed
 * at the end of a line.
 *
 * Functions tiary::allow_line_beginning and tiary::allow_line_end determines
 * whether it is appropriate to display a given character in the beginning or
 * at the end of a line.
 */
bool allow_line_beginning(char32_t) ATTRIBUTE_CONST;
/**
 * @brief	Determines whether it is appropriate to display a given character
 *			in the beginning of a line.
 *
 * See also @c tiary::allow_line_beginning
 */
bool allow_line_end(char32_t) ATTRIBUTE_CONST;






} // namespace tiary

#endif // Include guard
