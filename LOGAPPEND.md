[Back](README.md)

logappend
=========
    logappend -T <timestamp> -K <token> (-E <employee-name> | -G <guest-name>) (-A | -L) [-R <room-id>] <log>
    logappend -B <file>

Appends data to the log at the specified timestamp using the authentication token. If the log does not exist, `logappend` will create it. Otherwise it will append to the existing log. 

If the data to be appended to the log is not consistent with the current state of the log, `logappend` prints "invalid," exits with error code 255, and leaves the state of the log unchanged. 

 * `-T` *timestamp* Time the event is recorded. This timestamp is formatted as the number of seconds since the IITGN campus opened and is a non-negative integer (ranging from 1 to 1,073,741,823 inclusively). Time always increases, invoking `logappend` with an event at a time that is prior to the most recent event already recorded is an error. 

 * `-K` *token* Token used to authenticate the log. This token consists of an arbitrary-sized string of alphanumeric (a-z, A-Z, and 0-9) characters. Once a log is created with a specific token, any subsequent appends to that log must use the same token. 

 * `-E` *employee-name* Name of employee. Names are alphabetic characters (a-z, A-Z) in upper and lower case. Names may not contain spaces. Names are case sensitive. Employees and guests can have the same name.

 * `-G` *guest-name* Name of guest. Names are alphabetic characters (a-z, A-Z) in upper and lower case. Names may not contain spaces. Names are case sensitive. Employees and guests can have the same name.

 * `-A` Specifies that the current event is an arrival; can be used with `-E`, `-G`, and `-R`. This option can be used to signify the arrival of an employee or guest to the campus, or, to a specific room with `-R`. If `-R` is not provided, `-A` indicates an arrival to the campus as a whole. No employee or guest should enter a room without first entering the campus. No employee or guest should enter a room without having left a previous room. Violation of either of these conditions implies inconsistency with the current log state and results in `logappend` exiting with an error condition.

 * `-L` Specifies that the current event is a departure, can be used with `-E`, `-G`, and `-R`.This option can be used to signify the departure of an employee or guest from the campus, or, from a specific room with `-R`. If `-R` is not provided, `-L` indicates a deparature from the campus as a whole. No employee or guest should leave the campus without first leaving the last room they entered. No employee or guest should leave a room without entering it. Violation of either of these conditions implies inconsistency with the current log state and results in `logappend` exiting with an error condition.

 * `-R` *room-id* Specifies the room ID for an event. Room IDs are non-negative integer characters with no spaces (ranging from 0 to 1,073,741,823 inclusively). Leading zeros in room IDs are dropped, such that `003`, `03`, and `3` are all equivalent room IDs. A campus has multiple rooms. A complete list of the rooms of the campus is not available and rooms will only be described when an employee or guest enters or leaves one. A room cannot be left by an employee or guest unless that employee or guest has previously entered that room. An employee or guest may only occupy one room at a time. If a room ID is not specified, the event is for the campus. 

 * `log` The path to the file containing the event log. The log's filename may be specified with a string of alphanumeric characters (including underscores and periods). Slashes and periods may be used to reference log files in other directories. If the log does not exist, `logappend` creates it. `logappend` adds data to the log, preserving the history of the log such that queries from [logread](LOGREAD.html) can be answered. If the log file cannot be created due to an invalid path, or any other error, `logappend` prints "invalid" and returns 255. The log file cannot be present multiple times.

 * `-B` *file* Specifies a batch file of commands. *file* contains one or more command lines, not including the `logappend` command itself (just its options), separated by `\n` (newlines). These commands should be processed by `logappend` individually, in order. This allows `logappend` to add data to the file without forking or re-invoking. Option `-B` cannot itself appear in one of these command lines. Commands specified in a batch file include the log name. If a single line in a batch file is invalid, `logappend` prints the appropriate error message for that line and continues processing the rest of the batch file. Here is an [example](EXAMPLES.html) (the last one).

Command line arguments can appear in any order. If the same argument is provided multiple times, the last value is accepted. 

After `logappend` exits, the log specified by the `log` argument is updated. The added information is accessible to the `logread` tool when the token provided to both programs is the same, and not available (e.g., by inspecting the file directly) otherwise. 

Return values and error conditions
----------------------------------
If `logappend` must exit due to an error condition, or if the argument combination is incomplete or contradictory, logappend prints "invalid" to stdout and exits, returning 255. 

If the log to be appended already exists, and the supplied token does not match the expected token, "invalid" is printed to stdout and 255 returned. (When a log file is first created, the supplied token becomes the valid token for all future accesses to that log.)

If `-B` is passed, `logappend` returns 0 as long as the batch file exists and valid arguments are given to the batch command. If the batch file does not exist, it prints "invalid" to stdout and exits, returning 255. 

Some examples of conditions that would result in printing "invalid" and doing nothing to the log:

 * The specified timestamp on the command line is smaller than the most recent timestamp in the existing log 
 * `-B` is used in a batch file
 * The name for an employee or guest, or the room ID, does not correspond to the character constraints
 * Conflicting command line arguments are given, for example both `-E` and `-G` or `-A` and `-L`