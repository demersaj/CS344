//
// Created by Andrew Demers on 11/3/18.
//

#ifndef PROGRAM3_SMALLSH_H
#define PROGRAM3_SMALLSH_H

void showStatus(int status);
void catchSIGINT(int signal);
void catchSigStop(int signal);
char** parseLine(char* line);

#endif //PROGRAM3_SMALLSH_H
