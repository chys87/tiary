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

#include "common/aes.h"
#include "common/digest.h"
#include <string.h>
#include <memory>
#include <openssl/evp.h>
#include <openssl/rand.h>

// Reference:
// https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption

namespace tiary {
namespace {

const unsigned char kKeyGenSalt[] = "TIARYEVPAESKEYGENSALT";

} // namespace

EvpAesKey evp_aes_key_gen(std::string_view password) {
	EvpAesKey res;
	memcpy(res.data(), SHA512(password)(kKeyGenSalt, sizeof(kKeyGenSalt)).result().data(), res.size());
	return res;
}

EvpAesIV evp_aes_iv_gen() {
	EvpAesIV res;
	RAND_bytes(res.data(), res.size());
	return res;
}

std::string evp_aes_encrypt(std::string_view plaintext, std::string_view password) {
	EvpAesKey key = evp_aes_key_gen(password);
	EvpAesIV iv = evp_aes_iv_gen();

	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (ctx == nullptr)
		return {};

	int ret = EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data());
	if (ret != 1) {
		EVP_CIPHER_CTX_free(ctx);
		return {};
	}

	int block_size = EVP_CIPHER_CTX_block_size(ctx);
	std::string ciphertext(iv.size() + plaintext.size() + block_size, '\0');

	memcpy(&ciphertext[0], iv.data(), iv.size());
	int ciphertext_len = iv.size();

	int len;

	ret = EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(&ciphertext[ciphertext_len]), &len,
			reinterpret_cast<const unsigned char *>(plaintext.data()), plaintext.size());
	if (ret != 1) {
		EVP_CIPHER_CTX_free(ctx);
		return {};
	}
	ciphertext_len += len;

	ret = EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(&ciphertext[ciphertext_len]), &len);
	if (ret != 1) {
		EVP_CIPHER_CTX_free(ctx);
		return {};
	}
	ciphertext_len += len;

	EVP_CIPHER_CTX_free(ctx);

	ciphertext.resize(ciphertext_len);
	return ciphertext;
}

std::string evp_aes_decrypt(std::string_view ciphertext, std::string_view password) {
	EvpAesKey key = evp_aes_key_gen(password);
	size_t iv_len = EvpAesIV().size();
	if (ciphertext.size() < iv_len) {
		return {};
	}

	std::string_view real_ciphertext = {ciphertext.data() + iv_len, ciphertext.size() - iv_len};

	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (ctx == nullptr) {
		return {};
	}

	int ret = EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(),
			reinterpret_cast<const unsigned char *>(ciphertext.data()));
	if (ret != 1) {
		EVP_CIPHER_CTX_free(ctx);
		return {};
	}

	int block_size = EVP_CIPHER_CTX_block_size(ctx);
	std::string plaintext(real_ciphertext.size() + block_size, '\0');
	int len;

	ret = EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(&plaintext[0]), &len,
			reinterpret_cast<const unsigned char *>(real_ciphertext.data()), real_ciphertext.size());
	if (ret != 1) {
		EVP_CIPHER_CTX_free(ctx);
		return {};
	}
	int plaintext_len = len;

	ret = EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(&plaintext[plaintext_len]), &len);
	if (ret != 1) {
		EVP_CIPHER_CTX_free(ctx);
		return {};
	}
	plaintext_len += len;

	EVP_CIPHER_CTX_free(ctx);

	plaintext.resize(plaintext_len);
	return plaintext;
}


} // namespace tiary
