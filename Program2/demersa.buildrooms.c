// Program 2 - Rooms program
// Andrew Demers
// Date: 10/25/18

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

// global variables
enum RoomType {START_ROOM, END_ROOM, MID_ROOM};
char** roomNames[256];


// defines a room struct
struct Room {
    int numConnections;
    char* roomName;
    char* roomType;
    struct room* outBoundConnections[6];
    FILE* currentDir;
};


// creates a directory named <YOUR STUDENT ONID USERNAME>.rooms.<PROCESS ID OF ROOMS PROGRAM>
char* createDir() {
    char* dirName = malloc(255 * sizeof(char)); // dynamically allocate memory for strings

    int p_id = getpid();    // get process ID

    sprintf(dirName, "%s%d", "./demersa.rooms.", p_id); // combines the ONID username and the PID

    mkdir(dirName, 0755);   // create directory

    return dirName;
}

// generates random room names
char* genRooms() {
    char* names[] = {"Bar", "Pub", "Taproom", "Lounge", "Club", "Saloon", "Tavern", "Ratskeller", "Alehouse", "Canteen"};
    int namesCount = 10;
    //char** roomNames = malloc(10 * sizeof(char*));
    int i =0;

    for (i = 0; i < 10; i++) {
        roomNames[i] = malloc(30 * sizeof(char));
    }

    // loops through array 7 times and pick a random name each time
    for (i = 0; i < 7; i++) {
        int index = rand() % (namesCount - i);  // random index between 0 and last element in array
        roomNames[i] = names[index];    // store name
        names[index] = names[namesCount - i - 1];   // overwrite with the last element in array
    }

    return roomNames;   // return array of chosen names

}

// creates seven different room files
FILE** createFiles(char* dirName) {
    // allocate memory
    FILE** filePtr = malloc(7 * sizeof(FILE**));

    char filePath[256];

    // create file for each room name
    for (int i =0; i < 7; i++) {
        sprintf(filePath, "%s/%s.txt", dirName, roomNames[i]);
        filePtr[i] = fopen(filePath, "a+");
        printf(filePath);
        printf("\n");
    }
  return filePtr;
}



int main() {
    srand(time(NULL));      // initialize random seed
    char* dirName = createDir();
    char** roomNames = genRooms();
    FILE** filePtr = createFiles(dirName);
}
