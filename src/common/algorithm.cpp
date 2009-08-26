// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the so-called 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#include "common/algorithm.h"
#include "common/types.h"
#include <algorithm>
#include <functional>
#include <string.h> // memcpy
#include <assert.h>

namespace tiary {

namespace {

struct UIntInd
{
	uint64_t val;
	unsigned *ptr;
};

bool operator > (const UIntInd &a, const UIntInd &b)
{
	return (a.val > b.val);
}

} // anonymous namespace

void min_max_programming (unsigned *result, const unsigned *min, const unsigned *max, unsigned n, unsigned S)
{
	/*
	 * Although it seems everything here is in unsinged, we allow sum (max, n) to be larger than UINT_MAX
	 * Actually this happens in our program.
	 */
	if (n == 0)
		return;

	unsigned minsum = 0;
	uint64_t maxsum = 0;
	for (unsigned i=0; i<n; ++i) {
		minsum += min[i];
		maxsum += max[i];
	}

	if (minsum >= maxsum || S >= maxsum) {
		memcpy (result, max, n * sizeof (unsigned));
		return;
	}
	uint64_t diffsum = maxsum - minsum;
	if (minsum >= S) {
		memcpy (result, min, n * sizeof (unsigned));
		return;
	}
	unsigned left = S -= minsum;

	// Now use "Largest remainder method"
	UIntInd *rem = new UIntInd [n];
	for (unsigned i=0; i<n; ++i) {
		unsigned quotient = unsigned (left * uint64_t (max[i] - min[i]) / diffsum);
		uint64_t remainder = left * uint64_t (max[i] - min[i]) % diffsum;
		S -= quotient;
		result[i] = min[i] + quotient;
		rem[i].val = remainder;
		rem[i].ptr = &result[i];
	}
	assert (S < n);
	UIntInd *first = &rem[0];
	UIntInd *middle = &rem[S];
	UIntInd *last = &rem[n];
	std::partial_sort (first, middle, last, std::greater<UIntInd>());
	for (UIntInd *p=first; p!=middle; ++p)
		++*p->ptr;
	delete [] rem;
}







} // namespace tiary
