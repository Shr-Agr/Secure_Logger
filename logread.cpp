
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

// Structure to hold information of each person (employee/guest)
struct PersonInfo {
    string name;
    bool isEmployee; // true if employee, false if guest
    vector<pair<string, string>> roomHistory; // room visits: <roomId, timestamp>
};

// Function to read the log file and construct the data structure
void readLogFile(const string &logFile, map<string, PersonInfo> &campusState, set<string> &employees, set<string> &guests) {
    ifstream log(logFile);

    if (!log.is_open()) {
        cerr << "Unable to open log file." << endl;
        exit(255);
    }

    string line;

    while (getline(log, line)) {
        stringstream ss(line);
        string timestamp, token, action, personType, name, roomId;

        ss >> timestamp >> token >> action;
        ss >> personType >> name;

        bool isArrival = (action == "A");
        bool isEmployee = (personType == "E:");

        if (ss >> roomId) {
            roomId = roomId.substr(2); // Remove "R:" from roomId
        } else {
            roomId = ""; // No room mentioned
        }

        // Process the entry
        if (isEmployee) {
            if (isArrival) employees.insert(name);
            else employees.erase(name);
        } else {
            if (isArrival) guests.insert(name);
            else guests.erase(name);
        }

        if (!roomId.empty()) {
            campusState[name].name = name;
            campusState[name].isEmployee = isEmployee;

            // If it's an arrival, log the room and update the history
            if (isArrival) {
                campusState[name].roomHistory.push_back({roomId, timestamp});
            } else {
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
    cout << "Employees in campus: ";
    for (auto it = employees.begin(); it != employees.end(); ++it) {
        cout << *it;
        if (next(it) != employees.end()) cout << ", ";
    }
    cout << endl;

    // Print guests currently in the campus
    cout << "Guests in campus: ";
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
        cout << "Room " << room.first << ": "; // Print room ID
        for (auto it = room.second.begin(); it != room.second.end(); ++it) {
            cout << *it;
            if (next(it) != room.second.end()) cout << ", "; // Print comma between people but not after the last one
        }
        cout << endl;
    }
}



void printRoomHistory(const string &name, const map<string, PersonInfo> &campusState) {
    // Check if the person exists in campusState
    if (campusState.find(name) == campusState.end()) {
        cout << "invalid" << endl; // Output invalid if the person is not found
        return;
    }

    // Get the person's room history
    const auto &personInfo = campusState.at(name);
    vector<string> roomsVisited;

    // Collect all rooms visited by the person
    for (const auto &entry : personInfo.roomHistory) {
        roomsVisited.push_back(entry.first); // Extract room IDs
    }

    // Print room history as comma-separated values
    if (roomsVisited.empty()) {
        cout << "invalid" << endl; // If no rooms visited, return "invalid"
    } else {
        for (size_t i = 0; i < roomsVisited.size(); ++i) {
            cout << roomsVisited[i];
            if (i != roomsVisited.size() - 1) cout << ",";
        }
        cout << endl;
    }
}



void printTotalTime(const string &name, const map<string, PersonInfo> &campusState) {
    // Check if the person exists in campusState
    if (campusState.find(name) == campusState.end()) {
        cout << "invalid" << endl; // Output invalid if the person is not found
        return;
    }

    // Get the person's room history
    const auto &personInfo = campusState.at(name);
    int totalTime = 0;
    string lastEntryTime;
    bool currentlyInCampus = false;

    // Loop through the person's room history to calculate time
    for (const auto &entry : personInfo.roomHistory) {
        string timestamp = entry.second;

        if (!currentlyInCampus) {
            // Arrival: Record the entry time
            lastEntryTime = timestamp;
            currentlyInCampus = true;
        } else {
            // Departure: Add the difference to total time
            totalTime += stoi(timestamp) - stoi(lastEntryTime);
            currentlyInCampus = false;
        }
    }

    // If the person is currently in the campus, include ongoing time
    if (currentlyInCampus) {
        string currentTimestamp = "10000"; // Simulated current timestamp
        totalTime += stoi(currentTimestamp) - stoi(lastEntryTime);
    }

    cout << totalTime << endl;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        cout << "invalid" << endl;
        return 255;
    }
    string token = argv[2];
    string logFile = argv[argc - 1];

    map<string, PersonInfo> campusState;
    set<string> employees;
    set<string> guests;
    map<string, vector<string>> rooms;

    readLogFile(logFile, campusState, employees, guests);

    string option = argv[3];

    if (option == "-S") {
        printState(employees, guests, campusState);
    } 
    else if (option == "-R") {
        string name = argv[5];
        printRoomHistory(name, campusState);
    } 
    else if (option == "-T") {
        if (argc != 6) {
            cout << "invalid" << endl;
            return 255;
        }

        string name = argv[4];
        printTotalTime(name, campusState);
    } 
    // else if (option == "-C") {
    //     if (argc < 7) {
    //         cout << "invalid" << endl;
    //         return 255;
    //     }

    //     vector<string> names;
    //     for (int i = 4; i < argc - 1; ++i) {
    //         names.push_back(argv[i]);
    //     }

    //     printCommonRooms(names, campusState);
    // } 
    else {
        cout << "invalid" << endl;
        return 255;
    }

    return 0;
}
