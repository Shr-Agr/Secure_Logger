
#include "security.h"

int main()
{
    // Initialize the sodium library
    if (sodium_init() < 0)
    {
        cerr << "Failed to initialize sodium" << endl;
        return 1;
    }

    SecureLogger logger;
    string token = "Break the system ";
    const char *to_be_encrypted = "The plaintext to be encrypted";
    const unsigned char *plaintext = reinterpret_cast<const unsigned char *>(to_be_encrypted);
    // cout << "plaintext size: " << strlen((const char *)plaintext) << endl;
    string filename = "test1.txt";

    int status = logger.init(token, filename);

    if (status == 0)
    {
        cout << "The logfile exists from before and the token is correct" << endl;
    }
    else if(status == 2){
        cout<<"The file does not exist from before"<<endl ; 
    }
    else if (status == -1)
    {
        cout << "Token not verfified for file: " << filename << endl;
        return 1;
    }
    else
    {
        cout << "Some error occured during the verification" << endl;
        return 1;
    }

    // Encrypt the plaintext
    try
    {
        cout << "Encrypting plaintext....." << endl;
        logger.encrypt_log_plaintext(plaintext);
        cout << "Encryption of plaintext successful" << endl;
    }
    catch (const runtime_error &e)
    {
        cerr << "Encryption of plaintext error: " << e.what() << endl;
        return 1; // Return an error code
    }

    // Decrypt the encrypted text from the file
    try
    {
        cout << "Decrypting file....." << endl;
        unsigned char *data = logger.decrypt_log();
        cout << "Decrypted Data: " << reinterpret_cast<const char *>(data) << endl;
        // cout << "decrypted size: " << strlen((char *)data) << endl;
        cout << "Decryption of file successful" << endl;
    }
    catch (const runtime_error &e)
    {
        cerr << "Decryption of file error: " << e.what() << endl;
        return 1; // Return an error code
    }

    return 0; // Successful completion
}