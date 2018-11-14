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
#include <fcntl.h>

// global variables
#define MAX_CHARS   2048    // maximum of 2048 characters per line
#define MAX_ARGS    512     // maximum of 512 arguments
#define DELIM       " \n"   // allows both space and new line to be used as delimiters

typedef enum {false = 0, true = 1} bool;    // allows use of bools

bool isBgProcess,       // true if process should run in the background
    redirectInput,      // true if user requested to redirect the input
    redirectOutput,     // true if user requested to redirect the output
    fgOnlyMode = false; // foreground only mode. only set if SIGSTP is received.
char* inputFile,        // hold input filename
    *outputFile;        // hold output filename
int numArgs,            // count of the number of args provided
    status,             // status of current process
    redirect;           // input or output redirection: 1 for input, 2 for output
int shellPid;           // hold pid of shell itself


int main(int argc, char **argv) {
    bool exit = false;      // exit condition
    char* line = NULL;
    ssize_t buffer = MAX_CHARS;
    int i,                  // number of processes
        processCount = 0;   // number of background processes running
    char** args;            // array of pointers to the strings in the arg array
    pid_t cpid,             // child process
        processes[128];     // array of running processes
    char exitStatus[256] = "No previous foreground process!\n";

    // instances of sigaction struct for default, ignore, and SIGTSTP actions
    struct sigaction default_action = {0}, ignore_action = {0}, SIGTSTP_action = {0};
    default_action.sa_handler = SIG_DFL;     // default action
    ignore_action.sa_handler = SIG_IGN;     // ignore action
    sigaction(SIGINT, &ignore_action, NULL);     // prevent interruption of shell and bg processes

    // SIGTSTP handler to toggle foreground only mode
    SIGTSTP_action.sa_handler = handle_SIGTSTP;
    SIGTSTP_action.sa_flags = 0;
    sigfillset(&(SIGTSTP_action.sa_mask));
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    // get pid of shell to be used later
    shellPid = getpid();

    do {
        // clean up zombie processes
        cleanup();

        // get input from user
        printf(": ");
        fflush(NULL);       // prompt user and flush buffer to avoid errors
        getline(&line, &buffer, stdin);     // read in a line from the user via stdin
        fflush(NULL);      // flush buffer

        args = parse_line(line);

        //check if blank line is entered
        if (args[0] == NULL) {
            continue;         // ignore and keep going
        }

            // check if a comment is entered
        else if (strncmp(args[0], "#", 1) == 0)  // checks if first character is a #
            continue;       // ignore and keep going

            // built in exit command
        else if (strcmp(args[0], "exit") == 0) {
            // if there is more than just the exit arg
            if (numArgs > 1 || args[1]) {
                printf("Error: exit - too many arguments provided.\n");
                fflush(NULL);

                // free memory
                for (i = 0; i < MAX_ARGS; i++)
                    free(args[i]);
            } else {
                exit = true;        // exit program
                printf("Now killing all processes.\n");
                fflush(NULL);
                for (i = 0; i < processCount; i++)
                    kill(processes[processCount], SIGTERM);     // terminates each background process in array
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
        }

            // built in status command
        else if (strcmp(args[0], "status") == 0) {
            printf("%s\n", exitStatus);
            fflush(NULL);
        }

            // not a built-in command - pass arg to Bash to interpret
        else {
            cpid = fork();  // fork to start new process

            // error checking
            switch (cpid) {
                // error
                case -1:
                    perror("Fork error!");
                    return (1);
                    break;

                    // child process
                case 0:
                    if (!isBgProcess) {     // if foreground process
                        sigaction(SIGINT, &default_action, NULL);   // allow foreground process to be interrupted
                    }
                    execute_process(args);    // execute process using inputted args
                    break;

                    // parent process continues here
                default:
                    if (isBgProcess) {      // if background process
                        processes[processCount] = cpid;
                        processCount++;
                        printf("background pid is: %d\n", cpid);
                        fflush(NULL);
                    } else {      // foreground process
                        waitpid(cpid, &status, 0);      // wait for last child process to finish

                        if (WIFEXITED(status))
                            sprintf(exitStatus, "Error. Exit value: %d", WEXITSTATUS(status));
                        if (WIFSIGNALED(status)) {
                            sprintf(exitStatus, "Terminated by signal: %d.", WTERMSIG(status));
                            printf("%s\n", exitStatus);
                            fflush(NULL);
                        }
                    }

                    break;
            }
        }

    }
        while (!exit);

    return 0;
}

/*************************************************************************************
 * Function:        parse_line()
 * Description:     Takes input from main and parses it to determine if
 *                  the user requested I/O redirection, to run a background process,
 *                  or wants to use I/O files. Returns the parsed command.
 * Parameters:      char* line - input from the user
 * Preconditions:   MAX_ARGS is defined.
 * Postconditions:  Global commands are set.
 * Returns:         Parsed command as an array of char pointers.
 ************************************************************************************/
char** parse_line(char* line) {
    char** argPtr;      // hold command line arguments
    char* token,        // hold tokenized string
        *num,           // hold converted string
        *tok,
        *buffer;
    int i;

    // resets global flags
    isBgProcess = false;
    redirectInput = false;
    redirectOutput = false;
    numArgs = 0;
    status = 0;
    redirect = 0;

    argPtr = malloc(sizeof(char*) * MAX_ARGS);


    // replace $$ with pid
    if (strstr(line, "$$") != NULL) {
        num = integer_to_string(shellPid);
        tok = strtok(line, " ");
        buffer = malloc(sizeof(char*) * MAX_ARGS);

        while (tok != NULL) {
            if (strstr(tok, "$$") != NULL)
                strcat(buffer, num);
            else
                strcat(buffer, tok);
            strcat(buffer, " ");
            tok = strtok(NULL, " ");
        }
        strcpy(line, buffer);

    }

    token = strtok(line, DELIM);        // tokenizes the string to look for args

    // prevents errors with blank lines
    if (token == NULL)
        argPtr[0] = NULL;

    while (token != NULL && numArgs < MAX_ARGS) {
        if (strcmp(token, "<") == 0) {     // input redirection flag
            redirectInput = true;
            redirect = 1;
        }
        else if (strcmp(token, ">") == 0) {     // output redirection flag
            redirectOutput = true;
            redirect = 2;
        }
        else {                          // token is an arg
            if (redirect == 1)          // set input file
                inputFile = token;
            else if (redirect == 2)    // set output file
                outputFile = token;

            else {                      // copy arg list into array
                argPtr[numArgs] = token;
                numArgs++;
                argPtr[numArgs] = NULL; // null terminate
            }

        }
        token = strtok(NULL, DELIM);  // get the next token
    }
    if (strcmp(argPtr[numArgs - 1], "&") == 0 && fgOnlyMode == false) {          // if background process is requested
        isBgProcess = true;
        argPtr[numArgs - 1] = NULL;
        numArgs--;
    }
    // removes & if process is not supposed to run in background
    for (i = 0; i < numArgs; i++) {
        if (strcmp(argPtr[i], "&") == 0 && fgOnlyMode == true) {
            argPtr[i] = argPtr[i + 1];
        }
    }
    return argPtr;
}

/*************************************************************************************
 * Function:        check_file_status()
 * Description:     Takes input from passed in file descriptor. If there is an error
 *                  opening the file (fd == -1), the function prints an error and exits.
 * Parameters:      int fd - file descriptor from opening filepath.
 *                  char* file - string that holds filepath
 * Preconditions:   An attempt was made to open a file.
 * Postconditions:  If there is an error, it is outputted to stderr.
 * Returns:         none
 ************************************************************************************/
void check_file_status(int fd, char* file) {
    if (fd == -1) {
        fprintf(stderr,"Error with file: %s.\n", file);
        exit(1);
    }
}

/*************************************************************************************
 * Function:        execute_process()
 * Description:     Executes the given command line arguments, using the global flags
 *                  as needed.
 * Parameters:      char** args - pointer to a list of arguments
 * Preconditions:   All args are parsed and global flags are set
 * Postconditions:  The provided command is executed.
 * Returns:         none
 ************************************************************************************/
 void execute_process(char** argPtr) {
     int fd1, fd2;      // file descriptors

    // if user requested input redirection
    if (redirectInput) {
        fd1 = open(inputFile, O_RDONLY);

        // error checking
        check_file_status(fd1, inputFile);
        check_file_status(dup2(fd1, 0), inputFile);  // redirect stdin - 0 now points to fd1

        close(fd1);

    }
    // if user requests process to be run in background
    else if (isBgProcess) {
        fd1 = open("/dev/null", O_RDONLY); // redirect BG commands as stated in specs

        // error checking
        check_file_status(fd1, NULL);
        check_file_status(dup2(fd1, 0), NULL);

        close(fd1);
    }

    // if user requests output redirection
    if (redirectOutput) {
        fd2 = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);   // open file for writing only, create file if it doesn't exist

        // error checking
        check_file_status(fd2, outputFile);
        check_file_status(dup2(fd2, 1), outputFile);  // redirect stdout - 1 now points to fd2

        close(fd2);
    }


    // replace currently running process
    execvp(argPtr[0], argPtr);

    // error checking - will only run if execvp() fails
    fprintf(stderr, "%s", argPtr[0]);      // redirects stdout as stated in specs
    fflush(NULL);
    perror(" ");
    exit(1);

 }

/*************************************************************************************
* Function:        cleanup()
* Description:     Cleans up the zombie processes and handles background status.
* Parameters:      none
* Preconditions:   none
* Postconditions:  status is outputted to stdout
* Returns:         none
************************************************************************************/
void cleanup() {
    pid_t pid;

    // wait for the foreground process to finish
    pid = waitpid(-1, &status, WNOHANG);

    if (pid > 0) {
        if (WIFEXITED(status)) {
            printf("background pid %d is done: exit value %d\n", pid, WEXITSTATUS(status));
            fflush(NULL);
        }
        else if (WIFSIGNALED(status)) {
            printf("Background pid %d is done. terminated by signal %d\n", pid, WTERMSIG(status));
            fflush(NULL);
        }
    }
}

/*************************************************************************************
* Function:        handle_SIGTSTP()
* Description:     SIGTSTP handler to catch signal sent to program and toggle foreground
*                  only mode. If foreground only mode is not set, the program enters
*                  foreground only mode. Otherwise, it exits foreground only mode.
* Parameters:      int signal - signal number
* Preconditions:   fgOnlyMode is set to either true or false
* Postconditions:  status statement outputted to stdout, fgOnlyMode is set to true
* Returns:         none
************************************************************************************/
void handle_SIGTSTP(int signal) {
    char* output;

    if (!fgOnlyMode) {
        fgOnlyMode = true;

        output = "Entering foreground only mode (& is now ignored)\n";
        write(STDOUT_FILENO, output, strlen(output));   // must use write since printf is not allowed
    }
    else {
        output = "Exiting foreground only mode\n";
        write(STDOUT_FILENO, output, strlen(output));   // must use write since printf is not allowed

        fgOnlyMode = false;
    }
}

/*************************************************************************************
* Function:        integer_to_string()
* Description:     Converts the given integer to a string.
* Parameters:      int x - the integer to be converted
* Preconditions:   none
* Postconditions:  none
* Returns:         the converted int as a string
* Reference:       https://stackoverflow.com/questions/36274902/convert-int-to-string-in-standard-c
************************************************************************************/
char* integer_to_string(int x) {
    char* buffer = malloc(sizeof(char) * sizeof(int) * 4 + 1);
    if (buffer)
        sprintf(buffer, "%d", x);

    return buffer;  // caller is expected to invoke free() on this buffer to release memory
}
