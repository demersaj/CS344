// Program 2 - Rooms program
// Andrew Demers
// Date: 10/25/18

#include <memory.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_NUM_ROOMS 7

// global mutex
pthread_mutex_t mutex;

// room struct to hold info about game
struct Room {
    int numConnections;
    char* roomName;
    char* roomType;
    char* outBoundConnections[6];    // stores pointer to room connections
};

/* The code below was adapted from Required Reading 2.4: Manipulating Directories */
// loads files from directory and searches for starting room
char* findNewestDir() {
    DIR* currentDir;
    struct dirent* fileInDir;
    char targetDirPrefix[32] = "demersa.rooms";     // file name we are searching for
    struct stat dirAttributes;      // holds the info we are looking for
    int newestDirTime = -1;     // holds newest time of directory for comparison
    char* newestDirName = malloc(256 * sizeof(char));    // holds the name of the newest dir
    memset(newestDirName, '\0', sizeof(newestDirName));

    currentDir = opendir(".");      // opens current directory

    if (currentDir > 0) {       // if dir can be opened
        while ((fileInDir = readdir(currentDir)) != NULL) {     // while there are still files in directory
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) {       // if prefix is found
                stat(fileInDir->d_name, &dirAttributes);        // gets attributes of file

                if ((int)dirAttributes.st_mtime > newestDirTime) {  // if this time is bigger
                    newestDirTime = (int)dirAttributes.st_mtime;
                    memset(newestDirName, '\0', sizeof(newestDirName));
                    strcpy(newestDirName, fileInDir->d_name);
                }
            }
        }
    }
    closedir(currentDir);
    return newestDirName;
}

// open the requested file and read
struct Room* readFile(struct Room* gameRooms, char roomName[256]) {
    char* gameDir = findNewestDir();    // load the newest dir
    DIR* currentDir = opendir(gameDir);
    struct dirent* curFile;

    /*

    char* filesList[7];     // store names of files
    int i = 0;
    int j = 0;
    FILE* fptr;     // file pointer
    char temp[256];
    char filename[256];
    char name[9];
    char roomType[9];
    char buffer[256];
    char connection[9];

    // make sure file exists
    if (currentDir == NULL) {
        printf("Error\n");
    }

    // read each file name and stores it in a list
    while ((curFile = readdir(currentDir)) != NULL) {
        if (!strcmp(curFile->d_name, ".") ||  !strcmp(curFile->d_name, "..")) {
            continue;
        }
            // get file names
            filesList[i] = malloc(strlen(curFile->d_name) + 1);
            strcpy(filesList[i], curFile->d_name);
            sprintf(filename, "./%s/%s", gameDir, curFile->d_name);

            // open files and get info from them
            fptr = fopen(filename, "r");


            while (feof(fptr) == 0) {
                fgets(temp, 256, fptr);

                // find and store the name
                sscanf(temp, "%[^\n]", buffer);
                if (strstr(buffer, "ROOM NAME:") != NULL) {
                    sscanf(buffer, "%*s %*s %s", name);
                    gameRooms[i].roomName = name;

                    printf("Found the right name! %s\n", gameRooms[i].roomName); // TODO - DELETE
                }

                // find and store the connections
                else if (strstr(buffer, "CONNECTION") != NULL) {
                    sscanf(buffer, "%*s %*s %s", connection);
                    gameRooms[i].outBoundConnections[j] = connection;   // add the connection
                    gameRooms[i].numConnections++;      // increment number of connections

                    printf("Found a connection: %s\n", gameRooms[i].outBoundConnections[j]);    // TODO: delete
                    j++;
                }

                // find and store room types
                else if (strstr(buffer, "ROOM TYPE:") != NULL) {
                    sscanf(buffer, "%*s %*s %s", roomType);
                    gameRooms[i].roomType = roomType;

                    printf("Found the room type: %s\n", gameRooms[i].roomType);     //TODO: delete
                }
            }
            i++;
    }
    return gameRooms;*/
}

// starts the player in a the start room
struct Room startGame(struct Room* gameRoom) {
    struct Room startRoom;
    for (int i = 0; i < MAX_NUM_ROOMS; i++) {
        if (strcmp(gameRoom[i].roomType, "START_ROOM") == 0) {
            startRoom = gameRoom[i];
        }
    }
    return startRoom;
}

// checks connections available
int checkConnections(struct Room currentRoom) {
    for (int i = 0; i < currentRoom.numConnections; i++) {
        if (strcmp(currentRoom.roomName, currentRoom.outBoundConnections[i]) == 0)
            return 1;
    }
    return 0;
}


// play the game
void gamePlay() { /*
    int stepCount = 0;
    char* roomPath[1024];
    int end = -1;

    // read the game rooms from file
    struct Room gameRooms[MAX_NUM_ROOMS];
    readFiles(gameRooms);

    // output the players current location
    struct Room startRoom = startGame(gameRooms);
    struct Room room = startRoom;
    do {
        printf("\nCURRENT LOCATION: %s\n", room.roomName);

        // list the possible connections
        printf("POSSIBLE CONNECTIONS: ");
        for (int i =0; i < room.numConnections; i++) {
            printf("%s,", room.outBoundConnections[i]);
        }

        // ask player where they want to go next
        printf(".\nWHERE TO? >");
        //getline


        // end condition
        if (room.roomType == "END_ROOM") {
            printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
            printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", stepCount);
            end = -1;
        }
    }

    while (end == 0);*/
}

// controls threading of program
void threading(){
    return;
}

/* This function was adapted from code taken from: https://stackoverflow.com/questions/7411301/how-to-introduce-date-and-time-in-log-file */
// gets the current time and writes it to file
void getTime() {
    char buff[256];
    struct tm *sTm;

    // open currentTime.txt and write to it
    FILE* myFile = fopen("currentTime.txt", "w+");
    time_t now = time (0);
    sTm = gmtime(&now);

    strftime(buff, sizeof(buff), "%I:%M%p, %A, %B %d, %Y", sTm);    // store the formatted time
    fputs(buff, myFile);    // write the time to file
    fclose(myFile);     // close file
}

// reads the time from file and outputs it to the console
void printTime() {
    // read from file
    return;
    // make sure the file exists

    // Read into buffer and output to console

}

int main() {
    getTime();  // TODO: DELETE

    gamePlay();

    return 0;
}