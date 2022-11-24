#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define NUM_LINES 16
#define NAME_LENGTH 16
#define DIR_LENGTH 256
#define SAVEFILE "./.dirsave"

struct dirEntry {
    char name[NAME_LENGTH];
    char dir[DIR_LENGTH];
};

struct file {
    int lineCount;
    struct dirEntry entries[sizeof(char *) * NUM_LINES];
};


struct file readFile();

void writeFile(struct file data);

void list(struct file data);

int isvalid (char *str);


void usage(char *path) {
    printf("\nUsage: %s [COMMANDS]\n\n\t%s store \tStores current directory with an incremented number as the key\n\t\t-a \"NAME\"  Adds the current directory with NAME as the key\n\t\t-d \"NAME\"  Deletes directory with key NAME\n\t\t-l         Lists all stored directories and keys\n\n\t%s move \tChanges directory to first stored value \n\t\t-n \"NAME\"  Changes directory to stored value with key NAME\n\n", path, path, path);
}

int main(int argc, char **argv) {

    char *path = argv[0];

    if (argc == 1) {
        usage(path);
        return 1;
    }

        char *name = argv[1];

        if (strcmp(name, "store") == 0){
            if (strcmp(argv[2], "-a") == 0){

                if (argv[3] == NULL){
                    printf("%s add -a must have a NAME arguement\n", path);
                    return 1;
                }

                if (!isvalid(argv[3])){
                    printf("Invalid arguement for %s add -a: \"%s\"\n", path, argv[3]);
                    return 1;
                }

                struct file data = readFile();

                char cwd[DIR_LENGTH];
                if (getcwd(cwd, sizeof(cwd)) == NULL){
                    printf("Failed to get current woking directory");
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

            if (strcmp(argv[3], "-l") == 0){
                struct file data = readFile();
                list(data);    
                return 0;
            }
        }            
}

struct file readFile() {
    FILE *fp = fopen(SAVEFILE, "r");
    if (!fp) {
        printf("Error opening file %s\n", SAVEFILE);
        struct file ret;
        return ret; // exit here
    }
    struct file fileContents;
    
    fileContents.lineCount = 0;

    char line[DIR_LENGTH];

    while (fgets(line, DIR_LENGTH, fp) != NULL){
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

void writeFile(struct file data){
    FILE *fp = fopen(SAVEFILE, "w");
    
    for (int i = 0; i < data.lineCount; i++){
        fprintf(fp,"%s|%s\n", data.entries[i].name, data.entries[i].dir);
    }

    fclose(fp);
}

void list(struct file data){
    for (int i = 0; i < data.lineCount; i++){
        printf("[%d] - \"%s\": %s\n", i+1, data.entries[i].name, data.entries[i].dir);
    }
}

int isvalid (char *str){
    for (int i = 0; i < strlen(str); i++){
        if (! (((str[i] >= '0') && (str[i] <= '9')) || 
            ((str[i] >= 'a') && (str[i] <= 'z')) || 
            (str[i] == ' ')                      || 
            ((str[i] >= 'A') && (str[i] <= 'Z')))) {
        return 0; 
        }
    }
    return 1;
}