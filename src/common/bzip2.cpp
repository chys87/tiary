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


#include "common/bzip2.h"
#include <bzlib.h>

namespace tiary {

BZip2Result bunzip2 (const void *data, size_t len)
{
	BZip2Result ret;
	int bzret;

	bz_stream stream = {}; // Initialize with all zeroes
	if (BZ2_bzDecompressInit (&stream, 0, 0) == BZ_OK) {
		size_t size_used = 0;
		size_t size_guess = len*2;
		ret.resize (size_guess);
		stream.next_in = (char *)data;
		stream.avail_in = len;
		while (
				stream.next_out = &ret[size_used],
				stream.avail_out = size_guess - size_used,
				(bzret = BZ2_bzDecompress (&stream)) == BZ_OK
				) {
			size_used = stream.total_out_lo32; // Simply ignore high bits, each on 64-bit systems
			size_guess *= 2;
			ret.resize (size_guess);
		}
		ret.resize (bzret == BZ_STREAM_END ? // Success?
				stream.total_out_lo32 :
				0);
		BZ2_bzDecompressEnd (&stream);
	}
	return ret;
}

BZip2Result bzip2 (const void *data, size_t len)
{
	unsigned destlen = len/64 + len + 650; // ref to libbzip2's documentation

	BZip2Result ret (destlen);

	unsigned newsize;

	if (BZ2_bzBuffToBuffCompress (&ret[0], &destlen, (char *)data, len, 9, 0, 0) == BZ_OK)
		newsize = destlen;
	else
		newsize = 0;

	ret.resize (newsize);
	return ret;
}

} // namespace tiary
