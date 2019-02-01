/*************************************************************************************
 * Assignment:  Program 4 - OTP
 * Filename:    keygen.c
 * Author:      Andrew Demers
 * Class:       CS 344 - 400
 *
 * Date:        11/30/18
 * Last Edit:   11/23/18
 *
 * Description: Creates a key gen file of a specified length.
 ***********************************************************************************/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char *argv[]) {
    int keyLength;      // length of the key

    srand((unsigned) time(0));     // use current time as seed

    // ensure there are enough args
    if (argc < 2) {
        fprintf(stderr, "Usage: keygen keyLength\n");   // output to stderr per specs
        exit(1);
    }

    // ensure length is at least 1
    keyLength = atoi(argv[1]);  // convert string to integer
    if (keyLength < 1) {
        fprintf(stderr, "Error: invalid keyLength\n");  // output to stderr per specs
    }

    // generate the key file
    generate_key_file(keyLength);

    return 0;
}

/*************************************************************************************
 * Function:        random_int()
 * Description:     Returns a random integer between the given min and max.
 * Parameters:      int min - minimum desired integer
 *                  int max - maximum desired integer
 * Preconditions:   min and max ints are passed in
 * Postconditions:  none
 * Returns:         random int between int min and int max
 ************************************************************************************/
int random_int(int min, int max) {
    return rand() % (max - min + 1) + min;
}

/*************************************************************************************
 * Function:        generateKeyFile()
 * Description:     Generates a key file filled with random ASCII chars between 64 and
 *                  90 (A - Z + @). The @ is used to represent a space, which is
 *                  required by the specs.
 * Parameters:      int min - minimum desired integer
 *                  int max - maximum desired integer
 * Preconditions:   none
 * Postconditions:  set of random chars is outputted to stdout
 * Returns:         none
 ************************************************************************************/
 void generate_key_file(int length) {
     char randomLetter;     // ptBuffer for random letters
     int i;                 // iterator

    // loop to generate random string of specified length
    for (i = 0; i < length; i++) {
        randomLetter = (char) random_int(64, 90);   // cast int to char

        // replace @ with space
        if (randomLetter == 64)
            randomLetter = 32;

        printf("%c", randomLetter);
    }

    // newline per specs
    printf("\n");
 }