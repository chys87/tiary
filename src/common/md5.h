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
#include <string.h> // ::memcpy
#include <stddef.h> // ::size_t

namespace tiary {

/* Define the state of the MD5 Algorithm. */
struct MD5Context
{
	uint64_t count;		/* message length in bits */
	uint32_t abcd[4];	/* digest buffer */
	uint8_t buf[64];	/* accumulate block */
};

// C-style interfaces
void md5_init (MD5Context *pms) throw ();
void md5_append (MD5Context *pms, const void *data, size_t nbytes) throw ();
void md5_finish (MD5Context *pms) throw ();



class MD5
{
public:
	MD5 ()
	{
		md5_init (&context);
	}

	explicit MD5 (const std::string &s)
	{
		md5_init (&context);
		md5_append (&context, s.data (), s.length ());
	}

	MD5 (const void *data, size_t len)
	{
		md5_init (&context);
		md5_append (&context, data, len);
	}

	MD5 &operator () (const void *data, size_t len)
	{
		md5_append (&context, data, len);
		return *this;
	}
	MD5 &operator () (const std::string &s)
	{
		return operator () (s.data (), s.length ());
	}

	/// Note that the pointed memory is invalidated after the class is destructed
	const void *result ()
	{
		md5_finish (&context);
		return context.abcd;
	}

	void result (void *buffer)
	{
		memcpy (buffer, result (), 16);
	}

private:
	MD5Context context;
};

} // namespace tiary

#endif // Include guard
