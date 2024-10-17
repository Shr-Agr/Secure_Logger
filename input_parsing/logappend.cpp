
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>  // For parsing the log entry
#include "parseAppend.h"  // Include the input parsing module
#include<unordered_map>
#include<map> 
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
    if (logName.size() < 4 || logName.substr(logName.size() - 4) != ".log") {
        logName += ".log";  // Add ".log" if not already present
    }
    return logName;
}
bool is_log_file_empty(const std::string& logFileName) {
    ifstream file(logFileName);
    return file.peek() == ifstream::traits_type::eof();
}

// Function to read the last used timestamp from the log file
int read_last_timestamp(const std::string& logFileName) {
    if (is_log_file_empty(logFileName)) {
        return 0; // If the log file is empty, return 0 as the last timestamp
    }

    std::ifstream logFile(logFileName, std::ios::in);
    if (!logFile) {
        std::cerr << "Error: Could not open log file." << std::endl;
        return -1; // Error handling, return a negative value on failure
    }

    // Seek to the end of the file
    logFile.seekg(0, std::ios::end);
    
    // Move back from the end to read the last non-empty line
    char ch;
    int pos = logFile.tellg();
    
    // Move back to skip any trailing newlines
    while (pos > 0) {
        logFile.seekg(--pos, std::ios::beg);
        logFile.get(ch);
        if (ch != '\n' && ch != '\r') {
            break; // Exit if a non-newline character is found
        }
    }

    // Now read backward to the start of the last valid line
    std::string lastLine;
    while (pos > 0) {
        logFile.seekg(--pos, std::ios::beg);
        logFile.get(ch);
        if (ch == '\n' || pos == 0) {
            std::getline(logFile, lastLine);
            break;
        }
    }

    logFile.close();

    // Extract the timestamp from the last line
    int lastTimestamp = 0;
    std::stringstream ss(lastLine);
    std::string temp;

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

Activity givelastActivity(const string& logFileName, ParsedData data) {
    string personName;
    if (data.E != nullptr) {
        personName = data.E;
    } else if (data.G != nullptr) {
        personName = data.G;
    }

    ifstream logFile(logFileName);
    // Read the entire file into a vector to traverse it backwards
    vector<string> logEntries;
    string line;

    while (getline(logFile, line)) {
        logEntries.push_back(line);
    }

    logFile.close();

    // Traverse the log from the last entry to the first
    Activity lastActivity;

    for (auto it = logEntries.rbegin(); it != logEntries.rend(); ++it) {
        Activity activity = parse_log_entry(*it);

        // Check if the activity matches the person name (Employee or Guest)
        if ((activity.E != "" && data.E != nullptr &&  activity.E == personName) || (activity.G != "" && data.G != nullptr &&  activity.G == personName))  {
            lastActivity = activity;
            break;  // We found the last activity, no need to continue
        }
    }

    return lastActivity;
}

bool checks_on_sequence(const string& logFileName, ParsedData data){
    ifstream logFile(logFileName);

    if (!logFile.is_open()) {
        cerr << "Error: Could not open log file for reading!" << endl;
        return false;
    }
    Activity lastActivity = givelastActivity(logFileName, data) ; 
    
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


    
bool check_token_in_log(const string& logFileName, const string& token) {
    ifstream logFile(logFileName);
    if (!logFile) {
        // File does not exist
        return false;
    }

    string line;
    bool flag = 0 ; 
    while (getline(logFile, line)) {
        istringstream iss(line);
        string field, extractedToken;
        Activity lineData =  parse_log_entry(line) ; 
        if(lineData.K == token){
            flag = 1 ; 
        }
        else{
            invalid("token is not correct") ; 
            flag = 0 ; 
        }
        break ; 
        
    }

    logFile.close();
    return flag;
}
bool write_to_log(const string& logFileName, const ParsedData& data) {
    ifstream checkFile(logFileName);
    bool fileExists = checkFile.good();
    checkFile.close();

    // If file exists, check the token
    if (fileExists) {
        if (!check_token_in_log(logFileName, data.K)) {
            cout << "Error: Token K does not match any entry in the existing log file." << endl;
            return false;
        }
    }

    // Open the log file for appending (or creating if it doesn't exist)
    ofstream logFile;
    logFile.open(logFileName, ios::app);  // Open file in append mode

    if (!logFile) {
        cerr << "Error: Could not open log file " << logFileName << endl;
        return false;
    }

    // Write the parsed data to the log file
    logFile << "T: " << data.T
            << " K: " << (data.K ? data.K : "null")
            << " E: " << (data.E ? data.E : "null")
            << " G: " << (data.G ? data.G : "null")
            << " R: " << data.R
            << " A_flag: " << (data.A_flag ? "true" : "false")
            << " L_flag: " << (data.L_flag ? "true" : "false")
            << endl;

    // Close the log file
    logFile.close();
    return true;
}

bool fileExist(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
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

            // Ensure the log file has the ".log" extension
            string logFileName = ensure_log_extension(data.log);

            // Read the last used timestamp
            int lastTimestamp = read_last_timestamp(logFileName);

            // Ensure the provided timestamp is greater than the last one
            if (data.T <= lastTimestamp) {
                cerr << "Error on line " << lineNumber << ": The provided timestamp T must be greater than the last used timestamp (" << lastTimestamp << ")" << endl;
                continue;  // Skip to the next line
            }

            // Process the log and write data if valid
            
            if (fileExist(logFileName.c_str())) {
                if (checks_on_sequence(logFileName, data)) {
                    if (write_to_log(logFileName, data)) {
                        cout << "Entry added for line " << lineNumber << endl;
                    } else {
                        cerr << "Error on line " << lineNumber << ": Problem with the token" << endl;
                    }
                } else {
                    cerr << "Error on line " << lineNumber << ": Invalid operation, sequence of data is incorrect." << endl;
                }
            } else {
                // Handle initial entry case
                if (data.A_flag && data.R == -1) {
                    if (write_to_log(logFileName, data)) {
                        cout << "Initial entry added for line " << lineNumber << endl;
                    } else {
                        cerr << "Error on line " << lineNumber << ": Problem with the token" << endl;
                    }
                } else {
                    cerr << "Error on line " << lineNumber << ": Problem with the sequence of data" << endl;
                }
            }
        }

        batchFile.close();
    } else {
        data = parse_input(argc, argv);
        // Ensure the log file has the ".log" extension
        string logFileName = ensure_log_extension(data.log);
        // Read the last used timestamp
        int lastTimestamp = read_last_timestamp(logFileName);
        cout<<lastTimestamp<<" this is the timestamp"<<endl ; 
        // Ensure the provided timestamp is greater than the last one
        if (data.T <= lastTimestamp) {
            cerr << "Error: The provided timestamp T must be greater than the last used timestamp (" << lastTimestamp << ")" << endl;
            return 1;
        }

        // Process the log and write data if valid
        ifstream file(logFileName);  // Try to open the file
        bool fileExist = file.good();       
        if(!fileExist){
            if(data.A_flag && data.R == -1){
                if (write_to_log(logFileName, data)) {
                    // Successfully wrote to log, update timestamp
                cout<<"added entry"<<endl ; 
                } else {
                    invalid("problem with the token") ; 
                }
            }
            else{
                invalid("problem with the sequence of data") ; 
            }
        }
        else if ( checks_on_sequence(logFileName, data) ) {
            if (write_to_log(logFileName, data)) {
                // Successfully wrote to log, update timestamp
            cout<<"added entry"<<endl ; 
            } else {
                invalid("problem with the token") ; 
            }
        } else {
            invalid("Invalid operation: sequence of data is incorrect.") ;
        }
        
    }

    return 0;
}


/*


int main(int argc, char* argv[]) {
    // Call the input parsing function (not implemented in this snippet)
    ParsedData data = parse_input(argc, argv);

    // Ensure the log file has the ".log" extension
    string logFileName = ensure_log_extension(data.log);
    // Read the last used timestamp
    int lastTimestamp = read_last_timestamp(logFileName);
    cout<<lastTimestamp<<" this is the timestamp"<<endl ; 
    // Ensure the provided timestamp is greater than the last one
    if (data.T <= lastTimestamp) {
        cerr << "Error: The provided timestamp T must be greater than the last used timestamp (" << lastTimestamp << ")" << endl;
        return 1;
    }

    // Process the log and write data if valid
    ifstream file(logFileName);  // Try to open the file
    bool fileExist = file.good();       
    if(!fileExist){
        if(data.A_flag && data.R == -1){
            if (write_to_log(logFileName, data)) {
                // Successfully wrote to log, update timestamp
            cout<<"added entry"<<endl ; 
            } else {
                invalid("problem with the token") ; 
            }
        }
        else{
            invalid("problem with the sequence of data") ; 
        }
    }
    else if ( checks_on_sequence(logFileName, data) ) {
        if (write_to_log(logFileName, data)) {
            // Successfully wrote to log, update timestamp
           cout<<"added entry"<<endl ; 
        } else {
            invalid("problem with the token") ; 
        }
    } else {
        invalid("Invalid operation: sequence of data is incorrect.") ;
    }

    return 0;
}
*/
