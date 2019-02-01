/*************************************************************************************
 * Author:      Andrew Demers
 * Class:       CS 344 - 400
 * Assignment:  Program 4 - OTP
 * Filename:    otplib.h
 *
 * Date:        11/30/18
 * Last Edit:   11/30/18
 *
 * Description: Contain function definitions for all files.
 ***********************************************************************************/

#include "otplib.h"

/*************************************************************************************
 * Function:       	print_error()
 * Description:     Outputs an error message to stderr, then exits with the given
 * 					status. If status is NULL, the program does not exit.
 * Parameters:      int exitStatus - what status the program should exit with
 * 					char* message - message to output to stderr
 * Preconditions:   none
 * Postconditions:  exit program
 * Returns:         0 only if function fails
 ************************************************************************************/
int print_error(int exitStatus, char* message) {
	fprintf(stderr, "%s\n", message);
	if (exitStatus != NULL) {
		exit(exitStatus);
	}
	return 0;
}

/*************************************************************************************
 * Function:       	send_file()
 * Description:     Sends a file to the inputted socket.
 * Parameters:      char* filename - name of the file to send
 * 					int socketFD - socket file descriptor to send file to
 * 					int length - length of the file to send
 * Preconditions:   file exists and can be opened
 * 					socket connect exists
 * Postconditions:  file is sent
 * 					message is outputted to stderr if operation fails
 * Returns:         none
 ************************************************************************************/
void send_file(char *filename, int socketFD, int length) {
	FILE *f = fopen(filename, "r");
	char buffer[BUFFER_SIZE];
	int bytesLeft = 0;

	bzero(buffer, BUFFER_SIZE);

	while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, f)) > 0) {
		if ((bytesLeft = send(socketFD, buffer, length, 0)) < 0) {
			print_error(NULL, "Error: Failed to send file.");
			break;
		}
		bzero(buffer, BUFFER_SIZE);
	}
	if (bytesLeft == BUFFER_SIZE) {
		send(socketFD, "0", 1, 0);
	}
	fclose(f);
}

/*************************************************************************************
 * Function:       	validate_file()
 * Description:     Validates the contents of a file. Must only contain chars A - Z
 * 					and spaces.
 * Parameters:      char* filename - name of the file to validate
 * 					char* buffer - buffer to store contents in
 * Preconditions:   file exists and can be opened
 * 					buffer of appropriate size has been declared
 * Postconditions:  exits program and prints error message if file is not valid
 * Returns:         none
 ************************************************************************************/
void validate_file(char* filename, char* buffer) {
	int ptFile = open(filename, 'r');
	while (read(ptFile, buffer, 1) != 0) {
		if (buffer[0] > 90 || (buffer[0] < 65 && buffer[0] != ' ')) {
			if (buffer[0] != '\n') {
				fprintf(stderr, "Error: %s contains invalid characters.\n", filename);
				exit(1);
			}
		}
	}
}

/*************************************************************************************
 * Function:       	char_to_int()
 * Description:     Converts a char to an int.
 * Parameters:      char c - char to convert
 * Preconditions:   none
 * Postconditions:  none
 * Returns:         converted char as an int
 ************************************************************************************/
int char_to_int(char c) {
	if (c == ' ') {
		return 26;
	} else {
		return (c - 'A');
	}
}

/*************************************************************************************
 * Function:       	int_to_char()
 * Description:     Converts an int to a char.
 * Parameters:      int i - int to convert
 * Preconditions:   none
 * Postconditions:  none
 * Returns:         converted int as a char
 ************************************************************************************/
char int_to_char(int i) {
	if (i == 26) {
		return ' ';
	} else {
		return (i + 'A');
	}
}

/*************************************************************************************
 * Function:       	encrypt_message()
 * Description:     Encrypts given message using a one-time pad-like system.
 * Parameters:      char message[] - message to be encrypted
 * 					char key[] - key to use for encryption
 * Preconditions:   key exists and is valid
 * 					message contains no invalid characters
 * Postconditions:  none
 * Returns:         none
 ************************************************************************************/
void encrypt_message(char message[], char key[]) {
	int i;
	char n, c;

	for (i = 0; message[i] != '\n'; i++) {
		n = (char_to_int(message[i]) + char_to_int(key[i])) % 27;
		message[i] = int_to_char(n);
	}
	message[i] = '\0';
}

/*************************************************************************************
 * Function:       	decrypt_message()
 * Description:     Decrypts given message using a one-time pad-like system.
 * Parameters:      char message[] - message to be decrypted
 * 					char key[] - key to use for decryption
 * Preconditions:   key exists and is valid
 * 					message contains no invalid characters
 * Postconditions:  none
 * Returns:         none
 ************************************************************************************/
void decrypt_message(char message[], char key[]) {
	int i;
	char c;

	for (i = 0; message[i] != '\n'; i++) {
		c = char_to_int(message[i]) - char_to_int(key[i]);
		if (c < 0) {
			c += 27;
		}
		message[i] = int_to_char(c);
	}
	message[i] = '\0';
}