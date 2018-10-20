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
    struct Room* outBoundConnections[6];    // stores pointer to room connections
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
    for (i = 0; i < MAX_NUM_ROOMS; i++) {
        int index = rand() % (namesCount - i);  // random index between 0 and last element in array
        roomList[i].roomName = names[index];    // store name
        names[index] = names[namesCount - i - 1];   // overwrite with the last element in array

       // printf("%s\n", roomList[i].roomName);       // TODO: testing only - delete
    }
}

// creates seven different room files and fills with info
void createFiles(char* dirName) {
    FILE* filePath[256];
    FILE* filePtr;

    // create file for each room name and files
    for (int i =0; i < MAX_NUM_ROOMS; i++) {
        sprintf(filePath, "%s/%s.txt", dirName, roomList[i].roomName);  // create file name
        filePtr = fopen(filePath, "w");      // write file
        fprintf(filePtr, "ROOM NAME: %s\n", roomList[i].roomName);     // writes room name to file
        for (int j = 0; j < roomList[i].numConnections; j++) {      // iterates through each room connection and writes connection to file
            fprintf(filePtr, "CONNECTION %d: %s\n", j+1, roomList[i].outBoundConnections[j]->roomName);
        }

        if (roomList[i].roomType == 0) {        // prints room type since enum strings cannot be printed
            fprintf(filePtr, "ROOM TYPE: START_ROOM\n");
        }
        else if (roomList[i].roomType == 1) {
            fprintf(filePtr, "ROOM TYPE: MID_ROOM\n");
        }
        else if (roomList[i].roomType == 2) {
            fprintf(filePtr, "ROOM TYPE: END_ROOM\n");
        }
    }
}

// initializes room structs
void initRooms() {

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
int getRandomRoom() {
    int randomNum = rand() % 7;     // returns random number between 0 and 6

    return randomNum;
}

// returns true if a connection can be added from Room (< 6 outbound connections), false otherwise
bool canAddConnectionFrom(struct Room room) {
    if (room.numConnections < 6) {
        return TRUE;
    }
    return FALSE;
}

// returns true if a connection from Room 1 to Room 2 already exists, false otherwise
bool connectionAlreadyExists(int room1Pos, int Room2Pos) {
    int i = 0;

    while(roomList[room1Pos].outBoundConnections[i] != NULL) {      // while there are still connections
        if(strcmp(roomList[room1Pos].outBoundConnections[i], roomList[Room2Pos].roomName) == 0)     // if Room 1 connection matches Room 2
            return TRUE;       // return true
        else
            i++;
    }
    return FALSE;
}

// connects Rooms 1 and 2 together, does not check if this connection is valid
void connectRoom(int room1Pos, int room2Pos) {
    int i = 0;


    if (connectionAlreadyExists(room1Pos, room2Pos) == FALSE) {   // if there is not a connection between the two rooms
        while (roomList[room1Pos].outBoundConnections[i] != NULL){      // iterate through the list of connections and find the first empty spot
            i++;
        }
        roomList[room1Pos].outBoundConnections[i] = &roomList[room2Pos];
        roomList[room2Pos].outBoundConnections[i] = &roomList[room1Pos];

        // increment number of connections for both rooms
        roomList[room1Pos].numConnections++;
        roomList[room2Pos].numConnections++;
    }
}

// returns true if Rooms 1 and 2 are the same Room, false otherwise
bool isSameRoom(int room1Pos, int room2Pos) {
    if (strcmp(roomList[room1Pos].roomName, roomList[room2Pos].roomName) == 0)
        return TRUE;
    return FALSE;
}

// adds a random, valid outbound connection from one Room to another Room
void addRandomConnection() {
    int room1Pos = 0;
    int room2Pos = 0;

    while(TRUE) {
        room1Pos = getRandomRoom();

        if (canAddConnectionFrom(roomList[room1Pos]) == TRUE)
            break;
    }

    do {
        room2Pos = getRandomRoom();
    }
    while(canAddConnectionFrom(roomList[room2Pos]) == FALSE || isSameRoom(room1Pos, room2Pos) == TRUE || connectionAlreadyExists(room1Pos, room2Pos) == TRUE);

    connectRoom(room1Pos, room2Pos);
    connectRoom(room2Pos, room1Pos);
}


// iterate through whole list of rooms and create connections for each one
void initGameRooms () {
    while (isGraphFull() == FALSE) {
        addRandomConnection();
    }
}


// write the room list created in initGameRooms to the files created in createFiles
void writeRoomList() {
    char* dirName;
    FILE *filePtr;

    dirName = createDir();      // creates directory
    createFiles(dirName);  // creates the files for the directory
}

int main() {
    srand(time(NULL));      // initialize random seed
    genRooms();
    initRooms();
    initGameRooms();
    writeRoomList();
}
