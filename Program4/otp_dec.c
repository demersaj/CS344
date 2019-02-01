/*************************************************************************************
 * Author:      Andrew Demers
 * Class:       CS 344 - 400
 * Assignment:  Program 4 - OTP
 * Filename:    otp_dec.c
 *
 * Date:        11/30/18
 * Last Edit:   11/24/18
 *
 * Description: Connects to otp_edec_d and asks it to perform a OTP decryption. All
 *              decryption is actually done in otp_enc_d. Outputs plaintext to
 *              stdout upon receipt.
 *              Note: All errors are outputted to stderr per program specifications.
 ***********************************************************************************/


#include "otplib.h"

int main(int argc, char *argv[]) {
	int socketFD,
			portNum,
			fd,
			optval = 1,
			cipherFD,
			keyFD;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[BUFFER_SIZE],
			auth[] = "d";
	long cipherLength,
			keyLength;

	// ensure there are the proper amount fo args
	if (argc < 4 || argc > 5) {
		print_error(1, "otp_dec usage: otp_dec <cipher file> <key file> <port number>");
	}

	// ensure port is valid
	portNum = atoi(argv[3]);
	if (portNum < 0 || portNum > MAX_PORT_NUM) {
		print_error(1, "otp_dec error: Port is not valid.");
	}

	// validate cipher file
	validate_file(argv[1], buffer);

	bzero(buffer, sizeof(buffer));        // clear out buffer

	// create socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0) {
		close(socketFD);
		print_error(NULL, "otp_dec error: Failed to create socket.");
	}

	if (DEBUG) { printf("Created socket/\n"); }

	setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));    // allow reuse of port

	// set up server address struct
	memset((char *) &serv_addr, '\0', sizeof(serv_addr));        // clear out the address struct
	serv_addr.sin_family = AF_INET;        // create a network-capable socket
	serv_addr.sin_port = htons(portNum);        // store the port number
	server = gethostbyname("localhost");        // convert machine name into a special form of address
	if (server == NULL) {        // error checking
		print_error(2, "otp_dec error: No such host.");
	}

	memcpy((char *) &serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);    // copy in the address

	// connect to otp_dec_d and authenticate
	if (connect(socketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		print_error(2, "otp_dec error: Failed to connect to otp_enc_d.");
	}
	if (DEBUG) { printf("Connection to server established.\n"); }
	write(socketFD, auth, sizeof(auth));
	read(socketFD, buffer, sizeof(buffer));

	if (strcmp(buffer, "d") != 0) {
		print_error(2, "otp_dec_error: Failed to authenticate server.");
	}

	if (DEBUG) { printf("Server said: %s\n", buffer); }

	// open and validate key and cipher files
	keyFD = open(argv[2], O_RDONLY);
	if (keyFD < 0) {
		print_error(NULL, "Failed to access key file.");
	}
	keyLength = lseek(keyFD, 0, SEEK_END);


	cipherFD = open(argv[1], O_RDONLY);
	if (cipherFD < 0) {
		print_error(NULL, "Failed to access cipher file.");
	}
	cipherLength = lseek(cipherFD, 0, SEEK_END);
	if (cipherLength > keyLength) {
		print_error(1, "otp_dec error: Key is too short.");
	}

	// send cipher and key files
	send_file(argv[1], socketFD, cipherLength);
	send_file(argv[2], socketFD, keyLength);

	if (DEBUG) { printf("Files sent to server.\n"); }

	bzero(buffer, sizeof(buffer));

	// receive and output cipherfile
	int length = sizeof(buffer);
	ssize_t n;
	const char *p = buffer;
	while (length > 0) {
		n = recv(socketFD, p, length, 0);
		if (n <= 0) {
			break;
		}
		p += n;
		length -= n;
	}


	printf("%s\n", buffer); 	// output cipher file

	close(socketFD);		// cleanup

	return 0;
}
