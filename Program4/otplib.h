/*************************************************************************************
 * Author:      Andrew Demers
 * Class:       CS 344 - 400
 * Assignment:  Program 4 - OTP
 * Filename:    otplib.h
 *
 * Date:        11/30/18
 * Last Edit:   11/30/18
 *
 * Description: Contain function declarations for all files, as well as some global
 * 				variable.
 ***********************************************************************************/

#ifndef PROGRAM4_OTPLIB_H
#define PROGRAM4_OTPLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <ctype.h>

#define BUFFER_SIZE 250000
#define MAX_PORT_NUM 65535
#define DEBUG 0            // use for debug statements - 1 for on, 0 for off

int print_error(int extStatus, char* message);
void send_file(char* filename, int socketD, int length);
void validate_file(char* filename, char* buffer);

int char_to_int(char c);
char int_to_char(int i);
void encrypt_message(char message[], char key[]);
void decrypt_message(char message[], char key[]);

#endif //PROGRAM4_OTPLIB_H
