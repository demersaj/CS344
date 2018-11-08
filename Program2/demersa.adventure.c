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
#include <unistd.h>
#include <ctype.h>

#define MAX_NUM_ROOMS 7
typedef enum {false = 0, true = 1} bool;

// global mutex and thread
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t time_thread;

// room struct to hold info about game
struct Room {
    int numConnections;
    char roomName[64];
    char roomType[64];
    char outBoundConnections[8][9];    // stores room connections
};

// global list of game rooms
struct Room gameRooms[MAX_NUM_ROOMS];

/* The code below was adapted from Required Reading 2.4: Manipulating Directories */
// loads files from directory and searches for starting room
char* findNewestDir(char** newestDirName) {
    DIR* currentDir;
    struct dirent* fileInDir;
    char targetDirPrefix[32] = "demersa.rooms";     // file name we are searching for
    struct stat dirAttributes;      // holds the info we are looking for
    int newestDirTime = -1;     // holds newest time of directory for comparison

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

// open the requested file and read; populate name and room type
void readFile(char** newDirName) {
    char* filesList[7];     // store names of files
    int i = 0;
    int j = 0;
    FILE* fptr;     // file pointer
    DIR *dir;
    struct dirent* fileDir;
    char gameDir[256];
    char temp[256];
    char filename[256];
    char name[9];
    char roomType[9];
    char buffer[256];
    char connection[9];

    // clear out folder name to prevent errors
    memset(gameDir, '\0', sizeof(gameDir));
    strcpy(gameDir, findNewestDir(&newDirName));
    // make sure file exists
    if (gameDir == NULL) {
        printf("Error\n");
    }

    // read each file name and stores it in a list
    if ((dir = opendir(gameDir)) != NULL) {
        while ((fileDir = readdir(dir)) != NULL) {
            if (!strcmp(fileDir->d_name, ".") || !strcmp(fileDir->d_name, "..")) {
                continue;
            }

            // get file names
            filesList[i] = malloc(strlen(fileDir->d_name) + 1);
            sprintf(filesList[i], "%s", fileDir->d_name);
            sprintf(filename, "./%s/%s", gameDir, fileDir->d_name);

            // open files and get info from them
            fptr = fopen(filename, "r");

            while (feof(fptr) == 0) {
                fgets(temp, 256, fptr);

                // find and store the names
                sscanf(temp, "%[^\n]", buffer);
                if (strstr(buffer, "ROOM NAME:") != NULL) {
                    sscanf(buffer, "%*s %*s %s", name);
                    strcpy(gameRooms[i].roomName, name);
                }

                    // find and store the connections
                else if (strstr(buffer, "CONNECTION") != NULL) {
                    sscanf(buffer, "%*s %*s %s", connection);
                    strcpy(gameRooms[i].outBoundConnections[j], connection);   // add the connection
                    gameRooms[i].numConnections++;      // increment number of connections
                    j++;
                }

                    // find and store room types
                else if (strstr(buffer, "ROOM TYPE:") != NULL) {
                    sscanf(buffer, "%*s %*s %s", roomType);
                    strcpy(gameRooms[i].roomType, roomType);
                }

            }
            fclose(fptr);   // close file
            j = 0;
            i++;
        }

    }
    // free allocated memory
    for (i = 0; i < 7; i++)
        free (filesList[i]);

    closedir(dir);  // close the previously opened directory
}


// returns the position of the starting room
int getStartRoom() {
    int i;

    for (i = 0; i < MAX_NUM_ROOMS; i++) {   // iterate through the rooms
        if (strcmp(gameRooms[i].roomType, "START_ROOM") == 0) {    // if the room type is start room
            return i;
        }
    }
    return -1;
}

// used for debugging only
void printRoomInfo() {
    int i;
    int j;

    for (i =0; i < MAX_NUM_ROOMS; i++) {

        printf("Name: %s\n", gameRooms[i].roomName);
        for (j = 0; j < gameRooms[i].numConnections; j++) {
            printf("Connection %d: %s\n", i + 1, gameRooms[i].outBoundConnections[j]);
        }
        printf("Room Type: %s\n", gameRooms[i].roomType);
    }
}

// returns the room index
int getRoom(char* room) {
    int i;

    for (i = 0; i < MAX_NUM_ROOMS; i++) {
        if (strcmp(gameRooms[i].roomName, room) == 0) {
            return i;
        }
    }
    return -1;
}

/* This function was adapted from code taken from: https://stackoverflow.com/questions/7411301/how-to-introduce-date-and-time-in-log-file */
// gets the current time and writes it to file
// stays locked until called by main

void printTime() {
    char buffer[256];
    struct tm *sTm;

    // lock mutex so no other thread can access it
    pthread_mutex_lock(&mutex);


    // open currentTime.txt and write to it
    FILE *myFile = fopen("currentTime.txt", "w+");
    if (myFile == NULL)
        printf("Error opening file.\n");
    else {
        time_t now = time(0);
        sTm = localtime(&now);

        strftime(buffer, sizeof(buffer), "%I:%M%p, %A, %B %d, %Y", sTm);    // store the formatted time
        buffer[5] = tolower(buffer[5]);     // make AM/PM lowercase
        buffer[6] = tolower(buffer[6]);
        fputs(buffer, myFile);    // write the time to file
    }
    fclose(myFile);     // close file

    // unlock mutex
    pthread_mutex_unlock(&mutex);
}

// play the game
int main() {
    char roomPath[200][9];  // track the rooms the user has been in
    bool end = false;       // end game condition
    char buffer[256];       // buffer to hold user input
    struct Room curRoom;    // room struct to hold the current room file
    int i;
    bool validInput = false;// hold bool to validate user input
    int roomPos = 0,
            result_code = 0,
            stepCount = 0;      // keep track of how many rooms the user has been in
    FILE* myFile;


    // read the game rooms from file
    char* dirName = malloc(256 * sizeof(char));  // holds name of newest directory
    readFile(&dirName);

    // lock mutex
    if (pthread_mutex_lock(&mutex) != 0)   // lock the mutex
        printf("Error - mutex not locked from main.\n");

    // create time thread to write time to file
    result_code = pthread_create(&time_thread, NULL, printTime, NULL);    // thread ID, NULL, start_routine, NULL
    if (result_code != 0){
        printf ("Error - thread not created.\n");
        return result_code;     // return error code
    }

    // output the players current location
    int start = getStartRoom();
    curRoom = gameRooms[start];
    strcpy(roomPath[stepCount], curRoom.roomName);  // add start room to start path

    do {
        printf("\nCURRENT LOCATION: %s\n", curRoom.roomName);

        // list the possible connections
        printf("POSSIBLE CONNECTIONS: ");
        for (i = 0; i < curRoom.numConnections - 1; i++) {  // print all but last rooms
            printf("%s, ", curRoom.outBoundConnections[i]);
        }
        printf("%s.\n", curRoom.outBoundConnections[i]);    // print last room

        // ask player where they want to go next
        printf("WHERE TO? >");
        memset(buffer, '\0', sizeof(buffer));   // clean out buffer, just in case
        scanf("%256s", buffer);

        // validate user input
        validInput = false;
        for (i = 0; i < curRoom.numConnections; i++) {  // if choice is valid
            if (strcmp(curRoom.outBoundConnections[i], buffer) == 0) {
                validInput = true;  // set valid to true
                roomPos = getRoom(buffer);  // get new room position
                curRoom = gameRooms[roomPos];   // set current room == new room
                stepCount++;    // increment stepcount
                strcpy(roomPath[stepCount], curRoom.roomName);  // add room to room path
            }
        }

        // handle user request to print time. manage mutex and threads
        if (strcmp(buffer, "time") == 0) {
            pthread_mutex_unlock(&mutex);  // unlock mutex so timeThread can use it
            pthread_join(time_thread, NULL);   // waits for time thread to finish, then joins

            // read from time file and print to console
            myFile = fopen("currentTime.txt", "r");      // open time file read only
            if (myFile == NULL)
                perror ("Error opening time file!\n");  // make sure file can be opened
            else {
                // read into buffer and output to console
                while (fgets(buffer, 256, myFile) != NULL)
                    printf ("\n%s\n", buffer);
            }
            fclose (myFile);     // close file

            // lock mutex
            pthread_mutex_lock(&mutex);

            // recreate time thread
            result_code = (pthread_create(&time_thread, NULL, printTime, NULL));
            if (result_code != 0) {
                printf("Error - thread not created.\n");
                return result_code;     // return error code
            }
        }
        else if (validInput == false) {
            printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");   // output error message
        }

        // end condition
        if (strcmp(curRoom.roomType, "END_ROOM") == 0) {    // if room type is end room
            strcpy(roomPath[++stepCount], curRoom.roomName);    // add final room to step path
            printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");      // victory message
            printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", stepCount);    // output step count
            for (i = 0; i < stepCount; i++)
                printf("%s\n", roomPath[i]);    // output room path
            end = true;     // end game
        }
    }
    while (end == false);

    // destroy time thread
    pthread_cancel(time_thread);

    // unlock mutex
    pthread_mutex_unlock(&mutex);

    // join threads and wait for them to finish
    if (pthread_join(time_thread, NULL) != 0)
        perror ("Error - failed to join threads.\n");

    // destroy mutex
    pthread_mutex_destroy(&mutex);

    // free allocated memory
    free(dirName);

    return 0;
}