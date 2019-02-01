/*************************************************************************************
 * Author:      Andrew Demers
 * Class:       CS 344 - 400
 * Assignment:  Program 4 - OTP
 * Filename:    otp_enc_d.c
 *
 * Date:        11/30/18
 * Last Edit:   11/28/18
 *
 * Description: Connects to otp_enc and encrypt_message a message using modular
 *addition. It receives key and plaintext files via a socket connection. After
 *the message is encrypted, it sends the ciphertext back to the client.
 ***********************************************************************************/

#include "otplib.h"

int main(int argc, char *argv[]) {
	int socketFD,  					// file descriptor for listening socket
			newSocketFD,  			// file descriptor for new socket connections
			portNum,     			// port number
			optval = 1, 			// option value for setsockopt
			status,    			    // waitpid status
			bytesLeft,       		// number of bytes left to be read
			bytesRead,         		// number of bytes read so far
			i,              		// iterator
			newLines;               // number of new lines read
	socklen_t clientLength;         // size of the client message
	char buffer[BUFFER_SIZE],       // buffer to hold sent messages
			message[BUFFER_SIZE],   // buffer to hold messages
			auth[] = "e",           // authentication message
			invalid[] = "invalid";  // invalid connection message
	struct sockaddr_in serv_addr,   // hold info for server connection
			client_address;         // hold info for client connection
	pid_t pid;                      // process id
	char *c,                        // helper to keep track of buffer location
			*keyStart;              // location of the key file

	// check number of args
	if (argc < 2 || argc > 3) {
		print_error(1, "Usage: otp_enc_d <port>");
	}

	// set up socket and validate
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // create the socket
	if (socketFD < 0)
		print_error(1, "otp_enc_d error: Failed to open socket.");

	setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));	// allows reuse of sockets

	// set up the address struct for the server
	bzero((char *) &serv_addr, sizeof(serv_addr)); // clear out the address struct
	portNum = atoi(argv[1]);                      // get port number
	serv_addr.sin_family = AF_INET;         // create a network-capable socket
	serv_addr.sin_addr.s_addr = INADDR_ANY; // any address is allowed to connect
	serv_addr.sin_port = htons(portNum);    // store port number

	// bind the socket and enable it to begin listening
	if (bind(socketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) <
		0) // bind the socket to a port
		print_error(1, "otp_enc_d error: Unable to bind socket");

	listen(socketFD, 5); // wait for client to connect - can receive up to 5 connections

	while (1) {
		clientLength = sizeof(client_address);

		newSocketFD = accept(socketFD, (struct sockaddr *) &client_address, &clientLength); // accept incoming connection
		if (newSocketFD < 0) {
			print_error(1, "otp_enc_d error: Unable to accept connection.");
		}

		// spawn off a process whenever a client connects
		pid = fork();

		// error spawning child
		if (pid < 0) {
			print_error(1, "otp_enc_d error: Fork error.");
		}

		// chjld process
		if (pid == 0) {

			// reset variables
			bzero(buffer, sizeof(buffer));
			bytesLeft = sizeof(buffer);
			bytesRead = 0;
			c = buffer;
			newLines = 0;

			if (DEBUG) {
				printf("Connection established with client.\n");
			}

			// authenticate client
			read(newSocketFD, buffer, sizeof(buffer) - 1);

			if (DEBUG) {
				printf("Client said: %s\n", buffer);
			}

			if (strcmp(buffer, "e") != 0) { // if authentication message is not found
				write(newSocketFD, invalid, sizeof(invalid));
				print_error(2, "otp_enc_d error: Client not authorized.");
			} else {
				// connection authorized - send confirmation
				write(newSocketFD, auth, sizeof(auth));
			}

			bzero(buffer, sizeof(buffer));

			while (1) {
				bytesRead = read(newSocketFD, c, bytesLeft);

				// error checking
				if (bytesRead < 0) {
					print_error(NULL, "otp_enc_d error: Failed to receive message.");
				}

				// finished receiving message
				if (bytesRead == 0) {
					break;
				}

				// read message
				for (i = 0; i < bytesRead; i++) { // search for newlines in buffer
					if (c[i] == '\n') {
						newLines++;
						if (newLines == 1) { // first newline indicates start of key
							keyStart = c + i + 1;
						}
					}
				}

				if (newLines == 2) { 	// second newline signals end
					break;
				}

				c += bytesRead;
				bytesLeft -= bytesRead;
			}

			bzero(message, sizeof(message));

			strncpy(message, buffer, keyStart - buffer); 	// separate message and key

			encrypt_message(message, keyStart);	 // encrypt the message
			//write(newSocketFD, message, sizeof(message));		 // send encrypted message to client
			int len = sizeof(message);
			ssize_t n;
			const char *p = message;
			while (len > 0) {
				n = send(newSocketFD, p, len, 0);
				if (n<=0)
					break;
				p+=n;
				len -=n;
			}

		}
		close(newSocketFD); 	// clean up

		while (pid > 0) { 		// wait for children to finish
			pid = waitpid(-1, &status, WNOHANG);
		}
	}
	// end loop
	close(socketFD);
	exit(0);
}
