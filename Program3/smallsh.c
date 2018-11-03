/* Author: Andrew Demers
 * CS 344 - 400
 * Program 3 - smallsh
 * Date: 11/14/18
 * Last Edit: 11/03/18
 * Desc: Creates a small shell in C. Runs commands similar to the Bash shell. Supports three built in
 *      commands: exit, cd, and status. Allows comments. Supports both foreground and background processes,
 *      controllable by the command line and by receiving signals. */

#include "smallsh.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// globals
#define MAX_CHARS 2048      // maximum of 2048 characters per line
#define MAX_ARGS 512        // maximum of 512 arguments

int main(int argc, char **argv) {
    readLine();
    return 0;
}

/* readLine() reads in a line from the user via stdin using getline. The maximum size is specified
 *      with the MAX_CHARS variable.
 * Preconditions: MAX_CHARS is defined globally
 * Postconditions: none
 * Receives: none
 * Returns: input line as a char* */

char *readLine() {
    printf(":");
    fflush(stdin);      // flush out the output buffer

}