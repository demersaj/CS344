/*************************************************************************************
 * Author:      Andrew Demers
 * Class:       CS 344 - 400
 * Assignment:  Program 3 - smallsh
 *
 * Date:        11/14/18
 * Last Edit:   11/04/18
 *
 * Description: Creates a small shell in C. Runs commands similar to the Bash shell.
 *              Supports three built in commands: exit, cd, and status. Allows comments.
 *              Supports both foreground and background processes, controllable by the
 *              command line and by receiving signals.
 ***********************************************************************************/

#include "smallsh.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// global variables
#define MAX_CHARS 2048      // maximum of 2048 characters per line
#define MAX_ARGS 512        // maximum of 512 arguments
#define DELIM " \n"         // list of string tokenizer delimiters to look for (space and new line)

typedef enum {false = 0, true = 1} bool;    // allows use of bools

bool isBgProcess;       // true if process should run in the background
bool redirectInput;     // true if user requested to redirect the input
bool redirectOutput;    // true if user requested to redirect the output
char* inputFile;        // hold input filename
char* outputFile;       // hold output filename
int numArgs;            // count of the number of args provided

int main(int argc, char **argv) {
    bool exit = false;
    char* line = NULL,
        *token;
    ssize_t buffer = MAX_CHARS;
    int i,
        processCount = 0;
    char** args;            // array of pointers to the strings in the arg array
    pid_t processes[128],   // holds list of open processes
        spawnpid;
    char exitStatus[512] = "No previous foreground process!\n";

    // instance of sigaction struct for background processes
    struct sigaction background;

    do {

        // get input from user
        printf(": ");
        fflush(stdout);     // prompt user and flush buffer to avoid errors
        getline(&line, &buffer, stdin);     // read in a line from the user via stdin
        fflush(stdin);      // flush buffer

        args = parseLine(line);

        // check if a comment is entered
        if (strncmp(args[0], "#", 1) == 0)  // checks if first character is a #
            exit = false;       // keep going

        // built in exit command
        else if (strcmp(args[0], "exit") == 0) {
            // if there is more than just the exit arg
            if (numArgs > 1 || args[1]) {
                printf("Error: exit - too many arguments provided.\n");
                fflush(stdout);

                // free memory
                for (i = 0; i < MAX_ARGS; i++)
                    free(args[i]);
            }
            else {
                exit = true;        // exit program
                printf("Now killing all processes.\n");
                fflush(stdout);
                // TODO : kill all processes - use SIGTERM
            }
        }

        // built in cd command
        else if (strcmp(args[0], "cd") == 0) {
            // if no args provided, change to the HOME directory
            if (numArgs == 1)
                chdir(getenv("HOME"));
            // else change to dir provided
            else
                chdir(args[1]);
            // TODO: does this support absolute and relative paths?
        }

        // built in status command
        else if (strcmp(args[0], "status") == 0) {
            printf("%s\n", exitStatus);
            fflush(stdout);
        }

        // not a built-in command - pass arg to Bash to interpret
        else {
            spawnpid = fork();  // fork to start new process

            // error checking
            switch (spawnpid) {
                case -1:
                    perror("Fork error!");
                    return(EXIT_FAILURE);
                    break;
                case 0:
                    if (isBgProcess == false) {     // if this is a foreground process

                    }
                default:
                    return 0;

            }
        }

    }
        while (!exit);

    return 0;
}

/*************************************************************************************
 * Function:        parseLine()
 * Description:     Takes input from main and parses it to determine if
 *                  the user requested I/O redirection, to run a background process,
 *                  or wants to use I/O files. Returns the parsed command.
 * Parameters:      char* line - input from the user
 * Preconditions:   MAX_ARGS is defined.
 * Postconditions:  global commands are set.
 * Returns:         parsed command as an array of char arrays
 ************************************************************************************/
char** parseLine(char* line) {
    char** argPtr;
    char* token;
    int bufferSize = MAX_ARGS;
    bool finished = false;

    // resets global flags
    isBgProcess = false;
    redirectInput = false;
    redirectOutput = false;
    numArgs = 0;

    argPtr = malloc(bufferSize * sizeof(char*));
    token = strtok(line, DELIM);      // tokenizes the string to look for args

    while (token != NULL && finished == false) {
        if (strcmp(token, "&") == 0) {      // if background process is requested
            isBgProcess = true;
            finished = true;        // end parsing
        }
        else if (strcmp(token, "<") == 0) {     // input redirection
            redirectInput = true;
        }
        else if (strcmp(token, ">") == 0) {     // output redirection
            redirectOutput = false;
        }
        else {                          // token is an arg
            if (redirectInput)          // set input file
                inputFile = token;
            else if (redirectOutput)    // set output file
                outputFile = token;
            else {                      // copy arg list
                argPtr[numArgs] = token;
                numArgs++;
                argPtr[numArgs] = NULL;     // null terminate
            }

            token = strtok(NULL, DELIM);  // get the next token

        }
    }
        return argPtr;
}

