
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>  // For parsing the log entry
#include "parseAppend.h"  // Include the input parsing module
#include<unordered_map>
#include<map> 
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

struct PersonActivity {
    int campusArrivals = 0;
    int campusToRoom = 0;
    int roomDepartures = 0;
    int campusDepartures = 0;
    unordered_map<int, int> currentRooms; // Store rooms the person is currently living in
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


bool process_log(const string& logFileName, ParsedData data) {
    string personName  ; 
    if(data.E != nullptr){
        personName = data.E  ; 
    }
    else if(data.G != nullptr) {
        personName = data.G ; 
    }
    
    ifstream logFile(logFileName);

    if (!logFile.is_open()) {
        cerr << "Error: Could not open log file for reading!" << endl;
        return 0;
    }

    string line;
    PersonActivity activity;  // Initialize an activity struct for the person
    bool found = false;

    // Scan through the log file
    while (getline(logFile, line)) {
        Activity lineData = parse_log_entry(line) ; 
        if ((lineData.E != "" && lineData.E == data.E ) || (lineData.G != "" && lineData.G == data.G)) {
            // Update counts and room tracking based on the event
            if (lineData.A_flag && lineData.R == -1) {
                activity.campusArrivals++;
            } else if (lineData.A_flag && lineData.R != -1) {
                activity.campusToRoom++;
                activity.currentRooms[lineData.R]++;  // Add room to the currentRooms set
            } else if (lineData.L_flag && lineData.R != -1) {
                activity.roomDepartures++;
                activity.currentRooms[lineData.R]--;  // Remove room from the currentRooms set
            } else if (lineData.L_flag && lineData.R == -1) {
                activity.campusDepartures++;
            }
            found = true;
        }
    }

    logFile.close();

    // if (!found) {
    //     cout << "No activity found for the employee with name: " + personName << endl;
    //     return ;
    // }
    // Display the tracked activity
    cout << "Activity for " << personName << ":" << endl;
    cout << "Campus Arrivals: " << activity.campusArrivals << endl;
    cout << "Moved to Room: " << activity.campusToRoom  << endl;
    cout << "Room Departures: " << activity.roomDepartures << endl;
    cout << "Campus Departures: " << activity.campusDepartures << endl;

    // Display current rooms
    cout << "Currently in Rooms: ";
    if (activity.currentRooms.empty()) {
        cout << "None";
    } else {
        for (auto room : activity.currentRooms) {
            cout << room.first << " "<<room.second ;
        }
    }
    cout << endl;
    // campus arrival 
    if(data.A_flag && data.R == -1){
        // no problem here 
        return 1  ; 
    }
    else if (data.A_flag && data.R != -1){
        /// check an person with personName that is in campus but not in room 
        if(activity.campusArrivals > activity.campusDepartures && (activity.campusArrivals-activity.campusDepartures)-(activity.campusToRoom-activity.roomDepartures) > 0){
            return 1 ; 
        }
        else {
            return 0 ; 
        }
    }
    else if(data.L_flag && data.R != -1){
        if(activity.currentRooms.find(data.R) != activity.currentRooms.end()){
            return 1 ; 
        }
        else {
            return 0 ; 
        }
    }
    // campus departure 
    else if(data.L_flag && data.R == -1){
        if(activity.campusArrivals > activity.campusDepartures && (activity.campusArrivals-activity.campusDepartures)-(activity.campusToRoom-activity.roomDepartures) > 0 && activity.campusDepartures < activity.roomDepartures){
            return 1 ; 
        }
        else{
            return 0 ; 
        }
    }
    return 0 ; 
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
    if (!fileExist || process_log(logFileName, data) ) {
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


