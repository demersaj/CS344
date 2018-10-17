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

#define FALSE 0
#define TRUE 1

// global variables
enum RoomType {START_ROOM, END_ROOM, MID_ROOM};
char** roomNames[256];


// defines a room struct
struct Room {
    int numConnections;
    char* roomName;
    enum RoomType roomType;
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
    char* names[] = {"Bar", "Pub", "Taproom", "Lounge", "Club", "Saloon", "Tavern", "Drinkery", "Alehouse", "Canteen"};
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

        //printf("%s\n", filePath);   // TODO: testing only - DELETE
    }
  return filePtr;
}

// initializes room structs
struct Room* initRooms(char** roomNames) {
    struct Room** roomList = malloc(7 * sizeof(struct Room*));  // allocate memory for Rooms

    // set names to names picked above
    for (int i =0; i < 7; i++) {
        roomList[i] = malloc(sizeof(struct Room));
        roomList[i]->roomName = malloc(32 * sizeof(char));
        roomList[i]->roomType = malloc(32 * sizeof(char));
        roomList[i]->roomName = roomNames[i];       // set room name
        roomList[i]->numConnections = 0;        // make sure num connections = 0

        //printf("%s\n", roomList[i]->roomName);  // TODO: testing - DELETE

        // set connections to NULL
        for (int j = 0; j < 7; j++) {
            roomList[i]->outBoundConnections[j] = NULL;
        }

        // set room types
        if (i == 0) {   // for first room
            roomList[i]->roomType = START_ROOM;
        }
        else if (i == 6) {  // for last room
            roomList[i]->roomType = END_ROOM;
        }
        else
            roomList[i]->roomType = MID_ROOM;
    }
    return roomList;

}

/* NOTE: some of the code outlined below is from the required reading 2.2: Program Outlining in Program 2 */

// returns true if all rooms have 3 to 6 outbound connections, false otherwise
int isGraphFull(struct Room** roomList) {
    for (int i =0; i < 7; i++) {
        if (roomList[i]->numConnections >= 3 && roomList[i]->numConnections <= 6) {
            return TRUE;
        }
        else
            return FALSE;
    }
}

/*
// adds a random, valid outbound connection from one Room to another Room
 void addRandomConnection() {
    Room A;
    Room B;

    while(true) {
        A = getRandomRoom();

        if (canAddConnectionFrom(A) == true)
            break;
    }

    do {
        B = GetRandomRoom();
    }
    while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);

    ConnectRoom(A, B);  // TODO: Add this connection to the real variables,
    ConnectRoom(B, A);  //  because this A and B will be destroyed when this function terminates

}
*/
// returns a random Room, does not validate if connection can be added
struct Room* getRandomRoom(struct Room** roomList) {
    int randomNum = rand() % 7;     // returns random number between 0 and 6
    printf(roomList[randomNum]->roomName);

    return roomList[randomNum];

}

// returns true if a connection can be added from room x (< 6 outbound connections), false otherwise
int canAddConnectionFrom(struct Room* room) {
    return 0;
}

// returns true if a connection from Room x to Room y already exists, false otherwise
int connectionAlreadyExists(struct Room* x, struct Room* y) {
    return 0;
}

// connects Rooms x and y together, does not check if this connection is valid
void connectRoom(struct Room* x, struct Room* y) {

}

// returns true if Rooms x and y are the same Room, false otherwise
int isSameRoom(struct Room* x, struct Room* y) {
    return 0;
}


int main() {
    srand(time(NULL));      // initialize random seed
    char* dirName = createDir();
    char** roomNames = genRooms();
    FILE** filePtr = createFiles(dirName);
    initRooms(roomNames);
    //getRandomRoom(roomNames);     // TODO: why doesn't this work??
}
