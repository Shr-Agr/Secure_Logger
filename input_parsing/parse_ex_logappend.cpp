#include <iostream>
#include "parseAppend.h" // Include the input parsing module

using namespace std;

int main(int argc, char* argv[]) {
    // Call the input parsing function
    ParsedData data = parse_input(argc, argv);

    // Print the values of all variables
    cout << "T: " << (data.T) 
         << " K: " << (data.K ? data.K : "null") 
         << " E: " << (data.E ? data.E : "null") 
         << " G: " << (data.G ? data.G : "null") 
         << " R: " << (data.R ) 
         << " A_flag: " << data.A_flag 
         << " L_flag: " << data.L_flag 
         << " log file path: "<<(data.log ? data.log : "null")
         << endl;

    return 0;
}