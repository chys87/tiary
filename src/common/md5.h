// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009, 2018, chys <admin@CHYS.INFO>
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
 * @brief	Defines class MD5, wrapping MD5 routines from libcrypto (part of OpenSSL)
 */

#include <stddef.h> // ::size_t
#include <stdint.h>
#include <array>
#include <string>
#include <string_view>
#include <openssl/md5.h>

namespace tiary {


class MD5
{
public:
	MD5() {
		MD5_Init(&context_);
	}
	MD5(const void *data, size_t len) : MD5() {
		update(data, len);
	}
	explicit MD5(std::string_view s) : MD5() {
		update(s.data(), s.length());
	}

	void update(const void *data, size_t len) {
		MD5_Update(&context_, data, len);
	}

	MD5 &operator () (const void *data, size_t len) {
		update(data, len);
		return *this;
	}
	MD5 &operator()(std::string_view s) {
		update(s.data(), s.length());
		return *this;
	}

	std::array<unsigned char, MD5_DIGEST_LENGTH> result () {
		std::array<unsigned char, MD5_DIGEST_LENGTH> res;
		MD5_Final(res.data(), &context_);
		return res;
	}

	void result(void *buffer) {
		MD5_Final(static_cast<unsigned char *>(buffer), &context_);
	}

private:
	MD5_CTX context_;
};

} // namespace tiary

#endif // Include guard
