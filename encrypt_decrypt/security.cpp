
#include <sodium.h>
#include <vector>
#include <iostream>
#include <string>
#include <iomanip>
#include <cstring>
#include <fstream>
#include <stdexcept>

using namespace std;

class SecureLogger
{
private:
    unsigned char key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];
    unsigned char nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];
    const char *token;
    string filename;

    void print_hex(const unsigned char *data, unsigned long long len) const
    {
        for (unsigned long long i = 0; i < len; ++i)
        {
            cout << hex << setw(2) << setfill('0') << static_cast<int>(data[i]);
        }
        cout << endl;
    }

    void hash_and_verify_password()
    {
        cout << "Inside hash and verify password...." << endl;

        // Initialize sodium library
        if (sodium_init() < 0)
        {
            cerr << "Failed to initialize sodium" << endl;
            throw runtime_error("Failed to initialize sodium");
        }

        // // Lock memory to protect plaintext token from being swapped out during hashing
        // if (sodium_mlock(&token, sizeof(token)) != 0)
        // {
        //     cerr << "Failed to lock memory for token during hashing" << endl;
        //     throw runtime_error("Failed to lock memory for token during hashing");
        // }

        // Hash the token using crypto_pwhash_str() which includes opslimit, memlimit, and salt management
        char hashed_token[crypto_pwhash_STRBYTES]; // Buffer for the hashed token

        if (crypto_pwhash_str(hashed_token, token, strlen(token),
                              crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE) != 0)
        {
            cerr << "Token hashing failed!" << endl;
            // sodium_munlock(&token, sizeof(token)); // Unlock before exiting
            throw runtime_error("Token hashing failed");
        }

        cout << "Hashed token: " << hashed_token << endl;

        // Verifying the hashed token
        cout << "Verifying token..." << endl;

        // Verifying the hash
        if (crypto_pwhash_str_verify(hashed_token, token, strlen(token)) != 0)
        {
            cerr << "Token verification failed!" << endl;
            // sodium_munlock(&token, sizeof(token)); // Unlock before exiting
            throw runtime_error("Token verification failed");
        }

        cout << "Token verification successful!" << endl;

        // // Update the key
        // key = hashed_token;

        // // Unlock memory for plaintext token after verification
        // sodium_munlock(&token, sizeof(token));
    }

public:
    SecureLogger()
    {
        // Initialize nonce
        randombytes_buf(nonce, sizeof nonce);
    }

    void init(const string &init_token, const string &init_filename)
    {
        token = new char[init_token.size() + 1];
        strcpy(const_cast<char *>(token), init_token.c_str());
        filename = init_filename;
    }

    void encrypt_log(const unsigned char *plaintext, size_t plaintext_len)
    {
        if (plaintext_len == 0)
        {
            throw runtime_error("Cannot encrypt empty plaintext...");
        }

        // Generate encryption key from password
        hash_and_verify_password();

        unsigned char ciphertext[plaintext_len + crypto_aead_xchacha20poly1305_ietf_ABYTES];
        unsigned long long ciphertext_len;

        // Encrypt the plaintext
        if (crypto_aead_xchacha20poly1305_ietf_encrypt(ciphertext, &ciphertext_len, plaintext, plaintext_len,
                                                       NULL, 0, NULL, nonce, key) != 0)
        {
            throw runtime_error("Encryption failed");
        }

        cout << "Encrypted Data (Hex): ";
        print_hex(ciphertext, ciphertext_len);

        // Write nonce and ciphertext to the file
        ofstream out_file(filename, ios::binary);
        if (!out_file)
        {
            throw runtime_error("Failed to open output file for writing");
        }

        out_file.write(reinterpret_cast<const char *>(nonce), sizeof nonce);
        if (!out_file)
        {
            throw runtime_error("Failed to write nonce to file");
        }

        out_file.write(reinterpret_cast<const char *>(ciphertext), ciphertext_len);
        if (!out_file)
        {
            throw runtime_error("Failed to write ciphertext to file");
        }

        out_file.close();
        if (!out_file)
        {
            throw runtime_error("Failed to close output file");
        }

        cout << "Encrypted file written successfully" << endl;
    }

    void decrypt_log(size_t plaintext_len)
    {
        // Read the encrypted file
        ifstream myfile(filename, ios::binary);

        if (!myfile.is_open())
        {
            cerr << "Unable to open encrypted log file" << endl;
            throw runtime_error("Unable to open encrypted log file");
        }

        // Get the size of the file
        myfile.seekg(0, std::ios::end);
        std::streamsize file_size = myfile.tellg();
        myfile.seekg(0, std::ios::beg);

        if (file_size <= sizeof(nonce))
        {
            cerr << "File size is too small to contain both nonce and ciphertext" << endl;
            throw runtime_error("File size is too small to contain both nonce and ciphertext");
        }

        // Allocate a buffer to store the nonce and ciphertext
        vector<unsigned char> file_data(file_size);
        myfile.read(reinterpret_cast<char *>(file_data.data()), file_size);
        myfile.close();

        // Extract nonce and ciphertext
        copy(file_data.begin(), file_data.begin() + sizeof(nonce), nonce);
        vector<unsigned char> ciphertext(file_data.begin() + sizeof(nonce), file_data.end());

        // // Generate encryption key from password
        // hash_and_verify_password();

        // Decrypt the ciphertext
        unsigned char decrypted[plaintext_len];
        unsigned long long decrypted_len;

        if (crypto_aead_xchacha20poly1305_ietf_decrypt(decrypted, &decrypted_len,
                                                       NULL, ciphertext.data(), ciphertext.size(),
                                                       NULL, 0, nonce, key) != 0)
        {
            throw runtime_error("Decryption failed or integrity check failed");
        }

        // Print decrypted data
        cout << "Decrypted Data: ";
        cout.write(reinterpret_cast<char *>(decrypted), decrypted_len);
        cout << endl;
    }
};

int main()
{
    // Initialize the sodium library
    if (sodium_init() < 0)
    {
        cerr << "Failed to initialize sodium" << endl;
        return 1;
    }

    SecureLogger logger;
    string token = "hello";
    const unsigned char plaintext[] = "test";
    string filename = "test.txt";

    logger.init(token, filename);

    // Encrypt the file
    try
    {
        cout << "Encrypting....." << endl;
        logger.encrypt_log(plaintext, sizeof(plaintext) - 1);
        cout << "Encryption successful" << endl;
    }
    catch (const runtime_error &e)
    {
        cerr << "Encryption error: " << e.what() << endl;
        return 1; // Return an error code
    }

    // Decrypt the file
    try
    {
        cout << "Decrypting....." << endl;
        logger.decrypt_log(sizeof(plaintext) - 1);
        cout << "Decryption successful" << endl;
    }
    catch (const runtime_error &e)
    {
        cerr << "Decryption error: " << e.what() << endl;
        return 1; // Return an error code
    }

    return 0; // Successful completion
}
