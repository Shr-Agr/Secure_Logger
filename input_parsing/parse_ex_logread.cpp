#include <iostream>
#include "parseRead.h" 

using namespace std;

int main(int argc, char* argv[]) {
    ParsedData data = parse_input(argc, argv);

    // Example output: Successfully parsed the input
    cout << "Token: " << data.K << endl;
    if (data.S_flag) {
        cout << " (-S)" << endl;
    }
    if (data.R_flag) {
        cout << "Room list (-R)" << endl;
    }
    if (data.T_flag) {
        cout << "Total time (-T)" << endl;
    }
    if (data.I_flag) {
        cout << "Common rooms (-I)" << endl;
    }
    for (const auto& e : data.E_names) {
        cout << "Employee: " << e << endl;
    }
    for (const auto& g : data.G_names) {
        cout << "Guest: " << g << endl;
    }
    cout << "Log file: " << data.log << endl;

    return 0;
}