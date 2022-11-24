#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUM_LINES 16
#define NAME_LENGTH 16
#define LINE_LENGTH 256
#define SAVEFILE "./.dirsave"

struct dirEntry {
    char name[NAME_LENGTH];
    char dir[LINE_LENGTH];
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
    // printf("\nUsage: %s [OPTIONS]\n\n\t-r : Range (-r < x < r) \n\t-d : Domain (0 < x < d) (in π)\n\t-x : X unit size\n\t-y : Y unit size\n\n", path);
}

int main(int argc, char **argv) {


    if (argc == 1) {
        usage(argv[0]);
        return 1;
    }

    for (int i=1; i<argc; i+=2) {
        char *name = argv[i];

        if (strcmp(name, "add") == 0){
            if (isvalid(argv[i+1])){
                struct file data = readFile();

                struct dirEntry newDir;
                strcpy(newDir.name, argv[i+1]);
                strcpy(newDir.dir, "this is a directory");

                data.entries[data.lineCount] = newDir;
                data.lineCount++;

                writeFile(data);
                printf("Added \"%s\": %s\n", data.entries[data.lineCount-1].name, data.entries[data.lineCount-1].dir);
            } else {
                printf("Invalid arguement for add: \"%s\"\n", argv[i+1]);
            }
        }

        if (strcmp(name, "list") == 0){
            struct file data = readFile();
            list(data);    
        }

    }
    return 0;
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

    char line[LINE_LENGTH];

    while (fgets(line, LINE_LENGTH, fp) != NULL){
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