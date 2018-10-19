// Program 2 - Rooms program
// Andrew Demers
// Date: 10/25/18

#include <memory.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

// iterates through all subdirectories in current directory and finds the newest one
// some of the code below was adapted from Required Reading 2.4: Manipulating Directories
void findNewestDir() {
    int newestDirTime = -1;     // modified timestamp of newest subdir examined
    char targetDirPrefix[32] = "demersa.buildrooms.";       // desired prefix
    char newestDirName[256];    // holds name of the newest directory
    memset(newestDirName, '\0', sizeof(newestDirName));

    DIR *dirToCheck;        // holds the directory we're starting in
    struct dirent *fileInDir;       // holds the current subdir of the starting dir
    struct stat dirAttributes;      // holds info we've gained about subdir

    dirToCheck = opendir(".");      // open up the directory this program was run in

    if (dirToCheck > 0) {    // make sure the current directory can be open
        while ((fileInDir = readdir(dirToCheck)) != NULL) {     // check each entry in dir
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) {   // if entry has prefix
                printf("Found the prefix: %s\n", fileInDir->d_name);
                stat(fileInDir->d_name, &dirAttributes);    // get attributes of the entry

                if ((int) dirAttributes.st_mtime > newestDirTime) {      // if this time is bigger
                    newestDirTime = (int) dirAttributes.st_mtime;
                    memset(newestDirName, '\0', sizeof(newestDirName));
                    strcpy(newestDirName, fileInDir->d_name);
                    printf("Newer subdir: %s, new time: %d\n",
                           fileInDir->d_name, newestDirTime);
                }
            }
        }
    }
}

int main() {
    findNewestDir();
}