// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_COMMON_MD5_H
#define TIARY_COMMON_MD5_H

/**
 * @file	common/md5.h
 * @author	chys <admin@chys.info>
 * @brief	Header for the simple implementation of MD5
 *
 * We use the implementation written by L. Peter Deutsch <ghost@aladdin.com>,
 * which is licensed under a BSD-like license.
 *
 * I personally prefer the implementation in GNU coreutils, but that's
 * licensed under GPL-3. I want to license Tiary under a BSD-like
 * license. (--chys)
 */

#include "common/types.h"
#include <string>
#include <stddef.h> // ::size_t

namespace tiary {

namespace detail {

/* Define the state of the MD5 Algorithm. */
struct md5_state_t
{
	uint64_t count;		/* message length in bits */
	uint32_t abcd[4];	/* digest buffer */
	uint8_t buf[64];	/* accumulate block */
};
} // namespace tiary::detail

class MD5
{
public:
	MD5 ();
	explicit MD5 (const std::string &);
	MD5 (const void *, size_t);

	MD5 &append (const std::string &s);
	MD5 &append (const void *, size_t);

	/// Note that the pointed memory is invalidated after the class is destructed
	const void *result ();

	void result (void *);

private:
	detail::md5_state_t context;
};

} // namespace tiary

#endif // Include guard
