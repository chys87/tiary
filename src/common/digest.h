// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2009-2023, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_COMMON_DIGEST_H
#define TIARY_COMMON_DIGEST_H

/**
 * @file	common/digest.h
 * @author	chys <admin@chys.info>
 * @brief	Wraps digest routines from libcrypto (part of OpenSSL)
 */

#include <stddef.h> // ::size_t
#include <stdint.h>
#include <array>
#include <string>
#include <string_view>
#include <openssl/sha.h>

namespace tiary {
namespace digest_detail {

struct SHA512Desc {
	using CTX = SHA512_CTX;
	static constexpr auto init = SHA512_Init;
	static constexpr auto update = SHA512_Update;
	static constexpr auto final = SHA512_Final;
	static constexpr auto DIGEST_LENGTH = SHA512_DIGEST_LENGTH;
};

template <typename Desc>
class Digest
{
public:
	Digest() {
		Desc::init(&context_);
	}
	Digest(const void *data, size_t len) : Digest() {
		update(data, len);
	}
	explicit Digest(std::string_view s) : Digest() {
		update(s.data(), s.length());
	}

	void update(const void *data, size_t len) {
		Desc::update(&context_, data, len);
	}

	Digest &operator () (const void *data, size_t len) {
		update(data, len);
		return *this;
	}
	Digest &operator()(std::string_view s) {
		update(s.data(), s.length());
		return *this;
	}

	std::array<unsigned char, Desc::DIGEST_LENGTH> result () {
		std::array<unsigned char, Desc::DIGEST_LENGTH> res;
		Desc::final(res.data(), &context_);
		return res;
	}

	void result(void *buffer) {
		Desc::final(static_cast<unsigned char *>(buffer), &context_);
	}

	static constexpr unsigned DIGEST_LENGTH = Desc::DIGEST_LENGTH;

private:
	typename Desc::CTX context_;
};

} // namespace digest_detail

using SHA512 = digest_detail::Digest<digest_detail::SHA512Desc>;

} // namespace tiary

#endif // Include guard
