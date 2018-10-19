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
#define MAX_NUM_ROOMS 7
#define TOTAL_NUM_RUMS 10
enum RoomType {START_ROOM, END_ROOM, MID_ROOM};
char** roomNames[256];
typedef enum {FALSE = 0, TRUE = 1} bool;


// defines a room struct
struct Room {
    int numConnections;
    char* roomName;
    enum RoomType roomType;
    struct room** outBoundConnections[6];    // stores pointer to room connections
    FILE* currentDir;
};

// room global
struct Room roomList[TOTAL_NUM_RUMS];       // global variable to store rooms


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
    int i =0;

    // loops through array 7 times and pick a random name each time
    for (i = 0; i < 7; i++) {
        int index = rand() % (namesCount - i);  // random index between 0 and last element in array
        roomList[i].roomName = names[index];    // store name
        names[index] = names[namesCount - i - 1];   // overwrite with the last element in array

       // printf("%s\n", roomList[i].roomName);       // TODO: testing only - delete
    }
}

// creates seven different room files
FILE** createFiles(char* dirName) {
    // allocate memory
    FILE** filePtr = malloc(7 * sizeof(FILE**));

    char filePath[256];

    // create file for each room name
    for (int i =0; i < 7; i++) {
        sprintf(filePath, "%s/%s.txt", dirName, roomList[i].roomName);
        filePtr[i] = fopen(filePath, "a+");

        //printf("%s\n", filePath);   // TODO: testing only - DELETE
    }
  return filePtr;
}

// initializes room structs
struct Room* initRooms() {

    // set names to names picked above
    for (int i = 0; i < MAX_NUM_ROOMS; i++) {
        roomList[i].roomType = malloc(32 * sizeof(char));
        roomList[i].numConnections = 0;        // make sure num connections = 0

        // set connections to NULL
        for (int j = 0; j < 7; j++) {
            roomList[i].outBoundConnections[j] = NULL;
        }

        // set room types
        if (i == 0) {   // for first room
            roomList[i].roomType = START_ROOM;
        }
        else if (i == 6) {  // for last room
            roomList[i].roomType = END_ROOM;
        }
        else
            roomList[i].roomType = MID_ROOM;
    }

}

/* NOTE: some of the code outlined below is from the required reading 2.2: Program Outlining in Program 2 */

// returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool isGraphFull() {
    bool full = FALSE;

    for (int i =0; i < 7; i++) {
        if (roomList[i].numConnections >= 3 && roomList[i].numConnections <= 6) {
            full = TRUE;
        }
        else
            full = FALSE;
    }
    return full;

}

// returns a random Room, does not validate if connection can be added
struct Room getRandomRoom() {
    int randomNum = rand() % 7;     // returns random number between 0 and 6

    //printf(roomList[randomNum].roomName);  // TODO: testing - DELETE
    //printf("\n");

    return roomList[randomNum];
}


/* EVERYTHING ABOVE HERE WORKS */

// returns true if a connection can be added from Room (< 6 outbound connections), false otherwise
int canAddConnectionFrom(struct Room room) {
    if (room.numConnections < 6) {
        return TRUE;
    }
    return FALSE;
}

// returns true if a connection from Room 1 to Room 2 already exists, false otherwise
int connectionAlreadyExists(struct Room room1, struct Room room2) {
    int i = 0;

    while(room1.outBoundConnections[i] != NULL) {      // while there are still connections
        if(strcmp(room1.outBoundConnections[i], room2.roomName) == 0)     // if Room 1 connection matches Room 2
            return TRUE;       // return true
        else
            i++;
    }
    return FALSE;
}

// connects Rooms 1 and 2 together, does not check if this connection is valid
void connectRoom(struct Room room1, struct Room room2) {
    int i = 0;

    if (connectionAlreadyExists(room1, room2) == FALSE) {   // if there is not a connection between the two rooms
        while (room1.outBoundConnections[i] != NULL){      // iterate through the list of connections and find the first empty spot
            i++;
        }
        room1.outBoundConnections[i] = room2.roomName;
    }
}

// returns true if Rooms 1 and 2 are the same Room, false otherwise
int isSameRoom(struct Room room1, struct Room room2) {
    if (strcmp(room1.roomName, room2.roomName) == 0)
        return TRUE;
    return FALSE;
}
/*
// adds a random, valid outbound connection from one Room to another Room
void addRandomConnection(struct Room** gameRooms) {
    struct Room** room1;
    struct Room** room2;

    while(TRUE) {
        room1 = getRandomRoom(gameRooms);

        if (canAddConnectionFrom(room1) == TRUE)
            break;
    }

    do {
        room2 = getRandomRoom(gameRooms);
    }
    while(canAddConnectionFrom(room2) == FALSE || isSameRoom(room1, room2) == TRUE || connectionAlreadyExists(room1, room2) == TRUE); {

        connectRoom(room1, room2);
        connectRoom(room2, room1);
    }
}

// iterate through whole list of rooms and create connections for each one
void initGameRooms (struct Room** gameRooms) {
    while (isGraphFull(gameRooms) == FALSE) {
        addRandomConnection(gameRooms);
    }
}
*/

// write the room list created in initGameRooms to the files created in createFiles



int main() {
    srand(time(NULL));      // initialize random seed
    char* dirName = createDir();
    genRooms();
    FILE** filePtr = createFiles(dirName);
    initRooms();
    isGraphFull();
    getRandomRoom();


    //struct Room** gameRooms = initRooms(roomNames);
    //getRandomRoom(gameRooms);       // TODO: testing only - DELETE
    //addRandomConnection(gameRooms);     // TODO: testing only - DELETE
    //initGameRooms(gameRooms);
}

// need to fix outbound connections; probably addRandomConnections function