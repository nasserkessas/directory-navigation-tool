#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define NUM_LINES 16
#define NAME_LENGTH 16
#define DIR_LENGTH 256
#define SAVENAME ".nvsave"

struct dirEntry {
    char name[NAME_LENGTH];
    char dir[DIR_LENGTH];
};

struct file {
    int lineCount;
    struct dirEntry entries[sizeof(char *) * NUM_LINES];
};

void inject_shell(char *cmd);

struct file readFile();

void writeFile(struct file data);

void writeFileDelete(struct file data, char *key) ;

void list(struct file data);

int isvalid (char *str);

void usage(char *path) {
    printf("\nUsage: %s [COMMANDS]\n\n\tstore \tStores current directory with an incremented number as the key\n\t\t-a \"NAME\"  Adds the current directory with NAME as the key\n\t\t\t--set-default  Sets added directory to default position\n\n\t\t-d \"NAME\"  Deletes directory with key NAME\n\t\t-l         Lists all stored directories and keys\n\t\t-f         Prints save file location\n\n\tmove \tChanges directory to default or first listed directory\n\t\t-n \"NAME\"  Changes directory to stored value with key NAME\n\n", path);
}

int main(int argc, char **argv) {

    char *path = argv[0];

    if (argc == 1) {
        usage(path);
        return 1;
    }

    char *name = argv[1];

    if (strcmp(name, "store") == 0) {
        if (argv[2] == NULL){

            struct file data = readFile();

            if (data.lineCount == NUM_LINES-1) {
                printf("The maximum amount of saved directories (%d) has already been reached\n", NUM_LINES-1);
                return 1;
            }


            int highestFoundKey = 0;
            char converted[16];

            for (int i = 0; i < data.lineCount; i++) {
                sprintf(converted, "%d", highestFoundKey+1);

                if (strcmp(data.entries[i].name, converted) == 0) {
                    i = 0;
                    highestFoundKey++;
                }
            }

            char cwd[DIR_LENGTH];
            if (getcwd(cwd, sizeof(cwd)) == NULL) {
                printf("Failed to get current woking directory\n");
                return 1;
            }

            struct dirEntry newDir;
            strcpy(newDir.name, converted);
            strcpy(newDir.dir, cwd);

            data.entries[data.lineCount] = newDir;
            data.lineCount++;

            writeFile(data);
            printf("Added \"%s\": %s\n", data.entries[data.lineCount-1].name, data.entries[data.lineCount-1].dir);

            return 0;
        }

        if (strcmp(argv[2], "-f") == 0) {
            char saveloc[64] = { 0 };

            strcat(saveloc, getenv("HOME"));
            strcat(saveloc, "/");
            strcat(saveloc, SAVENAME);

            printf("Current directory save location: %s\n", saveloc);

            return 0;
        }

        if (strcmp(argv[2], "-a") == 0) {

            if (argv[3] == NULL) {
                printf("%s add -a must have a NAME arguement\n", path);
                return 1;
            }

            if (!isvalid(argv[3])) {
                printf("Invalid arguement for %s add -a: \"%s\"\n", path, argv[3]);
                return 1;
            }

            if (argv[4] == NULL) {
                struct file data = readFile();

                int foundKey = 0;
                for (int i = 0; i < data.lineCount; i++) {
                    if (strcmp(data.entries[i].name, argv[3]) == 0) {
                        foundKey = 1;
                    }
                }

                if (foundKey) {
                    printf("Directory key \"%s\" already exists\n", argv[3]);
                    return 1;
                }

                if(data.lineCount == NUM_LINES-1) {
                    printf("The maximum amount of saved directories (%d) has already been reached\n", NUM_LINES-1);
                    return 1;
                }

                char cwd[DIR_LENGTH];
                if (getcwd(cwd, sizeof(cwd)) == NULL) {
                    printf("Failed to get current woking directory\n");
                    return 1;
                }

                struct dirEntry newDir;
                strcpy(newDir.name, argv[3]);
                strcpy(newDir.dir, cwd);

                data.entries[data.lineCount] = newDir;
                data.lineCount++;

                writeFile(data);
                printf("Added \"%s\": %s\n", data.entries[data.lineCount-1].name, data.entries[data.lineCount-1].dir);
                return 0;
            }

            else {

                if (strcmp(argv[4], "--set-default") == 0){
                    struct file data = readFile();

                    int foundKey = 0;
                    for (int i = 0; i < data.lineCount; i++) {
                        if (strcmp(data.entries[i].name, argv[3]) == 0) {
                            foundKey = 1;
                        }
                    }

                    if (foundKey) {
                        printf("Directory key \"%s\" already exists\n", argv[3]);
                        return 1;
                    }

                    if(data.lineCount == NUM_LINES-1) {
                        printf("The maximum amount of saved directories (%d) has already been reached\n", NUM_LINES-1);
                        return 1;
                    }

                    char cwd[DIR_LENGTH];
                    if (getcwd(cwd, sizeof(cwd)) == NULL) {
                        printf("Failed to get current woking directory\n");
                        return 1;
                    }

                    struct dirEntry newDir;
                    strcpy(newDir.name, argv[3]);
                    strcpy(newDir.dir, cwd);

                    for (int i = data.lineCount; i > 0; i--) {
                        data.entries[i] = data.entries[i-1];
                    }
                    
                    data.entries[0] = newDir;
                    data.lineCount++;
                    printf("Added default directory \"%s\": %s\n", data.entries[0].name, data.entries[0].dir);

                    writeFile(data);

                    return 0;
                }

                else {
                    printf("Invalid option for %s store -a: %s\n", path, argv[4]);
                    return 1;
                }
            }
        }

        if (strcmp(argv[2], "-d") == 0) {

            if (argv[3] == NULL) {
                printf("%s add -d must have a NAME arguement\n", path);
                return 1;
            }

            struct file data = readFile();
            
            int foundKey = 0;
            for (int i = 0; i < data.lineCount; i++) {
                if (strcmp(data.entries[i].name, argv[3]) == 0) {
                    foundKey = 1;
                }
            }

            if (!foundKey) {
                printf("Directory key \"%s\" not found\n", argv[3]);
                return 1;
            }

            printf("Deleted directory with key \"%s\"\n", argv[3]);

            writeFileDelete(data, argv[3]);

            return 0;
        }

        if (strcmp(argv[2], "-l") == 0) {
            struct file data = readFile();
            list(data);    
            return 0;
        }

        else {
            printf("Invalid option for %s store: %s\n", path, argv[2]);
            return 1;
        }
    }

    if (strcmp(name, "move") == 0) {

        if (argv[2] == NULL) {

            struct file data = readFile();

            if (data.lineCount == 0) {
                printf("There are no saved directories\n");
                return 1;
            }
            
            char command[260] = "cd ";
            strcat(command, data.entries[0].dir);
            strcat(command, "\r");

            inject_shell(command);

            return 0;
        }

        if (strcmp(argv[2], "-n") == 0) {
            if (argv[3] == NULL) {
                printf("%s move -n must have a NAME arguement\n", path);
                return 1;
            }

            if (!isvalid(argv[3])) {
                printf("Invalid arguement for %s move -n: \"%s\"\n", path, argv[3]);
                return 1;
            }

            struct file data = readFile();
            
            int foundKey = 0;
            for (int i = 1; i < data.lineCount+1; i++) {
                if (strcmp(data.entries[i-1].name, argv[3]) == 0) {
                    foundKey = i;
                }
            }

            if (!foundKey) {
                printf("Directory key \"%s\" not found\n", argv[3]);
                return 1;
            }
            
            char command[260] = "cd ";
            strcat(command, data.entries[foundKey-1].dir);
            strcat(command, "\r");

            inject_shell(command);

            return 0;
        }

        else {
            printf("Invalid option for %s move: %s\n", path, argv[2]);
            return 1;
        }
    }

    else {
        printf("Unknown command: %s %s\n", path, name);
        return 1;
    }
}

void inject_shell(char *cmd){
  int i = 0;
  while (cmd[i] != '\0'){
    ioctl(0, TIOCSTI, &cmd[i++]);
  }
}

struct file readFile() {
    char saveloc[64] = { 0 };
    strcat(saveloc, getenv("HOME"));
    strcat(saveloc, "/");
    strcat(saveloc, SAVENAME);

    FILE *fp = fopen(saveloc, "r");
    if (!fp) {
        printf("Error opening file %s\n", saveloc);
        struct file ret;
        exit(1);
    }
    struct file fileContents;
    
    fileContents.lineCount = 0;

    char line[DIR_LENGTH];

    while (fgets(line, DIR_LENGTH, fp) != NULL) {
        if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
        struct dirEntry thisLine;
        strcpy(thisLine.name, strtok(line, "|"));
        strcpy(thisLine.dir, strtok(NULL, "|"));
        fileContents.entries[fileContents.lineCount] = thisLine;
        fileContents.lineCount++;
    };

    fclose(fp);
    return fileContents;
}

void writeFile(struct file data) {
    char saveloc[64] = { 0 };

    strcat(saveloc, getenv("HOME"));
    strcat(saveloc, "/");
    strcat(saveloc, SAVENAME);

    FILE *fp = fopen(saveloc, "w");
    
    for (int i = 0; i < data.lineCount; i++) {
        fprintf(fp,"%s|%s\n", data.entries[i].name, data.entries[i].dir);
    }

    fclose(fp);
}

void writeFileDelete(struct file data, char *key) {
    char saveloc[64] = { 0 };
    strcat(saveloc, getenv("HOME"));
    strcat(saveloc, "/");
    strcat(saveloc, SAVENAME);
    FILE *fp = fopen(saveloc, "w");

    for (int i = 0; i < data.lineCount; i++) {
        if (strcmp(data.entries[i].name, key) == 0) continue;

        fprintf(fp,"%s|%s\n", data.entries[i].name, data.entries[i].dir);
    }

    fclose(fp);
}

void list(struct file data) {
    for (int i = 0; i < data.lineCount; i++) {
        printf("[%d] - \"%s\": %s\n", i+1, data.entries[i].name, data.entries[i].dir);
    }
}

int isvalid (char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (! (((str[i] >= '0') && (str[i] <= '9')) || 
            ((str[i] >= 'a') && (str[i] <= 'z')) || 
            (str[i] == ' ')                      || 
            ((str[i] >= 'A') && (str[i] <= 'Z')))) {
        return 0; 
        }
    }
    return 1;
}