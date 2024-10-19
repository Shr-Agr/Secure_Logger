// #include <iostream>
// #include <cstdlib>
// #include <cstring>
// #include <getopt.h>
// #include <cctype>
// #include <vector>
// // #include<sodium.h>
// // #include "parseRead.h"  
// using namespace std;

// typedef struct {
//     const char* K = nullptr;
//     bool S_flag = false;
//     bool R_flag = false;
//     bool T_flag = false;
//     bool I_flag = false;

//     vector<const char*> E_names;
//     vector<const char*> G_names;
//     const char* log = nullptr;
// }ParsedData;

// bool validate_token(const char* token) {
//     for (int i = 0; token[i] != '\0'; i++) {
//         if (!isalnum(token[i])) {
//             return false;
//         }
//     }
//     return true;
// }

// bool validate_name(const char* name) {
//     for (int i = 0; name[i] != '\0'; i++) {
//         if (!isalpha(name[i])) {
//             return false;
//         }
//     }
//     return true;
// }

// bool validate_log_file(const char* log_path) {
//     bool last_was_slash = false;      
//     bool last_was_period = false;    
//     bool contains_alnum = false;     
//     bool valid_relative_path = false;
//     bool alnum_after_slash = false;   
//     int period_count = 0;             

//     for (int i = 0; log_path[i] != '\0'; i++) {
//         char c = log_path[i];

//         // Check for valid characters (alphanumeric, _, ., /)
//         if (!isalnum(c) && c != '_' && c != '.' && c != '/') {
//             return false;
//         }

      
//         if (c == '/') {
//             if (last_was_slash) {
//                 return false;  
//             }
//             last_was_slash = true;
//             period_count = 0; 
//             last_was_period = false;
//             alnum_after_slash = false; 
//         } else {
//             last_was_slash = false;  

//             // Handle periods `.`
//             if (c == '.') {
//                 period_count++;
//                 last_was_period = true;

//                 if (period_count > 2) {
//                     return false;  
//                 }
//             } else {
//                 // Handle valid alphanumeric characters after `.` or `..`
//                 if (last_was_period && period_count == 1 && c == '/') {
//                     valid_relative_path = true;  // Valid `./` pattern
//                 } else if (last_was_period && period_count == 2 && c == '/') {
//                     valid_relative_path = true;  // Valid `../` pattern
//                 } else if (period_count > 0 && !valid_relative_path) {
//                     return false;  
//                 }

//                 period_count = 0;  
//                 last_was_period = false;

              
//                 if (isalnum(c)) {
//                     contains_alnum = true;

//                     if (alnum_after_slash && last_was_period) {
//                         return false;  // No period can come after an alphanumeric string
//                     }

//                     alnum_after_slash = true;
//                 }
//             }
//         }
//     }

//     if (last_was_slash) {
//         return false;
//     }
//     if(last_was_period){
//         return false ; 
//     }

//     if (!contains_alnum && !valid_relative_path) {
//         return false;
//     }
//     return true;
// }


// void invalid() {
//     cout << "invalid" << endl;
//     exit(255);
// }

// ParsedData parse_input(int argc, char* argv[]) {
//     ParsedData data;
//     int option;

//     while ((option = getopt(argc, argv, "K:SRTIE:G:")) != -1) {
//         switch (option) {
//             case 'K':
//                 data.K = optarg;
//                 break;
//             case 'S':
//                 if (data.R_flag || data.T_flag || data.I_flag) invalid();
//                 data.S_flag = true;
//                 break;
//             case 'R':
//                 if (data.S_flag || data.T_flag || data.I_flag) invalid();
//                 data.R_flag = true;
//                 break;
//             case 'T':
//                 if (data.S_flag || data.R_flag || data.I_flag) invalid();
//                 data.T_flag = true;
//                 break;
//             case 'I':
//                 if (data.S_flag || data.R_flag || data.T_flag ) invalid();
//                 data.I_flag = true;
//                 break;
//             case 'E':
//                 data.E_names.push_back(optarg);
//                 break;
//             case 'G':
//                 data.G_names.push_back(optarg);
//                 break;
//             default:
//                 invalid();
//         }
//     }
//     if ( !validate_token(data.K)) invalid();
//     for(int i = 0 ; i<data.E_names.size() ; i++){
//         if(!validate_name(data.E_names[i])) invalid(); 
//     }
//     for(int i = 0 ; i<data.G_names.size() ; i++){
//         if(!validate_name(data.G_names[i])) invalid(); 
//     }
//     if (optind < argc) {
//         if (!validate_log_file(argv[optind])) invalid();
//         if (optind + 1 != argc) invalid();
//         data.log = argv[optind];
//     } else {
//         invalid();
//     }

//     if (data.K == nullptr || (!data.S_flag && !data.R_flag && !data.T_flag && !data.I_flag)) {
//         invalid();
//     }

//     if ((data.R_flag || data.T_flag) && data.E_names.empty() && data.G_names.empty()) {
//         invalid();
//     }
//     if ((data.R_flag || data.T_flag) && data.E_names.size() != 1 && data.G_names.size() != 1) {
//         invalid();
//     }
//     if (data.S_flag && (!data.E_names.empty() || !data.G_names.empty())) {
//         invalid();
//     }

//     return data;
// }

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <cctype>
#include <vector>
// #include<sodium.h>
// #include "parseRead.h"  
using namespace std;

typedef struct {
    const char* K = nullptr;
    bool S_flag = false;
    bool R_flag = false;
    bool T_flag = false;
    bool I_flag = false;
    bool E_flag = false;

    vector<const char*> E_names;
    vector<const char*> G_names;
    const char* log = nullptr;
}ParsedData;

bool validate_token(const char* token) {
    for (int i = 0; token[i] != '\0'; i++) {
        if (!isalnum(token[i])) {
            return false;
        }
    }
    return true;
}

bool validate_name(const char* name) {
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha(name[i])) {
            return false;
        }
    }
    return true;
}

bool validate_log_file(const char* log_path) {
    bool last_was_slash = false;      
    bool last_was_period = false;    
    bool contains_alnum = false;     
    bool valid_relative_path = false;
    bool alnum_after_slash = false;   
    int period_count = 0;             

    for (int i = 0; log_path[i] != '\0'; i++) {
        char c = log_path[i];

        // Check for valid characters (alphanumeric, _, ., /)
        if (!isalnum(c) && c != '_' && c != '.' && c != '/') {
            return false;
        }

      
        if (c == '/') {
            if (last_was_slash) {
                return false;  
            }
            last_was_slash = true;
            period_count = 0; 
            last_was_period = false;
            alnum_after_slash = false; 
        } else {
            last_was_slash = false;  

            // Handle periods `.`
            if (c == '.') {
                period_count++;
                last_was_period = true;

                if (period_count > 2) {
                    return false;  
                }
            } else {
                // Handle valid alphanumeric characters after `.` or `..`
                if (last_was_period && period_count == 1 && c == '/') {
                    valid_relative_path = true;  // Valid `./` pattern
                } else if (last_was_period && period_count == 2 && c == '/') {
                    valid_relative_path = true;  // Valid `../` pattern
                } else if (period_count > 0 && !valid_relative_path) {
                    return false;  
                }

                period_count = 0;  
                last_was_period = false;

              
                if (isalnum(c)) {
                    contains_alnum = true;

                    if (alnum_after_slash && last_was_period) {
                        return false;  // No period can come after an alphanumeric string
                    }

                    alnum_after_slash = true;
                }
            }
        }
    }

    if (last_was_slash) {
        return false;
    }
    if(last_was_period){
        return false ; 
    }

    if (!contains_alnum && !valid_relative_path) {
        return false;
    }
    return true;
}


void invalid() {
    cout << "invalid" << endl;
    exit(255);
}

ParsedData parse_input(int argc, char* argv[]) {
    ParsedData data;
    int option;

    while ((option = getopt(argc, argv, "K:SRTIE:G:")) != -1) {
        switch (option) {
            case 'K':
                if ( !validate_token(optarg)){
                cout << "invalidK" << endl;
                invalid();
                }
                data.K = optarg;
                break;
            case 'S':
                if (data.R_flag || data.T_flag || data.I_flag){
                cout << "invalidS" << endl;
                invalid();
                }
                data.S_flag = true;
                break;
            case 'R':
                if (data.S_flag || data.T_flag || data.I_flag){
                cout << "invalidR" << endl;
                invalid();
                }
                data.R_flag = true;
                break;
            case 'T':
                if (data.S_flag || data.R_flag || data.I_flag){
                cout << "invalidT" << endl;
                invalid();
                }
                data.T_flag = true;
                break;
            case 'I':
                if (data.S_flag || data.R_flag || data.T_flag ){
                cout << "invalidI" << endl;
                invalid();
                }
                data.I_flag = true;
                break;
            case 'E':
                data.E_flag = true;
                if (!validate_name(optarg)){
                cout << "invalidE" << endl;
                invalid();
                }
                data.E_names.push_back(optarg);
                break;
            case 'G':
                data.E_flag = false;
                if (!validate_name(optarg)){
                cout << "invalidG" << endl;
                invalid();
                }
                data.G_names.push_back(optarg);
                break;
            default:
                invalid();
        }
    }

    if (optind < argc) {
        if (!validate_log_file(argv[optind])) invalid();
        if (optind + 1 != argc) invalid();
        data.log = argv[optind];
    } else {
        invalid();
    }

    if (data.K == nullptr || (!data.S_flag && !data.R_flag && !data.T_flag && !data.I_flag)) {
        invalid();
    }

    if ((data.R_flag || data.T_flag) && data.E_names.empty() && data.G_names.empty()) {
        invalid();
    }
    if ((data.R_flag || data.T_flag) && data.E_names.size() != 1 && data.G_names.size() != 1) {
        invalid();
    }
    if (data.S_flag && (!data.E_names.empty() || !data.G_names.empty())) {
        invalid();
    }

    return data;
}
