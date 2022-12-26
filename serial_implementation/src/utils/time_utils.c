#include "time_utils.h"

double TimeUtils_lastTimeStamp = 0;
double TimeUtils_lastElapsedTime = 0;
int TimeUtils_indexOfFile = -1;
char* TimeUtils_lastFilename;

void takeTime() {
    // double end = MPI_Wtime();
    // TimeUtils_lastElapsedTime = end - TimeUtils_lastTimeStamp;
    // TimeUtils_lastTimeStamp = end;
}

void printTime(char* label) {
    printf("%s: %f\n", label, TimeUtils_lastElapsedTime);
}

double getTime() {
    return TimeUtils_lastElapsedTime;
}

void setTime(double time) {
    TimeUtils_lastElapsedTime = time;
}

// function to get number of lines in a file
int getNumberOfLines(FILE *fp) {
    int lines = 0;
    char * line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1)
        lines++;

    return lines;
}

void saveTime(char* filename, char* label) {
    if (TimeUtils_lastFilename != filename) {
        TimeUtils_indexOfFile = -1;
        TimeUtils_lastFilename = filename;
    }

    // open csv file
    FILE *fp = fopen(filename, "a+");
    if (fp == NULL) {
        // create file if it doesn't exist
        printf("Creating file\n");
        fp = fopen(filename, "w");
        TimeUtils_indexOfFile = 0;
        if (fp == NULL) {
            printf("Error opening file\n");
            exit(1);
        }
    }

    // read index of last row from file
    if (TimeUtils_indexOfFile == -1)
        TimeUtils_indexOfFile = getNumberOfLines(fp);

    // write to csv file
    fprintf(fp, "%d,%s,%f\n", TimeUtils_indexOfFile, label, TimeUtils_lastElapsedTime);
    TimeUtils_indexOfFile++; 

    // close csv file
    fclose(fp);
}
