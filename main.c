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

int isvalid (char *str);


int main(int argc, char **argv) {

    struct file data = readFile();
    for (int i = 0; i < data.lineCount; i++){
        printf("%s: %s\n", data.entries[i].name, data.entries[i].dir);
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