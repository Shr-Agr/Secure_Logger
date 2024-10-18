#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <algorithm>
#include "parseRead.h"

using namespace std;

// Structure to hold information of each person (employee/guest)
struct PersonInfo {
    string name;
    bool isEmployee; // true if employee, false if guest
    vector<pair<string, string>> roomHistory; // room visits: <roomId, timestamp>
    vector<string> roomEntries; // List to store all rooms entered over the history
};

struct TimeInfo {
    string entryTimestamp;
    string exitTimestamp;
    int totalTime = 0; // Total time spent
};

// Function to read the log file and construct the data structure
void readLogFile(const string &logFile, map<string, PersonInfo> &campusState, set<string> &employees, set<string> &guests, map<string, TimeInfo> &timeTracking, string &latestTimestamp) {
    ifstream log(logFile);

    if (!log.is_open()) {
        cerr << "Unable to open log file." << endl;
        exit(255);
    }

    string line;
    while (getline(log, line)) {
        stringstream ss(line);
        // T: 1 K: token1 E: Fred G: null R: -1 A_flag: true L_flag: false
        string T, timestamp,K, token,E, employee,G, guest, R,roomId,A, a_flag_str,L, l_flag_str;
        bool A_flag, L_flag;

        // Extract the fields from the log line
        ss >> T >> timestamp >>K>> token >>E>> employee >>G>> guest >>R>> roomId >>A>> a_flag_str >>L>> l_flag_str;
        // Parsing the values
        // timestamp = timestamp.substr(2);  // Remove "T:"
        // token = token.substr(2);          // Remove "K:"
        // employee = employee.substr(2);    // Remove "E:"
        // guest = guest.substr(2);          // Remove "G:"
        // roomId = roomId.substr(2);        // Remove "R:"
        // a_flag_str = a_flag_str.substr(8); // Remove "A_flag:"
        // l_flag_str = l_flag_str.substr(8); // Remove "L_flag:"

        // Convert A_flag and L_flag from string to boolean
        A_flag = (a_flag_str == "true");
        L_flag = (l_flag_str == "true");

        latestTimestamp = timestamp; // Update the latest timestamp

        // Determine whether it's an employee or guest
        bool isEmployee = (employee != "null");
        string name = isEmployee ? employee : guest;

        // Track entry and exit timestamps in the timeTracking map
        if (roomId == "-1") {  // Entering or leaving the campus
            if (A_flag) {
                // If a person enters, store the entry timestamp
                timeTracking[name].entryTimestamp = timestamp;
            } else if (L_flag) {
                // If a person exits, calculate the time spent and update the total
                if (!timeTracking[name].entryTimestamp.empty()) {
                    timeTracking[name].exitTimestamp = timestamp;

                    // Calculate time difference and update total time
                    int duration = stoi(timeTracking[name].exitTimestamp) - stoi(timeTracking[name].entryTimestamp);
                    timeTracking[name].totalTime += duration;

                    // Clear the entry timestamp for future entries
                    timeTracking[name].entryTimestamp.clear();
                    timeTracking[name].exitTimestamp.clear();
                }
            }
        }

        // Update campus state
        if (isEmployee) {
            if (A_flag && roomId == "-1") employees.insert(name);
            else if (L_flag && roomId == "-1") employees.erase(name);
        } else {
            if (A_flag && roomId == "-1") guests.insert(name);
            else if (L_flag && roomId == "-1") guests.erase(name);
        }

        // Track room history if it's not the campus entry (-1)
        if (roomId != "-1") {
            campusState[name].name = name;
            campusState[name].isEmployee = isEmployee;

            // If it's an arrival, log the room and update the history
            if (A_flag) {
                campusState[name].roomHistory.push_back({roomId, timestamp});
                campusState[name].roomEntries.push_back(roomId);  // Track all rooms entered
            } else if (L_flag) {
                // If leaving, remove the last room entry
                if (!campusState[name].roomHistory.empty()) {
                    campusState[name].roomHistory.pop_back();
                }
            }
        }
    }

    log.close();
}


// Function to print the current state of the log (for -S option)
void printState(const set<string> &employees, const set<string> &guests, const map<string, PersonInfo> &campusState) {
    // Print employees currently in the campus
    for (auto it = employees.begin(); it != employees.end(); ++it) {
        cout << *it;
        if (next(it) != employees.end()) cout << ", ";
    }
    cout << endl;

    // Print guests currently in the campus
    for (auto it = guests.begin(); it != guests.end(); ++it) {
        cout << *it;
        if (next(it) != guests.end()) cout << ", ";
    }
    cout << endl;

    // Prepare a map to store people present in each room
    map<string, vector<string>> rooms;

    // Iterate over each person in campusState and record room visits
    for (const auto &entry : campusState) {
        const string &name = entry.first;
        const auto &roomHistory = entry.second.roomHistory;

        // If the room history is not empty, get the last visited room
        if (!roomHistory.empty()) {
            const auto &lastVisit = roomHistory.back(); // Get last room visited
            rooms[lastVisit.first].push_back(name); // Add person to the respective room
        }
    }

    // Print the room-wise occupancy in ascending order
    for (const auto &room : rooms) {
        if(stoi(room.first)>0)
        {
            cout << room.first << ": "; // Print room ID
            for (auto it = room.second.begin(); it != room.second.end(); ++it) {
                cout << *it;
                if (next(it) != room.second.end()) cout << ", "; // Print comma between people but not after the last one
            }
            cout << endl;
        }
    }
}

// Function to print the history of rooms visited by a person (employee/guest)
void printRoomHistory(const string &name, const map<string, PersonInfo> &campusState,const set<string> &employees, const set<string> &guests, bool checkEmployee) {
    
    // Check if the person is an employee or a guest based on the flag
    if (checkEmployee && employees.find(name) == employees.end()) {
        cout <<name<<" not an employee." << endl; // Output invalid if the person is not an employee
        return;
    } else if (!checkEmployee && guests.find(name) == guests.end()) {
        cout <<name<<" not a guest." <<endl; // Output invalid if the person is not a guest
        return;
    }
    
    // Check if the person exists in campusState
    if (campusState.find(name) == campusState.end()) {
        cout << "not in campus" << endl; // Output invalid if the person is not found
        return;
    }
    // Get the person's room entries
    const auto &personInfo = campusState.at(name);
    const auto &roomsVisited = personInfo.roomEntries;

    // Print room history as comma-separated values
    if (roomsVisited.empty()) {
        cout << "invalidV" << endl; // If no rooms visited, return "invalid"
    } else {
        for (size_t i = 0; i < roomsVisited.size(); ++i) {
            if(stoi(roomsVisited[i])>0)
            {
                cout << roomsVisited[i];
                if (i != roomsVisited.size() - 1) cout << ",";
            }
        }
        cout << endl;
    }
}

void printTotalTime(const string &name, const map<string, TimeInfo> &timeTracking, const string &latestTimestamp,const set<string> &employees, const set<string> &guests, bool checkEmployee) {
     // Check if the person is an employee or a guest based on the flag
    if (checkEmployee && employees.find(name) == employees.end()) {
        cout <<name<<" not an employee." << endl; // Output invalid if the person is not an employee
        return;
    } else if (!checkEmployee && guests.find(name) == guests.end()) {
        cout <<name<<" not a guest." <<endl; // Output invalid if the person is not a guest
        return;
    }
       
    // Check if the person exists in timeTracking
    if (timeTracking.find(name) == timeTracking.end()) {
        cout << "invalidT" << endl; // Output invalid if the person is not found
        return;
    }

    const auto &timeInfo = timeTracking.at(name);
    int totalTime = timeInfo.totalTime;
    // If the person is currently inside (no exit timestamp), calculate time till now
    if (!timeInfo.entryTimestamp.empty()) {
        totalTime += stoi(latestTimestamp) - stoi(timeInfo.entryTimestamp);
    }

    cout << totalTime << endl;
}

#include <set>

// Function to print the rooms occupied by all specified persons simultaneously
// void printCommonRooms(const vector<string> &names, const map<string, PersonInfo> &campusState) {
//     map<string, set<int>> roomOccupancy; // roomId -> set of timestamps when it's occupied by given names

//     // Fill the roomOccupancy map with entries from the campusState
//     for (const auto &name : names) {
//         if (campusState.find(name) == campusState.end()) {
//             continue; // If the person is not found, ignore
//         }

//         const auto &personInfo = campusState.at(name);
//         const auto &roomHistory = personInfo.roomHistory;

//         // Iterate through room history
//         for (const auto &entry : roomHistory) {
//             roomOccupancy[entry.first].insert(stoi(entry.second)); // Add timestamp to room
//         }
//     }

//     // Find common rooms
//     vector<string> commonRooms;
//     for (const auto &entry : roomOccupancy) {
//         if (entry.second.size() == names.size()) {
//             commonRooms.push_back(entry.first);
//         }
//     }

//     // Print the common rooms in ascending order
//     if (!commonRooms.empty()) {
//         sort(commonRooms.begin(), commonRooms.end());
//         for (size_t i = 0; i < commonRooms.size(); ++i) {
//             if(stoi(commonRooms[i])>0)
//             {
//             cout << commonRooms[i];
//             if (i != commonRooms.size() - 1) cout << ",";
//             }
//         }
//         cout << endl;
//     } else {
//         cout << endl; // No common rooms found
//     }
// }

// int main(int argc, char *argv[]) {
//     if (argc < 4) {
//         cout << "invalidM" << endl;
//         return 255;
//     }
//     string token = argv[2];
//     string logFile = argv[argc - 1];

//     map<string, PersonInfo> campusState;
//     set<string> employees;
//     set<string> guests;
//     map<string, TimeInfo> timeTracking;
//     string latestTimestamp = "10"; // Variable to hold the latest timestamp

//     readLogFile(logFile, campusState, employees, guests, timeTracking, latestTimestamp);

//     string option = argv[3];
//     bool checkEmployee = false;

//     // Determine if we are checking for employees or guests
//     if (string(argv[4]) == "-E") {
//         checkEmployee = true;
//     } else if (string(argv[4]) == "-G") {
//         checkEmployee = false;
//     }

//     if (option == "-S") {
//         printState(employees, guests, campusState);
//     } 
//     else if (option == "-R") {
//         string name = argv[5];
//         printRoomHistory(name, campusState,employees,guests,checkEmployee);
//     } 
//     else if (option == "-T") {
//         string name = argv[5];
//         printTotalTime(name, timeTracking, latestTimestamp,employees,guests,checkEmployee);
//     } 
//     else if (option == "-I") {
//         vector<string> names;
//         for (int i = 5; i < argc; i=i+2) {
//             names.push_back(argv[i]); // Collect all specified names
//         }
//         for(int i=0; i<names.size(); i++)
//         {
//             cout<<names[i]<<" ";
//         }
//         printCommonRooms(names, campusState);
//     }
//     else {
//         cout << "invalidM1" << endl;
//         return 255;
//     }

//     return 0;
// }

void printCommonRooms(const vector<pair<string, bool>> &names, const map<string, PersonInfo> &campusState, const set<string> &employees, const set<string> &guests) {
    map<string, set<int>> roomOccupancy; // roomId -> set of timestamps when it's occupied by given names

    for (const auto &namePair : names) {
        const string &name = namePair.first;
        bool isEmployee = namePair.second;

        // Check if the person is in campusState
        if (campusState.find(name) == campusState.end()) {
            continue; // If the person is not found, ignore
        }

        // Check if the person is an employee or guest based on the flag
        if (isEmployee && employees.find(name) == employees.end()) {
            cout <<name<<" is not an employee."<< endl;
            return;
        } else if (!isEmployee && guests.find(name) == guests.end()) {
            cout << name<<" is not a guest." << endl;
            return;
        }

        const auto &personInfo = campusState.at(name);
        const auto &roomHistory = personInfo.roomHistory;

        // Iterate through room history
        for (const auto &entry : roomHistory) {
            cout<<entry.first<<" "<<entry.second<<endl;
            roomOccupancy[entry.first].insert(stoi(entry.second)); // Add timestamp to room
        }
    }

    // Find common rooms
    vector<string> commonRooms;
    for (const auto &entry : roomOccupancy) {
        if (entry.second.size() == names.size()) {
            commonRooms.push_back(entry.first);
        }
    }

    // Print the common rooms in ascending order
    if (!commonRooms.empty()) {
        sort(commonRooms.begin(), commonRooms.end());
        for (size_t i = 0; i < commonRooms.size(); ++i) {
            if (stoi(commonRooms[i]) > 0) {
                cout << commonRooms[i];
                if (i != commonRooms.size() - 1) cout << ",";
            }
        }
        cout << endl;
    } else {
        cout << endl; // No common rooms found
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        cout << "invalidM" << endl;
        return 255;
    }
    

   
    string token = argv[2];
    string logFile = argv[argc - 1];
    logFile=logFile.append(".log");
    cout<<logFile<<endl;
    ParsedData data;
    data=parse_input(argc,argv);
    cout<<data.S_flag<<endl;

    map<string, PersonInfo> campusState;
    set<string> employees;
    set<string> guests;
    map<string, TimeInfo> timeTracking;
    string latestTimestamp = "10"; // Variable to hold the latest timestamp

    readLogFile(logFile, campusState, employees, guests, timeTracking, latestTimestamp);

    string option = argv[3];

    if (option == "-S") {
        printState(employees, guests, campusState);
    } 
    else if (option == "-R") {
        bool checkEmployee = (string(argv[4]) == "-E");
        string name = argv[5];
        printRoomHistory(name, campusState, employees, guests, checkEmployee);
    } 
    else if (option == "-T") {
        bool checkEmployee = (string(argv[4]) == "-E");
        string name = argv[5];
        printTotalTime(name, timeTracking, latestTimestamp, employees, guests, checkEmployee);
    } 
    else if (option == "-I") {
        vector<pair<string, bool>> names; // To hold (name, isEmployee)

        // Parse names and flags (-E or -G)
        for (int i = 4; i < argc; i += 2) {
            if (i + 1 < argc) {
                bool isEmployee = (string(argv[i]) == "-E");
                names.push_back({argv[i + 1], isEmployee});
            }
        }

        // Print the list of names for debugging purposes
        for (int i = 0; i < names.size(); i++) {
            cout << names[i].first << " (" << (names[i].second ? "Employee" : "Guest") << ")" << endl;
        }

        // Call the function to print common rooms
        printCommonRooms(names, campusState, employees, guests);
    }
    else {
        cout << "invalidM1" << endl;
        return 255;
    }

    return 0;
}
