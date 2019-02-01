/*************************************************************************************
 * Author:      Andrew Demers
 * Class:       CS 344 - 400
 * Assignment:  Program 4 - OTP
 * Filename:    otp_enc.c
 *
 * Date:        11/30/18
 * Last Edit:   11/24/18
 *
 * Description: Connects to otp_enc_d and asks it to perform a OTP encryption. All
 *              encryption is actually done in otp_enc_d. Outputs ciphertext to
 *              stdout upon receipt.
 *              Note: All errors are outputted to stderr per program specifications.
 ***********************************************************************************/

#include "otplib.h"

int main(int argc, char *argv[]) {
	int portNum,                  	// port number
			keyFD,             		// file descriptor for key file
			ptFD,             		// file descriptor for plaintext file
			socketFD,              	// socket file descriptor
			optval = 1;        		// option value for setsockopt
	long ptLength,               	// length of plaintext file
			keyLength;              // length of key file
	char buffer[BUFFER_SIZE];      // buffer for plaintext file
	struct sockaddr_in serv_addr;   // holds info for server connection
	struct hostent *server;         // holds struct to store host info
	char auth[] = "e";              // authorization message sent to server

	// ensure there are the proper amount of args
	if (argc < 4 || argc > 5) {
		print_error(1, "Usage: opt_enc <plaintext file> <key file> <port number>");
	}

	// portNum error checking
	portNum = atoi(argv[3]);
	if (portNum < 0 || portNum > MAX_PORT_NUM) {
		print_error(1, "otp_enc error: Port is not valid.");
	}

	// validate plaintext files
	validate_file(argv[1], buffer);

	// create socket and validate
	socketFD = socket(AF_INET, SOCK_STREAM, 0);        // create the socket
	if (socketFD < 0) {
		close(socketFD);
		print_error(2, "otp_enc error: Failed to create socket.");
	}

	setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));    // allow reuse of port

	if (DEBUG) { printf("Created socket.\n"); }

	// set up the server address struct
	memset((char*)&serv_addr, '\0', sizeof(serv_addr));		// clear out the address struct
	serv_addr.sin_family = AF_INET;        // create a network-capable socket
	serv_addr.sin_port = htons(portNum);        // store the port number
	server = gethostbyname("localhost");    // convert machine name into a special form of address
	if (server == NULL) {        // error checking
		print_error(2, "otp_enc error: No such host");
	}
	memcpy((char *) &serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);    // copy in the address


	// connect to otp_enc_d and authenticate
	if (connect(socketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "Error: Failed to connect to otp_enc_d on portNum %d.\n", portNum);
		exit(2);
	}
	if (DEBUG) { printf("otp_enc: Connection to server established.\n"); }

	write(socketFD, auth, sizeof(auth));
	read(socketFD, buffer, sizeof(buffer));

	if (strcmp(buffer, "e") != 0) {		// if authentication message not found
		print_error(2, "otp_enc_error: Failed to authenticate server.");	// output error and close
	}

	if (DEBUG) { printf("Server said: %s\n", buffer); }

	// check key and plaintext file lengths and validate
	keyFD = open(argv[2], O_RDONLY);
	keyLength = lseek(keyFD, 0, SEEK_END);

	ptFD = open(argv[1], O_RDONLY);
	ptLength = lseek(ptFD, 0, SEEK_END);

	if (ptLength > keyLength) {
		print_error(2, "otp_enc error: Key file is too short.");
	}

	// send plaintext and key files
	send_file(argv[1], socketFD, ptLength);
	if (DEBUG) { printf("Plaintext sent to server.\n"); }

	send_file(argv[2], socketFD, keyLength);
	if (DEBUG) { printf("Key sent to server.\n"); }

	bzero(buffer, sizeof(buffer));

	// receive and output plaintext file
	ssize_t n;
	int length = sizeof(buffer);
	const char *p = buffer;
	while (length > 0) {
		n = recv(socketFD, p, length, 0);
		if (n <= 0) {
			break;
		}
		p += n;
		length -= n;
	}

	printf("%s\n", buffer);		 // output plaintext

	close(socketFD);		// cleanup

	return 0;

}