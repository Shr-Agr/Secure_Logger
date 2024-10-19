
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>  // For parsing the log entry
#include "parseAppend.h"  // Include the input parsing module
#include<unordered_map>
#include<map> 
#include "security.h"  // Include the security file
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include<vector> 
using namespace std;

// Struct to store the parsed data of the last activity
struct Activity {
    int T = -1 ;           // Timestamp
    string K = "";        // Employee key (name or ID)
    string E = "";        // Role or other identifier
    string G = "";        // Department or other identifier
    int R = -1;        // Additional field
    bool A_flag = false;     // Activity flag
    bool L_flag = false;     // Log flag
};


// Function to append ".log" if it's not already there
string ensure_log_extension(const char* logFileName) {
    string logName(logFileName);  // Convert the char* to a string for easier manipulation
    if (logName.size() < 4 || logName.substr(logName.size() - 4) != ".txt") {
        logName += ".txt";  // Add ".log" if not already present
    }
    return logName;
}


int read_last_timestamp(const char *decryptedDataChar) {
    // Convert the decryptedDataChar (const char*) to std::string for easier manipulation
    std::string decryptedData(decryptedDataChar);

    // If the decrypted log data is empty, return 0 as the last timestamp
    if (decryptedData.empty()) {
        return 0;
    }

    // Split the decrypted data into lines
    std::istringstream iss(decryptedData);
    std::string line;
    std::string lastLine;

    // Traverse through the decrypted data line by line to find the last non-empty line
    while (std::getline(iss, line)) {
        if (!line.empty()) {
            lastLine = line;  // Update lastLine to the most recent non-empty line
        }
    }

    // If no valid lines are found, return 0
    if (lastLine.empty()) {
        return 0;
    }

    // Extract the timestamp from the last line
    int lastTimestamp = 0;
    std::stringstream ss(lastLine);
    std::string temp;

    // Assuming the timestamp is the second token in the line
    ss >> temp >> lastTimestamp;

    return lastTimestamp;
}



// Function to parse a log entry into the LastActivity struct
Activity parse_log_entry(const string& logEntry) {
    Activity activity;
    stringstream ss(logEntry);
    string temp;

    // Parse the general fields
    ss >> temp >> activity.T    // "T:"
       >> temp >> activity.K    // "K:"
       >> temp >> activity.E    // "E:"
       >> temp >> activity.G    // "G:"
       >> temp >> activity.R;   // "R:"

    // Parse "A_flag:" and "L_flag:" properly
    ss >> temp; // "A_flag:"
    if (temp == "A_flag:") {
        string aFlagValue;
        ss >> aFlagValue;
        activity.A_flag = (aFlagValue == "true");
    }

    ss >> temp; // "L_flag:"
    if (temp == "L_flag:") {
        string lFlagValue;
        ss >> lFlagValue;
        activity.L_flag = (lFlagValue == "true");
    }

    return activity;
}

Activity givelastActivity(const char* decryptedText, ParsedData data) {
    // Convert decrypted unsigned char* data to a C++ string
    string decryptedString(decryptedText);
    // Split the decrypted text into individual log entries (lines)
    std::istringstream iss(decryptedString);
    std::vector<std::string> logEntries;
    std::string line;
    
    while (std::getline(iss, line)) {
        logEntries.push_back(line);
    }

    // Determine the person name (from E or G)
    std::string personName;
    if (data.E != nullptr) {
        personName = data.E;
    } else if (data.G != nullptr) {
        personName = data.G;
    }

    // Traverse the log entries from the last entry to the first
    Activity lastActivity;

    for (auto it = logEntries.rbegin(); it != logEntries.rend(); ++it) {
        Activity activity = parse_log_entry(*it);

        // Check if the activity matches the person name (Employee or Guest)
        if ((activity.E != "" && data.E != nullptr && activity.E == personName) || 
            (activity.G != "" && data.G != nullptr && activity.G == personName)) {
            lastActivity = activity;
            break;  // We found the last activity, no need to continue
        }
    }

    return lastActivity;
}

bool checks_on_sequence(Activity lastActivity, ParsedData data){    
    // no last activity, now entry on campus 
    if(!lastActivity.A_flag && !lastActivity.L_flag && data.A_flag && data.R == -1 ){
        return true ; 
    }
    // previous activity 
    // previous is campus arrival, now new entry is arrival in room 
    if(lastActivity.A_flag && lastActivity.R == -1 && data.A_flag && data.R != -1 ){
        return true ; 
    }
    // previous is room entry, current is room exit 
    else if(lastActivity.A_flag && lastActivity.R != -1 && data.L_flag && data.R != -1 ){
        return true ; 
    }
    // previous is room exit, current is campus departure 
    else if(lastActivity.L_flag && lastActivity.R != -1 && data.L_flag && data.R == -1){
        return true ; 
    }
    // previous is room exit, now going into new room 
    else if(lastActivity.L_flag && lastActivity.R != -1 && data.A_flag && data.R != -1){
        return true ; 
    }
    // no entry in the room, direct campus entry and departure 
    else if(lastActivity.A_flag && lastActivity.R == -1 && data.L_flag && data.R == -1){
        return true ; 
    }
    else {
        return false ; 
    }
}


 const char* combineStrings(const char* first, const char* second) {
    // Calculate the total length of the combined string
    size_t length = std::strlen(first) + std::strlen(second) + 2; // +2 for the newline and null terminator

    // Allocate memory for the combined string
    char* combined = new char[length];

    // Copy the first string and append the second string
    std::strcpy(combined, first);
    std::strcat(combined, "\n"); // Add a newline for better readability
    std::strcat(combined, second);

    // Return the combined string
    return combined;
}
const char* convertParsedDataToCStr(const ParsedData& data) {
    // Allocate a sufficiently large buffer to hold the final string
    // Adjust this size based on the expected maximum size of the formatted string
    char* buffer = new char[512];  // Dynamic memory allocation for a 512-byte buffer
    std::memset(buffer, 0, 512);   // Initialize the buffer with zeros

    // Start constructing the formatted string
    std::ostringstream oss;

    // Add each part, including default/null values
    oss << "T: " << (data.T != -1 ? std::to_string(data.T) : "null") << " "
        << "K: " << (data.K ? data.K : "null") << " "
        << "E: " << (data.E ? data.E : "null") << " "
        << "G: " << (data.G ? data.G : "null") << " "
        << "R: " << (data.R != -1 ? std::to_string(data.R) : "null") << " "
        << "A_flag: " << (data.A_flag ? "true" : "false") << " "
        << "L_flag: " << (data.L_flag ? "true" : "false");

    // Copy the formatted string into the buffer
    std::strncpy(buffer, oss.str().c_str(), 511);  // Copy to buffer, limit to 511 chars
    buffer[511] = '\0';  // Ensure null termination

    // Return the final C-style string
    return buffer;
}



int main(int argc, char* argv[]) {
    // Call the input parsing function (not implemented in this snippet)
    ParsedData data;
    bool batchMode = false;  // Flag to check if we're in batch mode
    string batchFileName;

    // Check if the -B flag is provided
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "-B" && i + 1 < argc) {
            batchMode = true;
            batchFileName = argv[++i];
            break;
        }
    }

    if (batchMode) {
        // Open the batch file
        //cout<<"yes got it"<<endl ; 
        ifstream batchFile(batchFileName);
        if (!batchFile.is_open()) {
            cerr << "Error: Could not open batch file." << endl;
            return 1;
        }

        string line;
        int lineNumber = 0;

        // Process each line in the batch file
        while (getline(batchFile, line)) {
            ++lineNumber;
            // Parse the line as input
            char* lineArgs[100];  // Arbitrary large array to hold arguments
            int argCount = 0;

            // Split the line into arguments (this function needs to be implemented)
            istringstream iss(line);
            string token;
            const char* logFilePath = "random/path/"; 
            lineArgs[argCount++] = strdup(logFilePath); 
            while (iss >> token) {
                lineArgs[argCount++] = strdup(token.c_str());
            }
            
            //cout<<*lineArgs<<endl ; 
            // cout << "Line " << lineNumber << ": Number of arguments (argc): " << argCount << endl;

            // // Also, print the arguments if needed for further debugging
            // for (int i = 0; i < argCount; ++i) {
            //     cout << "Arg[" << i << "]: " << lineArgs[i] << endl;
            // }
           
            //cout<<line<<endl ; 
            // Call parse_input to get ParsedData for the current line
            data = parse_input(argCount, lineArgs);
            if (sodium_init() < 0)
            {
                cerr << "Failed to initialize sodium" << endl;
                return 1;
            }

            SecureLogger logger;
            token = data.K ;
            
            const char *to_be_encrypted_New = convertParsedDataToCStr(data)  ;
            const unsigned char *plaintextNew = reinterpret_cast<const unsigned char *>(to_be_encrypted_New);
            // cout << "plaintext size: " << strlen((const char *)plaintext) << endl;
            string filename = data.log ;
            filename += ".txt" ; 

            int status = logger.init(token, filename);

            if (status == 0)
            {
                cout << "The logfile exists from before and the token is correct" << endl;
                const char *decryptedDataChar ; 
                try
                {
                    cout << "Decrypting file....." << endl;
                    unsigned char *decryptedData = logger.decrypt_log();
                    decryptedDataChar = reinterpret_cast<const char *>(decryptedData) ; 
                    cout << "Decrypted Data: " << decryptedDataChar << endl;
                    // cout << "decrypted size: " << strlen((char *)data) << endl;
                    cout << "Decryption of file successful" << endl;
                }
                catch (const runtime_error &e)
                {
                    cerr << "Decryption of file error: " << e.what() << endl;
                    return 1; // Return an error code
                }
                Activity lastactivity = givelastActivity(decryptedDataChar, data) ; 
                
                int lastTimestamp = read_last_timestamp(decryptedDataChar);
                cout<<lastTimestamp<<" this is the timestamp"<<endl ; 
                // Ensure the provided timestamp is greater than the last one
                if (data.T <= lastTimestamp) {
                    cerr << "Error: The provided timestamp T must be greater than the last used timestamp (" << lastTimestamp << ")" << endl;
                    return 1;
                }



                if ( checks_on_sequence(lastactivity, data) ) {
                    // here 
                    const char* combinedLogFile = combineStrings(decryptedDataChar, to_be_encrypted_New) ; 
                    try
                    {
                        cout << "Encrypting plaintext....." << endl;
                        const unsigned char *combinedPlaintextNew = reinterpret_cast<const unsigned char *>(combinedLogFile);
                        logger.encrypt_log_plaintext(combinedPlaintextNew);
                        cout << "Encryption of plaintext successful" << endl;
                    }
                    catch (const runtime_error &e)
                    {
                        cerr << "Encryption of plaintext error: " << e.what() << endl;
                        return 1; // Return an error code
                    }

                } else {
                    invalid("Invalid operation: sequence of data is incorrect.") ;
                }

            }
            else if(status == 2){
                cout<<"The file does not exist from before"<<endl ; 
                if(data.A_flag && data.R == -1){
                try
                {
                    cout << "Encrypting plaintext....." << endl;
                    logger.encrypt_log_plaintext(plaintextNew);
                    cout << "Encryption of plaintext successful" << endl;
                }
                catch (const runtime_error &e)
                {
                    cerr << "Encryption of plaintext error: " << e.what() << endl;
                    return 1; // Return an error code
                }
                }
                else{
                    invalid("problem with the sequence of data") ; 
                }

                
            }
            else if (status == -1)
            {
                cout << "Token not verfified for file: " << filename << endl;
                invalid("token not validate");
            }
            else
            {
                cout << "Some error occured during the verification" << endl;
                invalid("verification error") ; 
            }

            // Ensure the log file has the ".log" extension
            
        }

        batchFile.close();
    } else {
        data = parse_input(argc, argv);
        // Ensure the log file has the ".log" extension
        //string logFileName = ensure_log_extension(data.log);
        // Read the last used timestamp
        if (sodium_init() < 0)
        {
            cerr << "Failed to initialize sodium" << endl;
            return 1;
        }

        SecureLogger logger;
        const char* token = data.K ;
        
        const char *to_be_encrypted_New = convertParsedDataToCStr(data)  ;
        const unsigned char *plaintextNew = reinterpret_cast<const unsigned char *>(to_be_encrypted_New);
        // cout << "plaintext size: " << strlen((const char *)plaintext) << endl;
        string filename = data.log ;
        filename += ".txt" ; 

        int status = logger.init(token, filename);

        if (status == 0)
        {
            cout << "The logfile exists from before and the token is correct" << endl;
            const char *decryptedDataChar ; 
            try
            {
                cout << "Decrypting file....." << endl;
                unsigned char *decryptedData = logger.decrypt_log();
                decryptedDataChar = reinterpret_cast<const char *>(decryptedData) ; 
                cout << "Decrypted Data: " << decryptedDataChar << endl;
                // cout << "decrypted size: " << strlen((char *)data) << endl;
                cout << "Decryption of file successful" << endl;
            }
            catch (const runtime_error &e)
            {
                cerr << "Decryption of file error: " << e.what() << endl;
                return 1; // Return an error code
            }
            Activity lastactivity = givelastActivity(decryptedDataChar, data) ; 
            
            int lastTimestamp = read_last_timestamp(decryptedDataChar);
            cout<<lastTimestamp<<" this is the timestamp"<<endl ; 
            // Ensure the provided timestamp is greater than the last one
            if (data.T <= lastTimestamp) {
                cerr << "Error: The provided timestamp T must be greater than the last used timestamp (" << lastTimestamp << ")" << endl;
                return 1;
            }



            if ( checks_on_sequence(lastactivity, data) ) {
                // here 
                const char* combinedLogFile = combineStrings(decryptedDataChar, to_be_encrypted_New) ; 
                try
                {
                    cout << "Encrypting plaintext....." << endl;
                    const unsigned char *combinedPlaintextNew = reinterpret_cast<const unsigned char *>(combinedLogFile);
                    logger.encrypt_log_plaintext(combinedPlaintextNew);
                    cout << "Encryption of plaintext successful" << endl;
                }
                catch (const runtime_error &e)
                {
                    cerr << "Encryption of plaintext error: " << e.what() << endl;
                    return 1; // Return an error code
                }

            } else {
                invalid("Invalid operation: sequence of data is incorrect.") ;
            }

        }
        else if(status == 2){
            cout<<"The file does not exist from before"<<endl ; 
            if(data.A_flag && data.R == -1){
            try
            {
                cout << "Encrypting plaintext....." << endl;
                logger.encrypt_log_plaintext(plaintextNew);
                cout << "Encryption of plaintext successful" << endl;
            }
            catch (const runtime_error &e)
            {
                cerr << "Encryption of plaintext error: " << e.what() << endl;
                return 1; // Return an error code
            }
            }
            else{
                invalid("problem with the sequence of data") ; 
            }

            
        }
        else if (status == -1)
        {
            cout << "Token not verfified for file: " << filename << endl;
            invalid("token not validate");
        }
        else
        {
            cout << "Some error occured during the verification" << endl;
            invalid("verification error") ; 
        }
    }

    return 0;
}

