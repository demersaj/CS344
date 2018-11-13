//
// Created by Andrew Demers on 11/3/18.
//

#ifndef PROGRAM3_SMALLSH_H
#define PROGRAM3_SMALLSH_H

char** parse_line(char* line);
void check_file_status(int fd, char* file);
void execute_process(char** args);
void cleanup();

#endif //PROGRAM3_SMALLSH_H
