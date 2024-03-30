#include "Crypto.h"

std::string Crypto::Hash_SHA256(const std::string plain_text, int iterations, std::string salt)
{
    unsigned char message = (unsigned char)(plain_text + salt).c_str();
    unsigned int message_len = (plain_text + salt).length();

    unsigned char* digest_buffer = new unsigned char[EVP_MD_size(EVP_sha256())];
    unsigned int digest_bbuffer_len = EVP_MD_size(EVP_sha256());

    unsigned char* digest;
    unsigned int digest_len = 0;

    EVP_MD_CTX* mdctx;

    if ((mdctx = EVP_MD_CTX_new()) == NULL)
        return "Error creating mdctx";

    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL))
        return "Error initializing mdctx";

    if (1 != EVP_DigestUpdate(mdctx, &message, message_len))
        return "Error updating mdctx";

    if ((digest = (unsigned char*)OPENSSL_malloc(EVP_MD_size(EVP_sha256()))) == NULL)
        return "Error in size of";

    if (1 != EVP_DigestFinal_ex(mdctx, digest, &digest_len))
        return "Error finalizing mdctx";

    EVP_MD_CTX_free(mdctx);

    for (size_t i = 1; i < iterations; i++)
    {
        memcpy(&digest_buffer[0], &digest[0], digest_len * sizeof(unsigned char));

        if (1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL))
            return "Error initializing mdctx";

        if (1 != EVP_DigestUpdate(mdctx, digest_buffer, digest_bbuffer_len))
            return "Error updating mdctx";

        if ((digest = (unsigned char*)OPENSSL_malloc(EVP_MD_size(EVP_sha256()))) == NULL)
            return "Error in size of";

        if (1 != EVP_DigestFinal_ex(mdctx, digest, &digest_len))
            return "Error finalizing mdctx";
    }

    delete[] digest_buffer;

    return charToHexString(digest, digest_len);
}

std::string Crypto::Hash2_SHA256(const std::string plain_text, int iterations, std::string salt)
{
    std::string theString = plain_text + salt;
    std::string shaString;

    unsigned char* digest_buffer = new unsigned char[EVP_MD_size(EVP_sha256())];
    unsigned int digest_bbuffer_len = EVP_MD_size(EVP_sha256());

    EVP_MD_CTX* context = EVP_MD_CTX_create();
    EVP_MD_CTX_init(context);
    if (!context)
    {
        return "error in context";
    }
    EVP_MD* type = (EVP_MD*)EVP_sha256();
    if (!EVP_DigestInit_ex(context, type, NULL))
    {
        return "error init";
    }

    if (!EVP_DigestUpdate(context, theString.c_str(), theString.length()))
    {
        return "error update";
    }
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    if (!EVP_DigestFinal_ex(context, hash, &lengthOfHash))
    {
        return "error final";
    }
    //EVP_MD_CTX_free(context);

    for (size_t i = 1; i < iterations; i++)
    {
        memcpy(&digest_buffer[0], &hash[0], digest_bbuffer_len * sizeof(unsigned char));

        if (!EVP_DigestInit_ex(context, type, NULL))
        {
            return "error init";
        }

        if (!EVP_DigestUpdate(context, digest_buffer, digest_bbuffer_len))
        {
            return "error update";
        }

        if (!EVP_DigestFinal_ex(context, hash, &lengthOfHash))
        {
            return "error final";
        }
    }
    delete[] digest_buffer;
    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    shaString = ss.str();

    EVP_MD_CTX_free(context);

    return charToHexString(hash, lengthOfHash);
}

void Crypto::PBKDF2_derive_key_iv(
    const std::string& pass, const std::string& salt,
    unsigned char* key, unsigned int key_len,
    unsigned char* iv, unsigned int iv_len)
{
    unsigned int digest_len = key_len + iv_len;
    unsigned char* digest = (unsigned char*)malloc(digest_len);
    PKCS5_PBKDF2_HMAC(
        pass.c_str(), pass.length(),
        (const unsigned char*)salt.c_str(), salt.length(),
        1000, EVP_sha256(), digest_len, digest
    );

    memcpy(&key[0], &digest[0], key_len * sizeof(unsigned char));
    memcpy(&iv[0], &digest[key_len], iv_len * sizeof(unsigned char));

    delete[] digest;
}



std::string Crypto::generateSalt()
{
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string salt = "";

    for (std::size_t i = 0; i < 8; ++i)
    {
        salt += CHARACTERS[distribution(generator)];
    }

    return salt;
}

std::string Crypto::encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* iv, unsigned char* ciphertext)
{
    EVP_CIPHER_CTX* ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        return "EVP_CIPHER_CTX_new Error";

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        return "EVP_EncryptInit_ex Error";

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        return "EVP_EncryptUpdate Error";
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        return "EVP_EncryptFinal_ex Error";
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return charToHexString(ciphertext, ciphertext_len);
}

std::string Crypto::decrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* key, unsigned char* iv, unsigned char* plaintext)
{
    EVP_CIPHER_CTX* ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        return "EVP_CIPHER_CTX_new Error";

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        return "EVP_DecryptInit_ex Error";

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        return "EVP_DecryptUpdate Error";
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        return "EVP_DecryptFinal_ex Error";
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return charToString(plaintext, plaintext_len);
}

std::string Crypto::charToHexString(const unsigned char* digest, unsigned int digest_len)
{
    std::stringstream ss;
    for (unsigned int i = 0; i < digest_len; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    }
    return ss.str();
}

std::string Crypto::charToString(const unsigned char* digest, unsigned int digest_len)
{
    std::stringstream ss;
    for (unsigned int i = 0; i < digest_len; ++i)
    {
        ss << (char)digest[i];
    }
    return ss.str();
}

unsigned char* Crypto::hexStringToChar(std::string hexString)
{
    hexString = stringWithSpaces(hexString);
    const char* chexString = hexString.c_str();
    char* p;
    int cnt = (strlen(chexString) + 1) / 3; // Whether or not there's a trailing space
    unsigned char* result = (unsigned char*)malloc(cnt);
    unsigned int c;

    for (int i = 0; i < hexString.length(); i += 3) {
        p = (char*)chexString + i;
        if (sscanf_s(p, "%02X", &c) != 1) {
            break; // Didn't parse as expected
        }
        if (((i) / 3) < cnt)
        {
            result[(i) / 3] = (unsigned char)c;
        }
    }

    return result;
}

std::string Crypto::stringWithSpaces(std::string text)
{
    std::string result = "";
    for (size_t i = 0; i < text.length(); i++)
    {
        if (i == 0)
        {
            result += text[i];
        }
        else if (i % 2 == 0)
        {
            result += " ";
            result += text[i];
        }
        else
        {
            result += text[i];
        }
    }
    std::transform(result.begin(), result.end(), result.begin(), std::toupper);
    return result;
}

