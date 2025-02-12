#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

void createGameDirectory() {
    char *home = getenv("HOME");  // Get the home directory
    if (!home) {
        fprintf(stderr, "Failed to get HOME environment variable.\n");
        exit(EXIT_FAILURE);
    }

    char path[256];
    snprintf(path, sizeof(path), "%s/.rlg327", home); // Construct full path

    // Check if the directory exists
    struct stat st;
    if (stat(path, &st) == -1) { // Directory does not exist
        if (mkdir(path, 0700) == -1) {  // Create directory with rwx------ (only owner can access)
            perror("mkdir failed");
            exit(EXIT_FAILURE);
        } else {
            printf("Directory %s created successfully.\n", path);
        }
    } else {
    printf("Directory %s already exists.\n", path);
    }
}

int main(){
    createGameDirectory();
    return 0;
}
