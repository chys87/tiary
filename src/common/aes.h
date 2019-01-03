// -*- mode:c++; tab-width:4; -*-
// vim:ft=cpp ts=4

/***************************************************************************
 *
 * Tiary, a terminal-based diary keeping system for Unix-like systems
 * Copyright (C) 2018, chys <admin@CHYS.INFO>
 *
 * This software is licensed under the 3-clause BSD license.
 * See LICENSE in the source package and/or online info for details.
 *
 **************************************************************************/


#ifndef TIARY_COMMON_AES_H
#define TIARY_COMMON_AES_H

/**
 * @file	common/md5.h
 * @author	chys <admin@chys.info>
 * @brief	Wraps AES encryption and decryption from libcrypto (part of OpenSSL)
 */

#include <array>
#include <string_view>
#include <openssl/evp.h>

namespace tiary {

using EvpAesKey = std::array<unsigned char, 32>;
using EvpAesIV = std::array<unsigned char, 16>;

EvpAesKey evp_aes_key_gen(std::string_view password);
EvpAesIV evp_aes_iv_gen();

/**
 * Encrypt a sries of bytes using EVP-AES-256-CBC.
 * The IV is generated at random and stored in the first 16 bytes of the resulting
 * ciphertext.
 *
 * In case of an error, which is highly unlikely, an empty string is returned.
 */
std::string evp_aes_encrypt(std::string_view plaintext, std::string_view password);

/**
 * Decrypt a string encrypted with @c evp_aes_encrypt.
 *
 * In case of any error, an empty string is returned.
 */
std::string evp_aes_decrypt(std::string_view ciphertext, std::string_view password);

} // namespace tiary

#endif // Include guard
