/*
logappend -T <timestamp> -K <token> (-E <employee-name> | -G <guest-name>) (-A | -L) [-R <room-id>] <log>
logappend -B <file>
*/

// This is just a high level implementation. In actual implementation it would append the encrypted data and not the plaintext data.

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

void appendLog(const string &timestamp, const string &token, const string &name, bool isEmployee, bool isArrival, const string &roomId, const string &logFile)
{
    fstream log;

    // Open or create the log file in append mode
    log.open(logFile, ios::in | ios::out | ios::app);
    if (!log.is_open())
    {
        // If file doesn't exist, create it
        log.clear();                      // Clear fail state
        log.open(logFile, ios::out); // Create the file
        log.close();
        log.open(logFile, ios::in | ios::out | ios::app); // Re-open in append mode
    }

    string line;

    log.seekg(0, ios::beg); // Move to the beginning

    // Prepare the log entry string
    string logEntry = timestamp + " " + token + " ";

    if (isArrival)
        logEntry += "A ";
    else
        logEntry += "L ";

    if (isEmployee)
        logEntry += "E: " + name + " ";
    else
        logEntry += "G: " + name + " ";

    if (!roomId.empty())
        logEntry += "R: " + roomId;

    log.seekp(0, ios::end); // Move to the end of the file
    log << logEntry << endl;

    // Close the log file
    log.close();
}


int main(int argc, char *argv[])
{
    string timestamp = "12345";
    string token = "token123";
    string name = "JohnDoe";
    bool isEmployee = true;     // true if -E, false if -G
    bool isArrival = true;      // true if -A, false if -L
    string roomId = "101";
    string logFile = "campus.log";
    appendLog(timestamp, token, name, isEmployee, isArrival, roomId, logFile);
    return 0;
}