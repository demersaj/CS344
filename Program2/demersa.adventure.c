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

// global mutexes and threads
pthread_mutex_t lock[2];
pthread_t thread[2];

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

// open the requested file and read; populate name and room type
void readFile() {
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
    strcpy(gameDir, findNewestDir());
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
            j = 0;
            i++;
        }

    }
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
void getTimeThread(void* close) {
    char buffer[256];
    struct tm *sTm;
    int *closeFlag = (int*) close;  // close flag


    // loop until close flag is set
    while(!*closeFlag) {
        pthread_mutex_lock(&lock[0]);   // lock mutex until unlocked by main thread
        printf ("Lock 0.\n");   // TODO: delete
        if (*closeFlag)     // if close flag is set, break
            break;

        // open currentTime.txt and write to it
        FILE *myFile = fopen("currentTime.txt", "w+");
        if (myFile == NULL)
            printf("Error opening file.\n");
        time_t now = time(0);
        sTm = localtime(&now);

        strftime(buffer, sizeof(buffer), "%I:%M%p, %A, %B %d, %Y", sTm);    // store the formatted time TODO: fix format
        buffer[5] = tolower(buffer[5]);     // make AM/PM lowercase
        buffer[6] = tolower(buffer[6]);
        fputs(buffer, myFile);    // write the time to file
        fclose(myFile);     // close file

        // unlock mutex
        pthread_mutex_unlock(&lock[0]);
        sleep(1);
        printf ("Unlock 0\n");  //TODO: delete
    }
}

// reads the time from file and outputs it to the console
void* printTimeThread(void* close) {
    FILE* myFile;
    char buffer[256];
    int *closeFlag = (int*) close;  // hold close flag

    // loop until main thread is ready
    while (!*closeFlag) {
        pthread_mutex_lock(&lock[1]);   // lock until main is read
        printf("Lock 1\n"); // TODO - delete
        if (*closeFlag)     // if close flag is set, break
            break;

        // read from time file
        myFile = fopen("currentTime.txt", "r");

        // make sure the file exists
        if (myFile == NULL)
            printf("Error opening file!\n");

        // Read into buffer and output to console
        while (fgets(buffer, 256, myFile) != NULL) {
            printf("%s\n", buffer);
        }
        fclose(myFile);

        // unlock mutex
        pthread_mutex_unlock(&lock[1]);
        sleep(1);
        printf ("Unlock 1\n");  // TODO - delete
    }
}

// play the game
int main() {
    char roomPath[200][9];  // tracks the rooms the user has been in
    bool end = false;       // end game condition
    char buffer[256];       // buffer to hold user input
    struct Room curRoom;    // room struct to hold the current room file
    int i;
    bool validInput = false;// holds bool to validate user input
    int roomPos = 0,
        result_code,
        close = 0,
        stepCount = 0;      // keeps track of how many rooms the user has been in

    // read the game rooms from file
    readFile();

    // initialize both mutexes, then acquire lock for the main thread
    for ( i = 0; i < 2; i++) {
        if (pthread_mutex_init(&lock[i], NULL) != 0) {  // initialize mutex and check for error
            printf ("Error - mutex %d not initialized.\n", i + 1);
            return -1;  // return error code
        }
        pthread_mutex_lock(&lock[i]);   // lock each mutex
        printf ("Thread %d is now locked.\n", i);   // TODO - delete
    }

    // create first thread to write time to file
    result_code = pthread_create(&thread[0], NULL, &getTimeThread, &close);   // close flag should let the system know when execution is finished
    printf ("Thread 0 is now created.\n");  // TODO - delete
    if (result_code != 0){
        printf ("Error - thread not created.\n");
        return result_code;     // return error code
    }

    // create second thread to read time from file and output to console
    result_code = pthread_create(&thread[1], NULL, &printTimeThread, &close);   // close flag should let the system know when execution is finished
    printf ("Thread 1 is now created.\n");
    if (result_code != 0) {
        printf("Error - thread not created.\n");
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

        // handle user request to print time. manage mutexes and threads
        if (strcmp(buffer, "time") == 0) {
            pthread_mutex_unlock(&lock[0]);     // unlock mutex for the getTime thread
            sleep(1);   // sleep to allow time to thread to get lock
            pthread_mutex_lock(&lock[0]);   // block thread from executing again
            pthread_mutex_unlock(&lock[1]);     // unlock mutex for printTime thread
            sleep(1);   // memory leak otherwise
            pthread_mutex_lock(&lock[1]);   // lock mutex for printTime thread
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

    close = 1;  // set close flag

    // unlock each mutex
    for (i = 0; i < 2; i++) {
        if (pthread_mutex_unlock(&lock[i]) != NULL) {
            perror ("Error unlocking threads!\n");
            return -1;
        }
        else
            printf ("Thread %d is now unlocked.\n", i);     // TODO - delete
    }

    // join threads and wait for them to finish
    for (i = 0; i < 2; i++) {
        if (pthread_join(thread[i], NULL) != 0) {
            perror ("Error - failed to join threads.\n");
        }
        else printf("Thread %d is now joined\n", i); // TODO delete
    }

    // destroy mutexes
    pthread_mutex_destroy(&lock[0]);
    pthread_mutex_destroy(&lock[1]);

    return 0;
}
