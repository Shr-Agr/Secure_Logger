#ifndef SECURE_LOGGER_H
#define SECURE_LOGGER_H

#include <sodium.h>
#include <vector>
#include <iostream>
#include <string>
#include <iomanip>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

class SecureLogger
{
private:
    unsigned char *salt;
    unsigned char *nonce;
    const char *token;
    const unsigned char *key;
    string filename;
    string metadata_filename = "logfile_metadata.json";

    void print_hex(const unsigned char *data, unsigned long long len);
    unsigned char *hash_token();
    void store_token(unsigned char *hashpassword, const string &filename, const string &metadata_filename);
    int verify_token();
    json get_logfile_metadata();
    string toHexString(const unsigned char *data, size_t length);
    unsigned char *fromHexString(const string &hexStr, size_t outLength);

public:
    int init(string token, string filename);
    void encrypt_log_plaintext(const unsigned char *plaintext);
    unsigned char *decrypt_log();
};

#endif // SECURE_LOGGER_H
