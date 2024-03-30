#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <sha.h>
#include <evp.h>
#include <openssl/kdf.h>
#include <iostream>
#include <random>
#include <kdf.h>

namespace Crypto
{
    std::string Hash_SHA256(const std::string plain_text, int iterations, std::string salt);
    std::string Hash2_SHA256(const std::string plain_text, int iterations, std::string salt);
    void PBKDF2_derive_key_iv(const std::string& pass, const std::string& salt, unsigned char* key, unsigned int key_len, unsigned char* iv, unsigned int iv_len);
    std::string generateSalt();

    std::string encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* iv, unsigned char* ciphertext);
    std::string decrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* key, unsigned char* iv, unsigned char* plaintext);

    std::string charToHexString(const unsigned char* digest, unsigned int digest_len);
    std::string charToString(const unsigned char* digest, unsigned int digest_len);
    unsigned char* hexStringToChar(std::string hexString);
    std::string stringWithSpaces(std::string text);
};
