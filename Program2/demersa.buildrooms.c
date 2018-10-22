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
#define DEBUG 1
enum RoomType {START_ROOM, END_ROOM, MID_ROOM};
typedef enum {false = 0, true = 1} bool;

// defines a room struct
struct Room {
    int numConnections;
    char* roomName;
    enum RoomType roomType;
    struct Room* outBoundConnections[6];    // stores pointer to room connections
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
    }
}

// creates seven different room files and fills with info
void createFiles(char* dirName) {
    chdir(dirName);     // change to correct directory
    char filePath[256];

    for (int i = 0; i< MAX_NUM_ROOMS; i++) {
        sprintf(filePath, "%s.txt", roomList[i].roomName);  // create file name
        FILE* filePtr = fopen(filePath, "w");   // open file and write to it
        fprintf(filePtr, "ROOM NAME: %s\n", roomList[i].roomName);      // write room name to file

        for (int j = 0; j < roomList[i].numConnections; j++) {
            fprintf(filePtr, "CONNECTION %d: %s\n", j + 1, roomList[i].outBoundConnections[j]->roomName);
        }

        switch(roomList[i].roomType) {
            case START_ROOM:
                fprintf(filePtr, "ROOM TYPE: START_ROOM\n");
                break;
            case MID_ROOM:
                fprintf(filePtr, "ROOM TYPE: MID_ROOM\n");
                break;
            case END_ROOM:
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
int isGraphFull(struct Room* roomList) {

    for (int i = 0; i < MAX_NUM_ROOMS; i++) {
        if (roomList[i].numConnections  < 3) {
            return i;
        }
    }
    return -1;
}

// returns a random Room, does not validate if connection can be added
int getRandomRoom() {
    int randomNum = rand() % 7;     // returns random number between 0 and 6

    return randomNum;
}

// returns true if a connection can be added from Room (< 6 outbound connections), false otherwise
bool canAddConnectionFrom(struct Room room) {
    if (room.numConnections < 6) {
        return true;
    }
    return false;
}

// returns true if a connection from Room 1 to Room 2 already exists, false otherwise
bool connectionAlreadyExists(int room1Pos, int Room2Pos) {
    int i = 0;

    while(roomList[room1Pos].outBoundConnections[i] != NULL) {      // while there are still connections
        if(strcmp(roomList[room1Pos].outBoundConnections[i]->roomName, roomList[Room2Pos].roomName) == 0)     // if Room 1 connection matches Room 2
            return true;       // return true
        else
            i++;
    }
    return false;
}

// connects Rooms 1 and 2 together, does not check if this connection is valid
void connectRoom(int room1Pos, int room2Pos) {
    int i = 0;
    int j = 0;

    if (connectionAlreadyExists(room1Pos, room2Pos) == false) {   // if there is not a connection between the two rooms
        while (roomList[room1Pos].outBoundConnections[i] != NULL)      // iterate through the list of connections and find the first empty spot
            i++;
        while (roomList[room2Pos].outBoundConnections[j] != NULL)
            j++;

        roomList[room1Pos].outBoundConnections[i] = &roomList[room2Pos];
        roomList[room2Pos].outBoundConnections[j] = &roomList[room1Pos];

        // increment number of connections for both rooms
        roomList[room1Pos].numConnections++;
        roomList[room2Pos].numConnections++;
    }
}

// returns true if Rooms 1 and 2 are the same Room, false otherwise
bool isSameRoom(int room1Pos, int room2Pos) {
    if (strcmp(roomList[room1Pos].roomName, roomList[room2Pos].roomName) == 0)
        return true;
    return false;
}

// adds a random, valid outbound connection from one Room to another Room
void addRandomConnection(int room1Pos, struct room* gameRoom) {

    int room2Pos = 0;

    while(true) {

        if (canAddConnectionFrom(roomList[room1Pos]) == true)
            break;
    }

    do {
        room2Pos = getRandomRoom();
    }
    while(canAddConnectionFrom(roomList[room2Pos]) == false || isSameRoom(room1Pos, room2Pos) == true || connectionAlreadyExists(room1Pos, room2Pos) == true);

    connectRoom(room1Pos, room2Pos);
    connectRoom(room2Pos, room1Pos);
}

// iterate through whole list of rooms and create connections for each one
void initGameRooms () {
    int roomIndex;

    while ((roomIndex = isGraphFull(roomList)) != -1) {
        addRandomConnection(roomIndex, roomList);
    }
}

// write the room list created in initGameRooms to the files created in createFiles
void writeRoomList() {
    char* dirName;

    dirName = createDir();      // creates directory
    createFiles(dirName);  // creates the files for the directory
}


void getRoomType(struct Room* r, char* buff, int buffLen){
    if (r == NULL || buff == NULL)
        return;

    int len = buffLen - 1;
    memset(buff, 0, buffLen);

    switch (r->roomType) {
        case START_ROOM:
            if (10 < len)
                len = 10;
            strncpy(buff, "START_ROOM", len);
            break;
        case MID_ROOM:
            if (8 < len)
                len = 8;
            strncpy(buff, "MID_ROOM", len);
            break;
        case END_ROOM:
            if (8 < len)
                len = 8;
            strncpy(buff, "END_ROOM", len);
            break;
    }
}

void printRoom(struct Room* r){
    if(r == NULL)
        return;
    if (DEBUG) {
        printf("Name: %s\n", r->roomName);
        printf("Connected rooms: %i\n", r->numConnections);
        char rType[12];
        getRoomType(r, rType, 12);
        printf("Room type: %s\n", rType);
        printf("RoomID: 0x%x\n", r);
        printf("Adjacent rooms: \n");
        for (int i = 0; i < 7; i++) {
            printf("\t0x%x\n", r->outBoundConnections[i]);
        }
    }
}

void listAllRooms(struct Room* roomList, int numRoom) {
    printf("\n\n\n\n\n");
    for (int i = 0; i < numRoom; i++) {
        printRoom(&roomList[i]);
    }
}


int main() {
    srand(time(NULL));      // initialize random seed
    genRooms();
    initRooms();
    initGameRooms();
    writeRoomList();
    if (DEBUG)
        listAllRooms(roomList, 7);

    return 0;
}